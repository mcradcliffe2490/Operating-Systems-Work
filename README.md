# Operating-Systems-Work

## Description

This is a repository for keeping all of the work that I have completed so far for my Operating Systems class. 
Below I will briefly describe the purpose/Objective of each program

## 1.cpp

Simply introduces forking by forking a process and identifying the parent and child processes

## 2.cpp 
Builds on 1.cpp by forking a process and having the child execute a command through using the execvp() method


## 3.cpp
Again, builds off of 2.cpp by having the child process execute a command given by the user
and displaying the output of the command in the parent process using a pipe

## 4.cpp
Focus shifts to threads. This program simply creates two seperate threads and counts from 1 to 10 using them

## 6.cpp
Recreates the solution to the producer consumer problem. This program takes in a number of producers and consumers
and creates them to do work on a buffer without producers overwritting full slots of data

## 7.cpp
Almost identical to 6.cpp, but the buffer is bounded

## 8.cpp
Recreates the solution to the Readers-Writers problem. Readers and Writers are created randomly and do work on the same buffer.
No thread can enter when a writer thread is doing work, but multiple readers threads can do work at the same time.
Turnstile semaphore is used to prevent writer starvation.

## Ash
The ash project is an ongoing project to implement a basic shell and learn how they fundamentally work. 
Currently, all built in is functional and soon regular commands will be too.
