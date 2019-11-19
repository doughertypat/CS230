# -*- Makefile -*-
all: CS230_A3Q2

CS230_A3Q2: CS230_A3Q2A_DoughertyP.o CS230_A3Q2_DoughertyP.o CS230_A3Q2B_DoughertyP.o
	gcc -g -o CS230_A3Q2 CS230_A3Q2A_DoughertyP.o CS230_A3Q2_DoughertyP.o CS230_A3Q2B_DoughertyP.o

CS230_A3Q2_DoughertyP.o: CS230_A3Q2_DoughertyP.c
	gcc -c CS230_A3Q2_DoughertyP.c

CS230_A3Q2A_DoughertyP.o: CS230_A3Q2A_DoughertyP.c CS230_A3Q2A.h
	gcc -c CS230_A3Q2A_DoughertyP.c

CS230_A3Q2B_DoughertyP.o: CS230_A3Q2B_DoughertyP.c CS230_A3Q2B.h
	gcc -c CS230_A3Q2B_DoughertyP.c

