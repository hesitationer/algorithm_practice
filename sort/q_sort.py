import numpy as np

def q_sort(obj,l,r):
	if l < r:
		pivot = wiki_partion(obj,l,r)
		#pivot = my_partion(obj,l,r)
		q_sort(obj,l,pivot - 1)
		q_sort(obj,pivot+1,r)

def my_partion(obj,l,r):
	N = r - l + 1
	pivot = N/2

	if N <= 2:
		return pivot

	# swap pivot to r
	temp = obj[pivot];obj[pivot] = obj[r]; obj[r] = temp

	high = r - 1
	while True:
		# find the place to swap
		while obj[l] < obj[pivot]:
			print 'high', high
			print 'l',l
			print 'pivot',pivot
			if (high - l) == 1:
				break
			l = l + 1
		while obj[high] >= obj[pivot]:
			print 'high', high
			print 'l',    l
			print 'pivot',pivot
			if (high - l) == 1:
				break
			high = high - 1

		# l / r meets ?
		if high - l == 1:
			temp = obj[r];obj[r] = obj[high]; obj[high] = temp
			pivot = high
			break
		else:
			# swap l <--> r
			temp = obj[l]
			obj[l] = obj[high]
			obj[high] = temp

	print '---------------\n'

	return pivot

def wiki_partion(obj,l,r):
	N = r - l + 1
	#NOTE: Do NOT forget plus the l
	pivot = N / 2 + l 
	temp = obj[r];obj[r]=obj[pivot];obj[pivot]=temp
	si = l

	for i in range(l,r):
		if obj[i] <= obj[r]:
			temp = obj[i];obj[i]=obj[si];obj[si]=temp
			si = si + 1

	temp = obj[si];obj[si] = obj[r];obj[r] = temp

	return si

def demo():
	X = np.random.normal(0,1,10).round(1)
	#X = [3,9,2,7,4,6,5,4,9,3]
	Z = X
	print 'X',X

	q_sort(X,0,len(X) - 1)
	print 'Y',X

	Z.sort()
	print 'Z',Z

def change_list(X):
	X[1] = 1.111

def return_int():
	return 5

def test_range():
	for i in range(0,10):
		print i,

if __name__=="__main__":
	demo()
	#test_range()
