# Multithreaded-Sort-Files

This program implement a parallel merge of sorted integers stored in N input files. The input files 
will contain sorted integers in ascending order. Each line of an input file will contain exactly one 
integer.  The  first  line  of  each  input  file  will  contain  total  number  of    integers  in  that  file.  

The number of input files can be 2, 4, or 8. In more than 2 files, program merge the files in several 
phases. A merge will be done always between two files (no more than that). The idea is shown in 
the figure below. There are eight input files, each of which stores sorted integers. Then these files 
will be merged into four intermediate buffers. The content of these four buffers will merged into 
two other intermediate buffers. The content of these two other buffers will be merged into a final output file. 
Note that the buffers B1 through B6 are shared buffers. While one merge operation is 
using a buffer for output, another merge operation is using that buffer for input.  

<img width="649" alt="Screen Shot 2021-11-28 at 06 03 27" src="https://user-images.githubusercontent.com/46750371/143727234-d2ec492a-4efd-41a3-b6d6-742ca5ed02a3.png">

Program  use  concurrent  threads  to  implement  the  merge  operations.  For  example  in  the  figure 
above, there would be seven threads that are created by a main thread. These seven threads should 
be created at the beginning based on the total number of files and each should perform one merge 
operation as shown in the figure concurrently. A thread, while doing a merge operation, will take 
the input from either two files or two shared buffers. A thread will write the output to either a final 
output  file  or  to  another  shared  buffer.  Program  creates all of threads at the 
beginning.  

In case of 2 input files, only one thread is adequte. In case of 4 files, program create 3 threads in 
total. 

Example Usage
$ gcc main.c –o multi.out -lpthread 
$ ./multi.out  –n 4 file1 file2 file3 file4 -o outputfile 

The –n option specifies the  number of input 
files. Then comes the name of the input files. The name of the output file is specified using the –o 
option.  


Also program compares multithread with single thread
