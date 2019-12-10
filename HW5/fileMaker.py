"""
CS230 - Assignment 5 Question 1
Patrick Dougherty
patrick.r.dougherty@und.edu
9 December 2019

This program is a helper file to create the files needed for assignment 5
question 1.  It makes files with integers in the 10000's. For example 10000,
10001, 10002...  It will make as many files as indicated in fileNum and will
make as many integers as indicated in itemNum.  The usefulness of the program
is kind of lost for itemNum over 9999 as the files will begin to have
overlapping numbers.
""" 

fileNum = 5;
itemNum = 10000;

for i in range(1,fileNum+1):
	fileName = "Prod%d.txt" % i
	with open(fileName, 'w') as f:
		f.write("%d" % (i*10000+1))
		for j in range(2,itemNum+1):
			f.write(", %d" % (i*10000+j))
