import saga
import matplotlib.pyplot as plt
from decimal import Decimal, getcontext


e = Decimal('2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274')
getcontext().prec = 150


def read_file(file_name):
    f = open(file_name,"r",encoding='UTF-8')
    data = f.read()
    data = data.split("\n")
    
    data[0] = data[0].split(" ")
    center = float(data[0][0])
    sigma = float(data[0][1])
    print("center = %.3f, sigma = %.3f" %(center, sigma))
    
    data = data[1]
    data = data.split(" ")
    data.remove('')
    data = [int(_) for _ in data]
    return data, center, sigma

def draw_gauss_dist(file_name):
    data, center, sigma = read_file(file_name)
    
    range_num = max(max(data), -min(data))+10
    plt.figure(figsize=(10, 5), dpi=600)
    xs =[x + center for x in list(range(-range_num,range_num+1))]
    pdt = [e**(-Decimal(xs[i]-center)**Decimal(2)/Decimal(2)/Decimal(sigma)**Decimal(2)) for i in range(len(xs))]
    sum_pdt = sum(pdt)
    pdt = [_/sum_pdt for _ in pdt]
    
    
    numprob_dic ={}
    for i in data:
        if(i not in numprob_dic):
            numprob_dic[i] = 1
        else:
            numprob_dic[i] += 1
    
    
    for i in numprob_dic:
        numprob_dic[i] /= len(data)

    
    test_xs = sorted(list(numprob_dic.keys()))
    test_probs = [numprob_dic[i] for i in test_xs]
    
    plt.plot(test_xs, test_probs, color = "orange", label = "test_value")
    plt.plot(xs, pdt, color = "red", label = "theo_value")
    plt.legend()
    plt.savefig("Ver:DiscreteGaussDist(%.2f,%.2f).png" %(center, sigma))
    
   
for testNum in range(3,4):
    print("Normality Test for Sampler%d:" %testNum)
    draw_gauss_dist("../Integer_Gaussian_sampler/output-%d.txt" %testNum)
    # normality_test("../Knuth-Yao/output-%d.txt" %testNum)
