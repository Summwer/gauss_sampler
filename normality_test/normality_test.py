import saga




def normality_test(mu, sigma, file_name):
    f = open(file_name,"r",encoding='UTF-8')
    data = f.read()
    data = data.split(" ")
    data.remove('')
    data = [int(_) for _ in data]
    res = saga.UnivariateSamples(mu, sigma, data)
    print(res)
    
mu = -1.
sigma = 1.7
print("Normality Test for Karney:")
normality_test(mu, sigma, "../Karney/output.txt")

print("Normality Test for example:")
normality_test(mu, sigma, "../example/sampler/output.txt")

print("Normality Test for pre17:")
normality_test(mu, sigma, "../pre17/output.txt")

print("Normality Test for Knuth-Yao:")
normality_test(mu, sigma, "../Knuth-Yao/output.txt")