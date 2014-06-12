import numpy as np

def q_sort(obj,l,r):
	if l < r:
		#pivot = wiki_partion(obj,l,r)
		pivot = my_partion(obj,l,r)
		q_sort(obj,l,pivot - 1)
		q_sort(obj,pivot+1,r)

def my_partion(obj,l,r):
	# store the l
	temp = obj[l]

	i = l
	j = r

	while i != j:
		# find the place to swap

		# first r---->l
		while obj[j] >= temp and i < j:
			j = j - 1
		if i < j:
			obj[i] = obj[j]
			i = i + 1

		# then l----->r
		while obj[i] < temp and i < j:
			i = i + 1
		if i < j:
			obj[j] = obj[i]
			j = j - 1

	obj[i] = temp

	return i

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
