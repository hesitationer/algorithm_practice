#/usr/bin/python
import numpy as np
import matplotlib.pyplot as pyplot

X,Y = np.mgrid[2:6:5j, 2:6:5j]

pyplot.plot(X,Y,'k.',markersize=5)

for i in range(5):
	pyplot.text(1.9 + 0,1.7 + i,r"$ x_0 $",size='xx-large')

for i in range(5):
	pyplot.text(1.9 + 1,1.7 + i,r"$ x_1 $",size='xx-large')

for i in range(5):
	pyplot.text(1.9 + 2,1.7 + i,r"$ x_2 $",size='xx-large')

for i in range(5):
	pyplot.text(1.9 + 3,1.7 + i,r"$ x_3 $",size='xx-large')

for i in range(5):
	pyplot.text(1.9 + 4,1.7 + i,r"$ x_4 $",size='xx-large')

pyplot.xlim(1,7)
pyplot.ylim(1,7)


pyplot.show()
