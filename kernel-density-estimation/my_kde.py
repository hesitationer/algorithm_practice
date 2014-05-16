from scipy.stats import kde
import matplotlib.pyplot as pyplot
import numpy as np
import math

# Show a simple example of how to do KDE (kernel density estimation)
# Reference: http://en.wikipedia.org/wiki/Kernel_density_estimation#Definition

class gaussian:
	sigma = 2.25
	mu    = [0,0,0,0,0,0]

# G(x) = a*e^(b/c)
def eric_gaussian(x,mu):
	a = 1./(math.sqrt(2*math.pi)*gaussian.sigma)
	b = -(x-mu)*(x-mu)
	c = 2*gaussian.sigma*gaussian.sigma
	return a*np.exp(b/c)

def plot_gaussian():
	x = np.arange(-10,10,0.1)
	sample = [-2.1, -1.3, -0.4, 1.9, 5.1, 6.2]

	#pyplot.hist(x, bins=6, normed=True)
	pyplot.plot(x,eric_gaussian(x,sample[0]),'r-')
	pyplot.plot(x,eric_gaussian(x,sample[1]),'r-')
	pyplot.plot(x,eric_gaussian(x,sample[2]),'r-')
	pyplot.plot(x,eric_gaussian(x,sample[3]),'r-')
	pyplot.plot(x,eric_gaussian(x,sample[4]),'r-')
	pyplot.plot(x,eric_gaussian(x,sample[5]),'r-')

	pyplot.plot(x,eric_gaussian(x,sample[0]) + 
			eric_gaussian(x,sample[1]) + 
			eric_gaussian(x,sample[2]) + 
			eric_gaussian(x,sample[3]) + 
			eric_gaussian(x,sample[4]) + 
			eric_gaussian(x,sample[5]),
			'b-'
			)
	pyplot.ylim(0,0.8)
	pyplot.show()

if __name__=="__main__":
	plot_gaussian()
