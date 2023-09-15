## Directory Files Calculator ##

# Student Name: YE Hongran
# Student ID: [REDACTED]

## Synopsis
Assignment-01 of USP

## Overview
The Directory File Calculator is a utility designed to scan a directory for "usp" file extensions, 
process those files in parallel using child processes, 
and then aggregate the results into a single output file.

## Syntax
Usage: 
1. Navigate to the directory containing the source files and the makefile.
2. Run the following command to compile the code:
make
3. After successful compilation, run the program using:
./calculator
4. To detect memory leaks using: 
valgrind --leak-check=full ./calculator

## Contents
- directory_retriever.c: Contains functions that help in retrieving directory content.
- file_reader.c: Contains functions to read content from files. It provides utility functions that process and manipulate the content of the files once read.
- main.c： Utility scans a directory for specific file extensions, processes these files in parallel using child processes, and aggregates the results into a single output file.

## Version information
15/09/2023  - final version of Assignment 01 maze game programs

