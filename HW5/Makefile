# -*- Makefile -*-
all: CS230_HW5Q2

CS230_HW5Q2: CS230_HW5Q2A_DoughertyP.o CS230_HW5Q2_DoughertyP.o
	gcc -g -o CS230_HW5Q2 CS230_HW5Q2A_DoughertyP.o CS230_HW5Q2_DoughertyP.o -pthread

CS230_HW5Q2_DoughertyP.o: CS230_HW5Q2_DoughertyP.c
	gcc -c CS230_HW5Q2_DoughertyP.c

CS230_HW5Q2A_DoughertyP.o: CS230_HW5Q2A_DoughertyP.c CS230_HW5Q2A_DoughertyP.h
	gcc -c CS230_HW5Q2A_DoughertyP.c

