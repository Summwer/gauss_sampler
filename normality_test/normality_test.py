import saga




def normality_test(file_name):
    f = open(file_name,"r",encoding='UTF-8')
    data = f.read()
    data = data.split("\n")
    
    data[0] = data[0].split(" ")
    mu = float(data[0][0])
    sigma = float(data[0][1])
    print("center = %.3f, sigma = %.3f" %(mu, sigma))
    
    data = data[1]
    data = data.split(" ")
    data.remove('')
    data = [int(_) for _ in data]
    res = saga.UnivariateSamples(mu, sigma, data)
    print(res)
   
for testNum in range(1,5):
    print("Normality Test for Sampler%d:" %testNum)
    normality_test("../Integer_Gaussian_sampler/output-%d.txt" %testNum)
    # normality_test("../Knuth-Yao/output-%d.txt" %testNum)
