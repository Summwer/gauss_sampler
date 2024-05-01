#ifndef FALCON_INNER_H__
#define FALCON_INNER_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
 * "Naming" macro used to apply a consistent prefix over all global
 * symbols.
 */
#ifndef FALCON_PREFIX
#define FALCON_PREFIX   falcon_sampler
#endif
#define Zf(name)             Zf_(FALCON_PREFIX, name)
#define Zf_(prefix, name)    Zf__(prefix, name)
#define Zf__(prefix, name)   prefix ## _ ## name

/*
 * "Naming" macro used to apply a consistent prefix over all global
 * symbols.
 */
#ifndef FALCON_PREFIX
#define FALCON_PREFIX   falcon_sampler
#endif
#define Zf(name)             Zf_(FALCON_PREFIX, name)
#define Zf_(prefix, name)    Zf__(prefix, name)
#define Zf__(prefix, name)   prefix ## _ ## name


/*
 * Some computations with floating-point elements, in particular
 * rounding to the nearest integer, rely on operations using _exactly_
 * the precision of IEEE-754 binary64 type (i.e. 52 bits). On 32-bit
 * x86, the 387 FPU may be used (depending on the target OS) and, in
 * that case, may use more precision bits (i.e. 64 bits, for an 80-bit
 * total type length); to prevent miscomputations, we define an explicit
 * function that modifies the precision in the FPU control word.
 *
 * set_fpu_cw() sets the precision to the provided value, and returns
 * the previously set precision; callers are supposed to restore the
 * previous precision on exit. The correct (52-bit) precision is
 * configured with the value "2". On unsupported compilers, or on
 * targets other than 32-bit x86, or when the native 'double' type is
 * not used, the set_fpu_cw() function does nothing at all.
 */
#if defined __GNUC__ && defined __i386__
static inline unsigned
set_fpu_cw(unsigned x)
{
	unsigned short t;
	unsigned old;

	__asm__ __volatile__ ("fstcw %0" : "=m" (t) : : );
	old = (t & 0x0300u) >> 8;
	t = (unsigned short)((t & ~0x0300u) | (x << 8));
	__asm__ __volatile__ ("fldcw %0" : : "m" (t) : );
	return old;
}
#elif defined _M_IX86
static inline unsigned
set_fpu_cw(unsigned x)
{
	unsigned short t;
	unsigned old;

	__asm { fstcw t }
	old = (t & 0x0300u) >> 8;
	t = (unsigned short)((t & ~0x0300u) | (x << 8));
	__asm { fldcw t }
	return old;
}
#else
static inline unsigned
set_fpu_cw(unsigned x)
{
    return x;
}
#endif

/*
 * If using the native 'double' type but not AVX2 code, on an x86
 * machine with SSE2 activated for maths, then we will use the
 * SSE2 intrinsics.
 */
#if defined __GNUC__ && defined __SSE2_MATH__
#include <immintrin.h>
#endif

/*
 * For optimal reproducibility of values, we need to disable contraction
 * of floating-point expressions; otherwise, on some architectures (e.g.
 * PowerPC), the compiler may generate fused-multiply-add opcodes that
 * may round differently than two successive separate opcodes. C99 defines
 * a standard pragma for that, but GCC-6.2.2 appears to ignore it,
 * hence the GCC-specific pragma (that Clang does not support).
 */
#if defined __clang__
#pragma STDC FP_CONTRACT OFF
#elif defined __GNUC__
#pragma GCC optimize ("fp-contract=off")
#endif

/*
 * MSVC 2015 does not know the C99 keyword 'restrict'.
 */
#if defined _MSC_VER && _MSC_VER
#ifndef restrict
#define restrict   __restrict
#endif
#endif


/* ==================================================================== */
/*
 * SHAKE256 implementation (shake.c).
 *
 * API is defined to be easily replaced with the fips202.h API defined
 * as part of PQClean.
 */

typedef struct {
	union {
		uint64_t A[25];
		uint8_t dbuf[200];
	} st;
	uint64_t dptr;
} sampler_shake256_context;

#define sampler_shake256_init      Zf(i_shake256_init)
#define sampler_shake256_inject    Zf(i_shake256_inject)
#define sampler_shake256_flip      Zf(i_shake256_flip)
#define sampler_shake256_extract   Zf(i_shake256_extract)

void Zf(i_shake256_init)(
	sampler_shake256_context *sc);
void Zf(i_shake256_inject)(
	sampler_shake256_context *sc, const uint8_t *in, size_t len);
void Zf(i_shake256_flip)(
	sampler_shake256_context *sc);
void Zf(i_shake256_extract)(
	sampler_shake256_context *sc, uint8_t *out, size_t len);


#include "fpr.h"

/* ==================================================================== */
/*
 * RNG (rng.c).
 *
 * A PRNG based on ChaCha20 is implemented; it is seeded from a SHAKE256
 * context (flipped) and is used for bulk pseudorandom generation.
 * A system-dependent seed generator is also provided.
 */

/*
 * Obtain a random seed from the system RNG.
 *
 * Returned value is 1 on success, 0 on error.
 */
int Zf(get_seed)(void *seed, size_t seed_len);

/*
 * Structure for a PRNG. This includes a large buffer so that values
 * get generated in advance. The 'state' is used to keep the current
 * PRNG algorithm state (contents depend on the selected algorithm).
 *
 * The unions with 'dummy_u64' are there to ensure proper alignment for
 * 64-bit direct access.
 */
typedef struct {
	union {
		uint8_t d[512]; /* MUST be 512, exactly */
		uint64_t dummy_u64;
	} buf;
	size_t ptr;
	union {
		uint8_t d[256];
		uint64_t dummy_u64;
	} state;
	int type;
} prng;

/*
 * Instantiate a PRNG. That PRNG will feed over the provided SHAKE256
 * context (in "flipped" state) to obtain its initial state.
 */
void Zf(prng_init)(prng *p, sampler_shake256_context *src);

/*
 * Refill the PRNG buffer. This is normally invoked automatically, and
 * is declared here only so that prng_get_u64() may be inlined.
 */
void Zf(prng_refill)(prng *p);

/*
 * Get some bytes from a PRNG.
 */
void Zf(prng_get_bytes)(prng *p, void *dst, size_t len);

/*
 * Get a 64-bit random value from a PRNG.
 */
static inline uint64_t
prng_get_u64(prng *p)
{
	size_t u; //8 bytes, 64bit, unsigned long
	
	/*
	 * If there are less than 9 bytes in the buffer, we refill it.
	 * This means that we may drop the last few bytes, but this allows
	 * for faster extraction code. Also, it means that we never leave
	 * an empty buffer.
	 */
	u = p->ptr; //After prng_init, p->ptr = 0.
	//There is no enough value in prng p to generate a 64-bit random value.
	if (u >= (sizeof p->buf.d) - 9) {
		Zf(prng_refill)(p);
		u = 0;
	}
	p->ptr = u + 8; //use 8 bytes in prng p.

	/*
	 * On systems that use little-endian encoding and allow
	 * unaligned accesses, we can simply read the data where it is.
	 */
	return (uint64_t)p->buf.d[u + 0]
		| ((uint64_t)p->buf.d[u + 1] << 8)
		| ((uint64_t)p->buf.d[u + 2] << 16)
		| ((uint64_t)p->buf.d[u + 3] << 24)
		| ((uint64_t)p->buf.d[u + 4] << 32)
		| ((uint64_t)p->buf.d[u + 5] << 40)
		| ((uint64_t)p->buf.d[u + 6] << 48)
		| ((uint64_t)p->buf.d[u + 7] << 56);
}

/*
 * Get an 8-bit random value from a PRNG.
 */
static inline unsigned
prng_get_u8(prng *p)
{
	unsigned v;

	v = p->buf.d[p->ptr ++];
	if (p->ptr == sizeof p->buf.d) {
		Zf(prng_refill)(p);
	}
	return v;
}


/*
 * Get an 16-bit random value from a PRNG.
 */
static inline unsigned
prng_get_u16(prng *p)
{
	unsigned v;

	v = p->ptr; //After prng_init, p->ptr = 0.
	//There is no enough value in prng p to generate a 16-bit random value.
	if (v >= (sizeof p->buf.d) - 3) { //3 = 2+1, 16 bits = 2 bytes
		Zf(prng_refill)(p);
		v = 0;
	}
	p->ptr = v + 2;
	return (uint16_t)p->buf.d[v + 0]
		| ((uint16_t)p->buf.d[v + 1] << 8);
}

typedef struct {
	prng p; 
	fpr sigma_min;
} sampler_context;

int sampler(void *ctx, fpr mu, fpr isigma);

int pre17_gaussian0_sampler(prng *p);

/* ==================================================================== */

#endif
