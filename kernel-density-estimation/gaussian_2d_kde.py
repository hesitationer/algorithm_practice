#  -*- coding: utf-8 -*-
# Try to show process of the Kernel Density Estimation in 2D-sample.
# Points to NOTE:
#    1. The co-variance matrix M
#    2. The meaning of sum the 
#         从１维到２维，形式是相乘的关系(e^x*e^y)；指数是相加的关系(e^(x+y))
#
#   参考： scipy.stats.kde的源码

from numpy import atleast_2d, reshape, zeros, newaxis, dot, exp, pi, sqrt, \
     ravel, power, atleast_1d, squeeze, sum, transpose
from scipy.stats import kde
from scipy import linalg, special
import matplotlib.pyplot as pyplot
import matplotlib.mlab as mlab
import numpy as np
import math


# calculate the bandwidth
def scotts_factor(data_number, data_dimension):
    return power(data_number,-1./(data_dimension + 4))


# calcuate the covariance matrix
def calculate_variance_2d(dataset):
    M,N = dataset.shape

    row_1 = dataset[0]
    row_2 = dataset[1]
    mean_1 = sum(row_1)/N
    mean_2 = sum(row_2)/N

    mean_matrix = dataset - [[mean_1],[mean_2]]

    covariance_matrix = zeros((2,2), dtype=np.float)

    covariance_matrix[0,0] = sum((mean_matrix[0])*(mean_matrix[0]))/(N-1)
    covariance_matrix[0,1] = sum((mean_matrix[0])*(mean_matrix[1]))/(N-1)
    covariance_matrix[1,0] = sum((mean_matrix[1])*(mean_matrix[0]))/(N-1)
    covariance_matrix[1,1] = sum((mean_matrix[1])*(mean_matrix[1]))/(N-1)

    return covariance_matrix

def gaussian_2d_kde(values,positions):
    _,N = values.shape

    cov_mat = calculate_variance_2d(values)
    inv_cov  = linalg.inv(cov_mat)

    bandwidth = scotts_factor(N,2)
    tdiff_factor = inv_cov/bandwidth**2

    _, len_v = values.shape
    _, len_p = positions.shape
    len_ret = max(len_v,len_p)
    result = zeros((len_ret,),dtype=np.float)

    for i in range(N):
        diff = values[:,i,newaxis] - positions
        tdiff = dot(tdiff_factor,diff)
        #NOTE: the meaning of sum the diff*tdiff
        energy = sum(diff*tdiff,axis=0) / 2.0

        result = result + exp(-energy)

    #scale the result
    scale_factor = sqrt(2*pi*linalg.det(cov_mat))*bandwidth*N
    result = result / scale_factor

    return result

def prepare_2d_samples_and_positions():
    N = 1000
    t1 = np.random.normal(size=N)
    t2 = np.random.normal(scale=0.5,size=N)

    m1 = t1 + t2
    m2 = t1 - t2

    samples = np.vstack([m1,m2])

    xmin = m1.min()
    xmax = m1.max()
    ymin = m2.min()
    ymax = m2.max()

    X,Y = np.mgrid[xmin:xmax:100j,ymin:ymax:100j]
    positions = np.vstack([X.ravel(),Y.ravel()])

    return samples,positions,xmin,xmax,ymin,ymax


def test_variance_2d():
    #prepare dataset
    values,positions = prepare_2d_samples_and_positions() 
      
    calculate_variance_2d(values)


def demo():
    values,positions,xmin,xmax,ymin,ymax = prepare_2d_samples_and_positions()
    result = gaussian_2d_kde(values,positions)

    Z = np.reshape(result.T,(100,100))

    #plot 
    fig = pyplot.figure()
    ax = fig.add_subplot(121)
    ax.imshow(np.rot90(Z),cmap=pyplot.cm.gist_earth_r,
            extent=[xmin,xmax,ymin,ymax])
    ax.plot(values[0],values[1],'k.',markersize=2,label='right')

    ax2 = fig.add_subplot(122)
    ax2.imshow(Z,cmap=pyplot.cm.gist_earth_r,
            extent=[xmin,xmax,ymin,ymax])
    ax2.plot(values[0],values[1],'k.',markersize=2,label='no rotation')

    ax.set_xlim([xmin,xmax])
    ax.set_ylim([ymin,ymax])
    ax2.set_xlim([xmin,xmax])
    ax2.set_ylim([ymin,ymax])

    #legend
    legend = ax.legend(bbox_to_anchor=(0.2,1.02),loc='lower left', shadow=True)
    legend = ax2.legend(bbox_to_anchor=(0.2,1.02),loc='lower left', shadow=True)

    pyplot.show()


if __name__=="__main__":
    #test_variance_2d()
    demo()

