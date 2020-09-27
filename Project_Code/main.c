
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "linkedList.h"
#include "spmat.h"
#include "Algorithm.h"
#include <time.h>


int main(int argc, char* argv[]) {
	/* variables declaration */
	FILE* input_file;
	FILE* output_file;
	spmat* B_mat;
	head_list* groups_divition;
	clock_t end, start;
	start = clock();

	check_good_input(argc);

	input_file = open_file(argv[1], "rb");

	output_file = open_file(argv[2], "wb");

	B_mat = input_to_matrix(input_file, output_file);

	if (B_mat->M == 0) {
		printf("M=0, can't divide to zero\n");
		exit(1);
	}

	groups_divition = divide_to_modularity_groups(B_mat, B_mat->n);

	result_to_output_File(output_file, argv[2], groups_divition);

	B_mat->free(B_mat);
	end = clock();
	cal_and_print_time("total PROGRAM TIME", start, end);

	return 0;

}

