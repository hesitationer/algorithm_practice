from scipy.stats import kde
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

	print 'sum_all, sum_square', sum_all, sum_square

	mean = sum_all/len(sample_used)
	print 'len is', len(sample_used)
	print 'mean is:',mean

	stand_deviation = np.power((sum_square/len(sample_used) - mean*mean),0.5)
	print 'stand_deviation is:',stand_deviation
	print 'np.std is', np.std(sample_used)

	bandth = 1.06*stand_deviation*np.power(len(sample_used), -0.2)
	print 'bandth is:',bandth

	return bandth


def gaussian_kde_with_bandth(x,mu,sigma,bandth):
	a = 1./(np.sqrt(2*math.pi*np.power(sigma,2)))
	b = -(x-mu)*(x-mu)/bandth
	c = 2*sigma*sigma

	return a*np.exp(b/c)


def estimate_standard_norm():

	#set the x-range
	x = np.arange(-4.5,4.5,0.1)

	#generate 100 random number
	scalor = 2.5
	random_sample_1 = scalor*np.random.rand(1,50)
	random_sample_2 = -scalor*np.random.rand(1,50)
	random_sample_x = random_sample_1[0].tolist() + random_sample_2[0].tolist()
	#random_sample = eric_gaussian(np.asarray(random_sample_x),0,1)
	random_sample = np.random.normal(0,1,100)
	
	print random_sample

	#mark these random number on x-coordinate
	for marker in random_sample:
		pyplot.axvline(x=marker,color='b',ymax=0.02)

	#estimate with bandth 0.05
	estimate_value = 0.0
	for marker in random_sample:
		estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,0.05)
	estimate_value = estimate_value/len(random_sample)
	#plot the estimate distribution
	pyplot.plot(x,estimate_value,'r--')

	#estimate with bandth 2
	estimate_value = 0.0
	for marker in random_sample:
		estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,2)
	estimate_value = estimate_value/len(random_sample)
	#plot the estimate distribution
	pyplot.plot(x,estimate_value,'g--')

	#estimate with bandth 0.337
	estimate_value = 0.0
	for marker in random_sample:
		estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,0.337)
	estimate_value = estimate_value/len(random_sample)
	#plot the estimate distribution
	pyplot.plot(x,estimate_value,'black', ls='--')

	#estimate with  Silverman's rule of thumb
	estimate_value = 0.0
	for marker in random_sample:
		estimate_value = estimate_value + gaussian_kde_with_bandth(x,marker,1,choose_bandwith(random_sample))
	estimate_value = estimate_value/len(random_sample)
	#estimate_value = estimate_value/choose_bandwith(random_sample)
	#plot the estimate distribution
	pyplot.plot(x,estimate_value,'yellow',ls='-')


	#the scipy lib result
	lib_result = kde.gaussian_kde(random_sample)
	xgrid = np.linspace(random_sample.min(),random_sample.max(),100)
	pyplot.plot(xgrid,lib_result(xgrid),'red',ls=':',lw=2)

	#plot the standard gaussian (0,1)
	pyplot.plot(x,gaussian_kde_with_bandth(x,0,1,1),'grey')
	pyplot.plot(x,mlab.normpdf(x,0,1),'g*')

	pyplot.ylim(-0.02,0.5)
	pyplot.show()

if __name__=="__main__":
	#plot_gaussian()
	#choose_bandwith()
	estimate_standard_norm()
