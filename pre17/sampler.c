#include "sampler.h"

static const uint32_t pre17_dist[] = { //18 elements for all, and each element in 16 bit (actually 10bit)
    656u, 530u, 416u, 321u, 245u, 185u, 139u, 104u, 77u, 57u, 43u, 
    32u, 23u, 17u, 13u, 10u, 7u, 5u
}; //rcocdt

/*
 * Sample an integer value along a half-gaussian distribution centered
 * on zero and standard deviation 1.8205, with a precision of 72 bits.
 */
int
pre17_gaussian0_sampler(prng *p)
{

    uint16_t u, lo, w; 
    int z, i = 0;

    /*
     * Get a random 10-bit value
     */
    lo = prng_get_u16(p); //Generate a 16-bit random value
    u = lo & 0x3FF;  //10bit = 3FF, lo[-10:], random 10-bit number
    
    /*
     * Sampled value is z, such that v is lower than the first
     * z elements of the table. [Pre17]
     */
    z = 0;
    w = pre17_dist[i];
    // printf("u = %d\n", u);
    while((u - w) >> 16){//If u<w, then (u - w) >> 16 = 1; Else, (u - w) >> 16 = 0
        z += 1; 
        lo = prng_get_u16(p); //Generate a 16-bit random value
        u = lo & 0x3FF;  //10bit = 3FF, lo[-10:], random 10-bit number
        // printf("p->ptr:%zu\n", p->ptr);
        w = pre17_dist[++i];
    }
    return z;
}

/*
 * Sample a bit with probability exp(-x) for some x >= 0.
 */
static int
BerExp(prng *p, fpr x, fpr ccs)
{
    int s, i;
    fpr r;
    uint32_t sw, w;
    uint64_t z;

    /*
     * Reduce x modulo log(2): x = s*log(2) + r, with s an integer,
     * and 0 <= r < log(2). Since x >= 0, we can use fpr_trunc().
     */
    s = (int)fpr_trunc(fpr_mul(x, fpr_inv_log2));
    r = fpr_sub(x, fpr_mul(fpr_of(s), fpr_log2));

    /*
     * It may happen (quite rarely) that s >= 64; if sigma = 1.2
     * (the minimum value for sigma), r = 0 and b = 1, then we get
     * s >= 64 if the half-Gaussian produced a z >= 13, which happens
     * with probability about 0.000000000230383991, which is
     * approximatively equal to 2^(-32). In any case, if s >= 64,
     * then BerExp will be non-zero with probability less than
     * 2^(-64), so we can simply saturate s at 63.
     */
    sw = (uint32_t)s;
    sw ^= (sw ^ 63) & -((63 - sw) >> 31);
    s = (int)sw;

    /*
     * Compute exp(-r); we know that 0 <= r < log(2) at this point, so
     * we can use fpr_expm_p63(), which yields a result scaled to 2^63.
     * We scale it up to 2^64, then right-shift it by s bits because
     * we really want exp(-x) = 2^(-s)*exp(-r).
     *
     * The "-1" operation makes sure that the value fits on 64 bits
     * (i.e. if r = 0, we may get 2^64, and we prefer 2^64-1 in that
     * case). The bias is negligible since fpr_expm_p63() only computes
     * with 51 bits of precision or so.
     */
    z = ((fpr_expm_p63(r, ccs) << 1) - 1) >> s;

    /*
     * Sample a bit with probability exp(-x). Since x = s*log(2) + r,
     * exp(-x) = 2^-s * exp(-r), we compare lazily exp(-x) with the
     * PRNG output to limit its consumption, the sign of the difference
     * yields the expected result.
     */
    i = 64;
    do {
        i -= 8;
        w = prng_get_u8(p) - ((uint32_t)(z >> i) & 0xFF);
    } while (!w && i > 0);
    return (int)(w >> 31);
}

/*
 * The sampler produces a random integer that follows a discrete Gaussian
 * distribution, centered on mu, and with standard deviation sigma. The
 * provided parameter isigma is equal to 1/sigma.
 *
 * The value of sigma MUST lie between 1 and 2 (i.e. isigma lies between
 * 0.5 and 1); in Falcon, sigma should always be between 1.2 and 1.9.
 */
int
sampler(void *ctx, fpr mu, fpr isigma)
{
    sampler_context *spc;
    int s;
    fpr r, dss, ccs;

    spc = ctx;

    /*
     * Center is mu. We compute mu = s + r where s is an integer
     * and 0 <= r < 1.
     */
    s = (int)fpr_floor(mu);
    r = fpr_sub(mu, fpr_of(s));

    /*
     * dss = 1/(2*sigma^2) = 0.5*(isigma^2).
     */
    dss = fpr_half(fpr_sqr(isigma));

    /*
     * ccs = sigma_min / sigma = sigma_min * isigma.
     */
    ccs = fpr_mul(isigma, spc->sigma_min);

    /*
     * We now need to sample on center r.
     */
    for (;;) {
        int z0, z, b;
        fpr x;

        /*
         * Sample z for a Gaussian distribution. Then get a
         * random bit b to turn the sampling into a bimodal
         * distribution: if b = 1, we use z+1, otherwise we
         * use -z. We thus have two situations:
         *
         *  - b = 1: z >= 1 and sampled against a Gaussian
         *    centered on 1.
         *  - b = 0: z <= 0 and sampled against a Gaussian
         *    centered on 0.
         */
        z0 = pre17_gaussian0_sampler(&spc->p);
        b = (int)prng_get_u8(&spc->p) & 1;
        z = b + ((b << 1) - 1) * z0;

        /*
         * Rejection sampling. We want a Gaussian centered on r;
         * but we sampled against a Gaussian centered on b (0 or
         * 1). But we know that z is always in the range where
         * our sampling distribution is greater than the Gaussian
         * distribution, so rejection works.
         *
         * We got z with distribution:
         *    G(z) = exp(-((z-b)^2)/(2*sigma0^2))
         * We target distribution:
         *    S(z) = exp(-((z-r)^2)/(2*sigma^2))
         * Rejection sampling works by keeping the value z with
         * probability S(z)/G(z), and starting again otherwise.
         * This requires S(z) <= G(z), which is the case here.
         * Thus, we simply need to keep our z with probability:
         *    P = exp(-x)
         * where:
         *    x = ((z-r)^2)/(2*sigma^2) - ((z-b)^2)/(2*sigma0^2)
         *
         * Here, we scale up the Bernouilli distribution, which
         * makes rejection more probable, but makes rejection
         * rate sufficiently decorrelated from the Gaussian
         * center and standard deviation that the whole sampler
         * can be said to be constant-time.
         */
        x = fpr_mul(fpr_sqr(fpr_sub(fpr_of(z), r)), dss); //dss = 1/(2sigma^2)
        x = fpr_sub(x, fpr_mul(fpr_of(z0 * z0), fpr_inv_2sqrsigma0));
        if (BerExp(&spc->p, x, ccs)) {
            /*
             * Rejection sampling was centered on r, but the
             * actual center is mu = s + r.
             */
            return s + z;
        }
    }
}