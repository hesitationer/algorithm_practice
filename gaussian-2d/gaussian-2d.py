#!/usr/bin/python

# This file create the 2-D gaussian blur sample matrix
# reference: http://en.wikipedia.org/wiki/Gaussian_blur#Sample_Gaussian_matrix

import math

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

# G(x,y) = a*exp(-b/c)
PI = 3.14159265359
sigma = 0.84089642
def gaussian_2d(size):
	a = 1./(2*PI*pow(sigma,2))
	c = 2*pow(sigma,2)

	print 'a,c:', a,c,'\n\n'

	for y in range(-size,size + 1, 1):
		for x in range(-size, size + 1, 1):
			b = pow(x,2) + pow(y,2)
			G = a*math.exp(-b/c)
			format_output(x,y,G)
		print '\n'

# utility print, ensure:
# 1. colorful some value
# 2. align the number
def format_output(x,y,G):
	if(y == 0 and x == 0):
		print(bcolors.OKGREEN),
		print("%.8f" % round(G,8)),
		print(bcolors.ENDC),
	elif math.fabs(y) == 3 and math.fabs(x) == 1:
		print(bcolors.OKGREEN),
		print("%.8f" % round(G,8)),
		print(bcolors.ENDC),
	elif math.fabs(y) == 1 and math.fabs(x) == 3:
		print(bcolors.OKGREEN),
		print("%.8f" % round(G,8)),
		print(bcolors.ENDC),
	else:
		print(" %.8f " % round(G,8)),


if __name__=="__main__":
	gaussian_2d(3);

