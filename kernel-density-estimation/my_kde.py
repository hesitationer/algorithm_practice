from numpy import atleast_2d, reshape, zeros, newaxis, dot, exp, pi, sqrt, \
     ravel, power, atleast_1d, squeeze, sum, transpose
from scipy.stats import kde
from scipy import linalg, special
import matplotlib.pyplot as pyplot
import matplotlib.mlab as mlab
import numpy as np
import math

# Show a simple example of how to do KDE (kernel density estimation)
# Reference: http://en.wikipedia.org/wiki/Kernel_density_estimation#Definition

class gaussian:
    sigma = np.sqrt(2.25)
    mu    = [0,0,0,0,0,0]

sample = [-2.1, -1.3, -0.4, 1.9, 5.1, 6.2]

# G(x) = a*e^(b/c)
def eric_gaussian(x,mu,sigma):
    a = 1./(np.sqrt(2*math.pi*np.power(sigma,2)))
    b = -(x-mu)*(x-mu)
    c = 2*sigma*sigma

    return a*np.exp(b/c)

def plot_gaussian():
    global sample
    x = np.arange(-10,10,0.1)
    print 'np.arange type is:', type(x)

    #pyplot.hist(x, bins=6, normed=True)
    pyplot.plot(x,eric_gaussian(x,sample[0],gaussian.sigma),'r--')
    #use mlib to check if my-draw is right. :-P
    pyplot.plot(x,mlab.normpdf(x,sample[0],gaussian.sigma),'g*')
    pyplot.plot(x,eric_gaussian(x,sample[1],gaussian.sigma), 'r--')
    pyplot.plot(x,eric_gaussian(x,sample[2],gaussian.sigma), 'r--')
    pyplot.plot(x,eric_gaussian(x,sample[3],gaussian.sigma), 'r--')
    pyplot.plot(x,eric_gaussian(x,sample[4],gaussian.sigma), 'r--')
    pyplot.plot(x,eric_gaussian(x,sample[5],gaussian.sigma), 'r--')

    estimated_value = (eric_gaussian(x,sample[0],gaussian.sigma) + 
            eric_gaussian(x,sample[1],gaussian.sigma) + 
            eric_gaussian(x,sample[2],gaussian.sigma) + 
            eric_gaussian(x,sample[3],gaussian.sigma) + 
            eric_gaussian(x,sample[4],gaussian.sigma) + 
            eric_gaussian(x,sample[5],gaussian.sigma))

    pyplot.plot(x, estimated_value, 'b-')

    #plot the vertical line
    for marker in sample:
        pyplot.axvline(x=marker,color='g',ls='--')

    pyplot.ylim(-0.02,0.8)
    pyplot.show()

def choose_bandwith(sample_used):
    #calculate the bandwidth
    sum_all = 0
    sum_square = 0
    for item in sample_used:
        sum_all = sum_all + item
        sum_square = sum_square + item*item

    #print 'sum_all, sum_square', sum_all, sum_square

    mean = sum_all/len(sample_used)
    #print 'len is', len(sample_used)
    #print 'mean is:',mean

    stand_deviation = np.power((sum_square/len(sample_used) - mean*mean),0.5)
    #print 'stand_deviation is:',stand_deviation
    #print 'np.std is', np.std(sample_used)

    bandth = 1.06*stand_deviation*np.power(len(sample_used), -0.2)
    #print 'bandth is:',bandth

    return bandth


def gaussian_kde_with_bandth(x,mu,sigma,bandth):
    print 'mu:\n',mu
    print 'x:\n',x
    print 'bandth:\n',bandth
    a = 1./(np.sqrt(2*math.pi*np.power(sigma,2)))
    b = -(x-mu)*(x-mu)/bandth
    print 'x-mu\n',x-mu
    c = 2*sigma*sigma

    return a*np.exp(b/c)


def estimate_standard_norm():

    #set the x-range
    x = np.arange(-4.5,4.5,0.1)

    #generate 100 random number
    random_sample = np.random.normal(0,1,100)
    #print random_sample
    
    #mark these random number on x-coordinate
    for marker in random_sample:
        pyplot.axvline(x=marker,color='b',ymax=0.02)

    #estimate with bandth 0.05
    estimate_value = 0.0
    for marker in random_sample:
        estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,0.05)
    estimate_value = estimate_value/(len(random_sample)*0.05)
    #plot the estimate distribution
    pyplot.plot(x,estimate_value,'r--')

    #estimate with bandth 2
    estimate_value = 0.0
    for marker in random_sample:
        estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,2)
    estimate_value = estimate_value/(len(random_sample)*2)
    #plot the estimate distribution
    pyplot.plot(x,estimate_value,'g--')

    #estimate with bandth 0.337
    estimate_value = 0.0
    for marker in random_sample:
        estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,0.337)
    estimate_value = estimate_value/(len(random_sample)*0.337)
    #plot the estimate distribution
    pyplot.plot(x,estimate_value,'black', ls='--')

    #estimate with  Silverman's rule of thumb
    estimate_value = 0.0
    n = 100
    d = 1
    first = n*(d+2)/4.
    second = (-1./(d+4))
    silver_factor = np.power(first,second)
    for marker in random_sample:
        estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,silver_factor)
    estimate_value = estimate_value/(len(random_sample)*silver_factor)
    #estimate_value = estimate_value/choose_bandwith(random_sample)
    #plot the estimate distribution
    pyplot.plot(x,estimate_value,'yellow',ls='-')


    #estimate with scotts_factor
    estimate_value = 0.0
    n = 100
    d = 1
    first = n
    second = (-1./(d+4))
    scotts_factor = np.power(first,second)
    for marker in random_sample:
        estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,scotts_factor)
    estimate_value = estimate_value/(len(random_sample)*scotts_factor)
    #estimate_value = estimate_value/choose_bandwith(random_sample)
    #plot the estimate distribution
    pyplot.plot(x,estimate_value,'purple',ls='-')

    #the scipy lib result
    lib_result = kde.gaussian_kde(random_sample)
    xgrid = np.linspace(random_sample.min(),random_sample.max(),100)
    pyplot.plot(xgrid,lib_result(xgrid),'red',ls=':',lw=2)

    print lib_result

    #plot the standard gaussian (0,1)
    pyplot.plot(x,gaussian_kde_with_bandth(x,0,1,1),'grey')
    pyplot.plot(x,mlab.normpdf(x,0,1),'g*')

    pyplot.ylim(-0.02,0.9)
    pyplot.show()

def test_my_kde():

    sample = np.random.normal(0,1,100)

    for marker in sample:
        pyplot.axvline(x=marker,color='b',ymax=0.02)

    #x = np.arange(sample.min(),sample.max(),0.1)
    x = np.linspace(sample.min(),sample.max(),100)


    #the scipy lib result
    lib_result = kde.gaussian_kde(sample)
    xgrid = np.linspace(sample.min(),sample.max(),100)
    print 'xgrid\n', xgrid
    pyplot.plot(xgrid,lib_result(xgrid),'red',ls=':',lw=2)

    #plot the standard gaussian (0,1)
    pyplot.plot(x,gaussian_kde_with_bandth(x,0,1,1),'grey')
    pyplot.plot(x,mlab.normpdf(x,0,1),'g*')


    # my kde
    estimate_value = 0.0
    scotts_factor = power(100,-1./(1+4))
    
    #for i in range(len(sample)):
    #   diff = sample[:,i,newaxis] - points
    #print diff

    #pyplot.plot(x,estimate_value,'purple',ls='-')


    pyplot.ylim(-0.02,0.9)
    pyplot.show()

def test_add_list():
    sample_0 = np.random.normal(0,1,2)
    sample_1 = np.random.normal(0,1,2)

    print sample_0,'\n'
    print sample_1,'\n'
    print sample_0 + sample_1,'\n'

def test_atleast():
    sample_0 = np.random.normal(0,1,100)

    points = atleast_2d(sample_0)

    d,m = points.shape

    print type(sample_0)
    print sample_0
    print type(points)
    print points
    print d,m

class lib_gaussian_kde(object):
    def __init__(self, dataset, bw_method=None):
        self.dataset = atleast_2d(dataset)
        if not self.dataset.size > 1:
            raise ValueError("`dataset` input should have multiple elements.")

        self.d, self.n = self.dataset.shape
        self.set_bandwidth(bw_method=bw_method)

    def evaluate(self, points):
        points = atleast_2d(points)

        d, m = points.shape
        if d != self.d:
            if d == 1 and m == self.d:
                # points was passed in as a row vector
                points = reshape(points, (self.d, 1))
                m = 1
            else:
                msg = "points have dimension %s, dataset has dimension %s" % (d,
                    self.d)
                raise ValueError(msg)

        result = zeros((m,), dtype=np.float)

        print '(m,n): ',m,self.n

        if m >= self.n:
            # there are more points than data, so loop over data
            for i in range(self.n):
                diff = self.dataset[:, i, newaxis] - points
                tdiff = dot(self.inv_cov, diff)
                energy = sum(diff*tdiff,axis=0) / 2.0
                result = result + exp(-energy)
        else:
            # loop over points
            for i in range(m):
                diff = self.dataset - points[:, i, newaxis]
                tdiff = dot(self.inv_cov, diff)
                energy = sum(diff * tdiff, axis=0) / 2.0
                result[i] = sum(exp(-energy), axis=0)


        result = result / self._norm_factor

        return result

    __call__ = evaluate


    def scotts_factor(self):
        print "scotts_factor"
        return power(self.n, -1./(self.d+4))

    #  Default method to calculate bandwidth, can be overwritten by subclass
    covariance_factor = scotts_factor


    def set_bandwidth(self,bw_method=None):
        print "set_bandwidth"
        if bw_method is None:
            pass
        elif bw_method == 'scott':
            self.covariance_factor = self.scotts_factor
        elif bw_method == 'silverman':
            self.covariance_factor = self.silverman_factor
        elif np.isscalar(bw_method) and not isinstance(bw_method, string_types):
            self._bw_method = 'use constant'
            self.covariance_factor = lambda: bw_method
        elif callable(bw_method):
            self._bw_method = bw_method
            self.covariance_factor = lambda: self._bw_method(self)
        else:
            msg = "`bw_method` should be 'scott', 'silverman', a scalar " \
                    "or a callable."
            raise ValueError(msg)

        self._compute_covariance()


    def _compute_covariance(self):
        self.factor = self.covariance_factor()
        # Cache covariance and inverse covariance of the data
        if not hasattr(self, '_data_inv_cov'):
            self._data_covariance = atleast_2d(np.cov(self.dataset, rowvar=1,
                                               bias=False))
            self._data_inv_cov = linalg.inv(self._data_covariance)

        self.covariance = self._data_covariance * self.factor**2
        self.inv_cov = self._data_inv_cov / self.factor**2
        self._norm_factor = sqrt(linalg.det(2*pi*self.covariance)) * self.n



def debug_lib_kde():

    #sample = np.random.normal(0,1,3)
    sample_lsit = [21,17,9,29]
    sample = np.asarray(sample_lsit)
    print 'sample'
    print sample

    xgrid = np.linspace(sample.min(),sample.max(),3)
    lib_result = lib_gaussian_kde(sample)
    pyplot.plot(xgrid,lib_result(xgrid),'red',ls=':',lw=2)

    #plot the standard gaussian (0,1)
    #pyplot.plot(xgrid,gaussian_kde_with_bandth(xgrid,0,1,1),'grey')
    #pyplot.plot(xgrid,mlab.normpdf(xgrid,0,1),'g*')


    pyplot.ylim(-0.02,0.9)
    pyplot.show()

def test_2d_kde():
    mean = [0,0]
    cov = [[1,1],[2,2]]
    sample = np.random.multivariate_normal(mean,cov, 100)

    print sample.shape
    print atleast_2d(sample).shape

def test_dot_product():
    v1 = [[1,2],[3,4]]
    v2 = [[1,2,3,4],[4,5,6,7]]
    v2 = [[1,2],[3,4],[5,6],[7,8]]
    v3 = dot(v1,v2)
    #v4 = np.cross(v1,v2)

    print v1
    print v2
    print v3
    #print v4

def test_covariance():
    
    #X1 = np.random.normal(0,3,50)
    #X2 = np.random.normal(2,4,50)
    X1 = [1,2,3]
    X2 = [3,6,9]
    X3 = [8,10,12]

    X = np.asarray([X1,X2,X3])
    Y = np.asarray([X1,X3])

    print type(X)
    print X.shape
    print X

    print np.cov(X1)
    print np.cov(X2)
    print np.cov(X3)
    print np.cov(X,rowvar=1,bias=False)
    print np.cov(Y,rowvar=1,bias=False)
  

def test_determinant():
    X1 = [1,2,3]
    X2 = [3,6,9]

if __name__=="__main__":
    #plot_gaussian()
    #choose_bandwith()
    #estimate_standard_norm()

    #test_my_kde()   
    #test_add_list()

    #test_atleast()
    #test_dot_product()
    test_2d_kde()

    #debug_lib_kde()

    #test_covariance()

