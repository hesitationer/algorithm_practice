from numpy import atleast_2d, reshape, zeros, newaxis, dot, exp, pi, sqrt, \
     ravel, power, atleast_1d, squeeze, sum, transpose
from scipy.stats import kde
from scipy import linalg, special
import matplotlib.pyplot as pyplot
import matplotlib.mlab as mlab
import numpy as np
import math
import my_kde

count  = 0
# caculate gaussian value
def gaussian_1d(x,mu,variance,bandwidth):
    global count
    a = 1./sqrt(2*pi*variance)
    b = (x-mu)*(x-mu)
    #NOTE: The bandwidth has square
    c = 2*variance*power(bandwidth,2)

    result = a*exp(-b/c)

    #if count == 0:
        #print 'x_pos: '
        #print x
        #print 'mu: ', mu
        #print 'my 2*pi*variance: ',2*pi*variance
        #print 'a: ', a
        #print 'x - mu: '
        #print x - mu
        #print 'b/c: '
        #print b/c
        #count = 1
    return result

# calculate the bandwidth
def scotts_factor(data_number, data_dimension):
    return power(data_number,-1./(data_dimension + 4))

def caculate_variance(dataset):
    N = len(dataset)
    print N
    mean = sum(dataset)/N
    variance = sum((dataset - mean)*(dataset - mean))/(N-1)
    print 'my variance: ',variance

    return variance

def gaussian_1d_kde(dataset,x_pos):
    #bandwidth
    N = len(dataset)
    dim = 1
    bandwidth = scotts_factor(N,1)
    print 'bandwidth: ', bandwidth

    # variance
    variance = caculate_variance(dataset)
    
    result = zeros((N,),dtype=np.float)
    for i in range(N):
        #result[i] = sum(gaussian_1d(x_pos,dataset[i],variance,bandwidth))
        result = result + gaussian_1d(x_pos,dataset[i],variance,bandwidth)

    #NOTE: DO NOT forget the bandwidth
    result = result/(N*bandwidth)

    return result

def kde_demo():
    dataset = np.random.normal(0,1,100)

    # my kde
    x = np.linspace(dataset.min(),dataset.max(),100)
    result = gaussian_1d_kde(dataset,x)

    pyplot.plot(x,result,'black')

    # standard scipy kde 
    #kde_object = kde.gaussian_kde(dataset)
    #pyplot.plot(x,kde_object(x),'red')

    # scipy kde with debug
    kde_object = my_kde.lib_gaussian_kde(dataset)
    pyplot.plot(x,kde_object(x),'*-')
    print 'factor: ',kde_object.factor


    pyplot.ylim(-0.02,0.9)
    pyplot.show()

if __name__=="__main__":
    kde_demo()







