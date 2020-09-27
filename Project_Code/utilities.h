/*
 * utilities.h
 *
 *  Created on: Sep 1, 2020
 *      Author: ofir1
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_
#define _CRT_SECURE_NO_WARNINGS
#include "linkedList.h"
#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*for DEBUG- print the time it took*/
void cal_and_print_time(char* part_name, clock_t start, clock_t end);
/*---------------choose double or int-----------------------*/

 /* (1)----------------- mallocs + error check ---------------*/
/* malloc object and check for success*/
void* malloc_struct(size_t obj_size);

/* calloc object and check for success*/
void* calloc_array(int arr_size, size_t obj_size);

/* --------------- END of mallocs + error check ---------------*/

/* (1)----------------- main_utilities ---------------*/
/* check thet the program was given enougth args*/
void check_good_input(int argc);

/* open file and check for success*/
FILE* open_file(char* path, const char* mood);

/* create the sparse matrix from input file*/
spmat* input_to_matrix(FILE* inputFile, FILE* outputFile);

/* create rhe result file according to the program results*/
void result_to_output_File(FILE* output_file, char* path, head_list* devision);

/* (1)----------------- END main_utilities ---------------*/

#endif /* UTILITIES_H_ */
