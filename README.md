# Parallel-Programming
Parallel promramming with C language using MPI library
Program finds Armstrong numbers randomly distributed processors
between 1 and N where you need specfy N as argument in command

First you need install MPI library to your computer
You have specify hostfile to make available to slots

you can run code as follow:

mpicc mpi_project.c -o mpi_project
mpirun -np 11 --hostfile hostfile mpi_project 10000
