import decimal
from math import log2, floor

decimal.getcontext().prec = 72
prec = decimal.Decimal(str(2**72))
#the scaling pdt value of Gaussian distribution with (mu,sigma) = (0,0.18205), scaling value = 2^72.
scale_pdt = ['1697680241746640300030', '1459943456642912959616', '928488355018011056515',
             '436693944817054414619',  '151893140790369201013', '39071441848292237840',
             '7432604049020375675', '1045641569992574730', '108788995549429682', '8370422445201343',
             '476288472308334', '20042553305308', '623729532807', '14354889437', '244322621',
             '3075302', '28626', '197', '1'    ]
scale_pdt = [decimal.Decimal(_) for _ in scale_pdt]
pdt = [_/prec for _ in scale_pdt]
print("pdt:", pdt)


#Verify the value in cdt
scale_cdt = [sum(pdt[:i])*prec for i in range(len(pdt))]

print("scale_cdt:", scale_cdt)



cocdt = [scale_pdt[z]/sum(scale_pdt[z:]) for z in range(len(scale_pdt))]
print("cocdt:", cocdt)
print(sum(cocdt))


scale_cocdt = [str(round(scale_pdt[z]/sum(scale_pdt[z:])*prec)) for z in range(len(scale_pdt))]
print("scale_cocdt:", scale_cocdt)

prec = decimal.Decimal(str(2**72))
scale_cocdt = [str(round(scale_pdt[z]/sum(scale_pdt[z:])*prec)) for z in range(len(scale_pdt))]
print("scale_cocdt:", scale_cocdt)



rcocdt = [decimal.Decimal('1') - scale_pdt[z]/sum(scale_pdt[z:]) for z in range(len(scale_pdt))]
print("rcocdt:", rcocdt)
min_differ = 1.
for i in range(len(rcocdt)-1):
    if(abs(rcocdt[i] -rcocdt[i+1])<min_differ):
            min_differ = abs(rcocdt[i] -rcocdt[i+1])

new_prec = floor(log2(round(decimal.Decimal('1')/min_differ)))+1
print("new_prec: ", new_prec," bit")
new_prec = decimal.Decimal(str(2**new_prec))


scale_rcocdt = [ str(round(rcocdt[z]*new_prec)) for z in range(len(cocdt))]
print("scale_rcocdt:", scale_rcocdt )


