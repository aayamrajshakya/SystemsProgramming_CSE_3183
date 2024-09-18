# Project 2

For the next few exercises, you will be working with various parts of a data set 
collected from a WIT Motion WT901BLECL multiaxis accelerometer. Accelerometers
are used in VR, robotics, logistics, etc.; so, reading and processing the data
is a typical skill that many programmers will experience. 

Part of that experience has been done for you, configuring the tty that connects 
to and reads from the USB port has already been done and a data set has been 
collected and stored in a binary file (note: this type of file doesn't seem to 
upload very well to onlineGDB, so I can't recommend writing the program using that tool).

The provided binary file contains the raw data as read from the accelerometer. The
accelerometer reports its data in 20-byte packets; these packets contain a header and
multiple data points that have been converted from doubles into pairs of integers as 
a form of data compression. The details of the data packet and the data compression
used for each of the store values is described in the provided data sheet. 

In this exercise you must write a program that reads accelerometer data from the provided file, 
decompresses the data points, and outputs the data points to both the console and a binary
file named "data.dat" by default. (Note: You shouldn't be able to read the contents of "data.dat" 
with notepad or another text editor ... it is a binary file.)

Make certain to perform all file access using the read, write, open, and close system 
commands for this program.

## How to run

```bash
clang main.c -o out 
./out raw.dat data.dat
```
