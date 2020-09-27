#include "linkedList.h"
#include "utilities.h"
#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void cal_and_print_time(char* part_name, clock_t start, clock_t end) {
	printf("%s took %f seconds\n", part_name, (((double)end - (double)start) / CLOCKS_PER_SEC));
}
/*--------------------------alloc_func-----------------------------------*/

void* malloc_struct(size_t obj_size) {
	void* n;
	n = malloc(obj_size);
	if (n == NULL) {
		printf("malloc failed\n");
		exit(1);
	}
	return n;
}

void* calloc_array(int arr_size, size_t obj_size) {
	void* n;
	n = calloc(arr_size, obj_size);
	if (n == NULL) {
		printf("calloc failed\n");
		exit(1);
	}
	return n;
}

/*--------------------------END alloc_func-----------------------------------*/


/*--------------------------main_utilities-----------------------------------*/

void check_good_input(int argc) {
	if (argc != 3) {
		printf("wrong input, the program has %d args insted 2 args\n", argc - 1);
		exit(1);
	}
}

FILE* open_file(char* path, const char* mood) {
	FILE* file;

	file = fopen(path, mood);
	if (file == NULL) {
		printf("Open file failed\n");
		exit(1);
	}
	return file;
}

void read_from_file(void* read_var, size_t element_size, size_t element_count, FILE* inputFile) {
	int n;
	n = fread(read_var, element_size, element_count, inputFile);
	if (n != element_count) {
		printf("file read has failed\n");
		exit(1);
	}
}

void write_to_file(const void* write_info, size_t element_size, size_t element_count, FILE* outputFile) {
	int n;
	n = fwrite(write_info, element_size, element_count, outputFile);
	if (n != element_count) {
		printf("file write has failed\n");
		exit(1);
	}
}

/* Analysis of the input data:
	1. take the Input file and change it to sparse matrix - Representation of matrix A
	2. save to int array the degrees of each vertex
	3. save sum of degrees - M

	RETURN: sparse matrix - A
 */
spmat* input_to_matrix(FILE* inputFile, FILE* outputFile) {
	int i, M, nodes_num, node_deg, one;
	int* rowArr;
	int* k;
	spmat* mat;

	read_from_file(&nodes_num, sizeof(int), 1, inputFile);

	mat = spmat_allocate_list(nodes_num);
	k = (int*)calloc_array(nodes_num, sizeof(int));
	rowArr = (int*)calloc_array(nodes_num, sizeof(int));
	M = 0;

	/* loop over rows to create spmat*/
	for (i = 0; i < nodes_num; ++i) {
		read_from_file(&node_deg, sizeof(int), 1, inputFile);
		*(k + i) = node_deg;
		M += node_deg;

		read_from_file(rowArr, sizeof(int), node_deg, inputFile);

		mat->add_row(mat, rowArr, i, node_deg);
	}

	if (M == 0) {
		one = 1;
		write_to_file(&nodes_num, sizeof(int), 1, outputFile);
		for (i = 0; i < nodes_num; ++i) {
			/* write the size of the group */
			write_to_file(&one, sizeof(int), 1, outputFile);

			/* write the nodes num in the group*/
			write_to_file(&i, sizeof(int), 1, outputFile);
		}
	}
	mat->k = k;
	mat->M = M;
	free(rowArr);
	return mat;
}

void result_to_output_File(FILE* output_file, char* path, head_list* devision) {
	int group_size;
	linked_list* curr_node;

	write_to_file(&(devision->list_size), sizeof(int), 1, output_file);

	curr_node = devision->next;
	while (curr_node != NULL)
	{
		group_size = curr_node->group_size;
		/* write the size of the group */
		write_to_file(&group_size, sizeof(int), 1, output_file);

		/* write the nodes num in the group*/
		write_to_file((curr_node->vertexes_group), sizeof(int), group_size, output_file);

		curr_node = curr_node->next;
	}

	free_linked_list(devision);
}

/*--------------------------main_utilities-----------------------------------*/
