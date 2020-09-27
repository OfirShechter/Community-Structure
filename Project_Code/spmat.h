
#include "linkedList.h"
#ifndef _SPMAT_H
#define _SPMAT_H

/*---------------choose double or int-----------------------*/
typedef struct _spmat {
	/* Matrix size (n*n) */
	int		n;

	/* Adds row i the matrix. Called before any other call,
	 * exactly n times in order (i = 0 to n-1) */
	void	(*add_row)(struct _spmat* A, const int* row, int i, int size);

	/* Frees all resources used by A */
	void	(*free)(struct _spmat* A);

	/* Private field for inner implementation.
	 * Should not be read or modified externally */
	void* private;

	int* k;
	int M;
	double norm1;
} spmat;

spmat* spmat_allocate_list(int n);


/* ----------------------------------- B_cals ------------------------------*/

/* mult B[g]_hat matrix with s (B[g]_h * s)- include matrix shifting
* RESULT = vector
*/
void B_g_hat_mult_s(double* s, spmat* B_g_hat, linked_list* g, double* result, char c_tag);

/* mult B[g]_hat matrix with s (B[g]_h * s)
* RESULT = vector
*/
void B_g_hat_mult_s_char(char* s, spmat* B_g_hat, linked_list* g, double* result);

/* calculate the norm of the matrix*/
void cal_norm1_new(struct _spmat* C, int size, int* g_nodes);

/* mult specific row with given vector according to the needed nodes*/
double mat_row_mult_vec_double(spmat* C, int row_num, double* vec, linked_list* g, double sub_val, char c_tag);

/* mult specific row with given vector according to the needed nodes*/
double mat_row_mult_vec_char(spmat* C, int row_num, char* vec, linked_list* g, double sub_val);

/* mult two vector in the relevant nodes (according to g group) */
double vec_mult_vec(double* vec1, double* vec2, linked_list* g);

/* get node in the matrix*/
double get_specific_node_val(struct _spmat* C, int i, int j);

/* ----------------------------------- B_cals ------------------------------*/

#endif
