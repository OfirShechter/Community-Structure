#include "spmat.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*--------------------------List_spmat_define-----------------------------------*/

typedef struct _list_row_ {
	struct _list_row_* next;
	int col;
} rowListNode;

typedef struct empty_list_row {
	rowListNode* head;
	rowListNode* tail;
}rowLinkedList;

rowLinkedList* create_empty_list(int n);
rowLinkedList create_empty_list_row();
void insertRowToArray(rowLinkedList* listArray, const int* row, int size);
rowListNode* create_node_row(rowListNode* next, int col);
void insert_end_of_list_row(rowLinkedList* list, int col);
void add_row_list(struct _spmat* A, const int* row, int i, int size);
void mult_list(const struct _spmat* A, const int* v, double* result);
void free_subList(rowListNode* rowNode);
void free_mainList(rowLinkedList* LinkedListArr, int n);
void free_list(struct _spmat* A);

/*This function allocated memory for "list" struct*/
spmat* spmat_allocate_list(int n) {
	spmat* listSPmat;
	listSPmat = (spmat*)malloc_struct(sizeof(spmat));
	listSPmat->private = create_empty_list(n);
	listSPmat->n = n;
	listSPmat->add_row = &add_row_list;
	listSPmat->free = &free_list;

	return listSPmat;
}

/*This function creates an empty linked-list
where every node is a linked-list presenting
the row of a matrice*/
rowLinkedList* create_empty_list(int n) {
	rowLinkedList* mainListArray = (rowLinkedList*)calloc_array(n, sizeof(rowLinkedList));
	return mainListArray;
}

/*This function creates an empty linked-list,
This list represents a single row of the matrice*/
rowLinkedList create_empty_list_row() {
	rowLinkedList list;

	list.tail = NULL;
	list.head = (rowListNode*)malloc_struct(sizeof(rowListNode)); /*dummy head*/
	list.head->next = list.tail;

	return list;
}

/*This function inserts a row from the matrice to the linked list implementation*/
void insertRowToArray(rowLinkedList* listArray, const int* row, int size) {
	int i;
	*listArray = create_empty_list_row();
	if (size != 0) {
		listArray->head->col = *(row);
	}
	else {
		listArray->head = NULL;
	}
	for (i = 1; i < size; i++)
	{
		insert_end_of_list_row(listArray, *(row+i));

	}
}

void insert_end_of_list_row(rowLinkedList* list, int col) {

	rowListNode* new_tail;
	new_tail = create_node_row(NULL, col);
	if (list->tail == NULL) {
		list->head->next = new_tail;
	}
	else {
		list->tail->next = new_tail;
	}
	list->tail = new_tail;
}

/*This function creates a new node, inserting "item" as its item,
"col", as its col, and a given "next" as next.*/
rowListNode* create_node_row(rowListNode* next, int col)
{
	rowListNode* node;
	node = (rowListNode*)malloc_struct(sizeof(rowListNode));
	node->next = next;
	node->col = col;
	return node;
}


/*This function takes a row from the matrice and adds it to the list implementation*/
void add_row_list(struct _spmat* A, const int* row, int i, int size) {
	rowLinkedList* mainListArray;
	mainListArray = (rowLinkedList*)A->private; /*starting from NULL and stays so if row is full of zeroes*/
	insertRowToArray(mainListArray + i, row, size);
}

/*Free functions*/
void free_subList(rowListNode* rowNode)
{
	if (rowNode != NULL) {
		free_subList(rowNode->next);
		free(rowNode);
	}
}

void free_mainList(rowLinkedList* LinkedListArr, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		free_subList((LinkedListArr + i)->head);
	}
	free(LinkedListArr);
}

void free_list(struct _spmat* A)
{
	rowLinkedList* LinkedListArr = (rowLinkedList*)A->private;
	LinkedListArr = (rowLinkedList*)A->private;
	free_mainList(LinkedListArr, A->n);
	free(A->k);
	free(A);
}
/*--------------------------END List_spmat_define-----------------------------------*/

/* return value of B node by given indexes --NOT B_HAT---*/
double get_specific_node_val(struct _spmat* C, int i, int j) {
	rowLinkedList* row_list;
	rowListNode* curr_node;
	double value;
	int* k;
	k = C->k;
	row_list = ((rowLinkedList*)(C->private));
	curr_node = (row_list + i)->head;

	value = - ( ((double)(*(k + i)) * (*(k + j))) / (C->M) );

	while (curr_node != NULL) {
		if (curr_node->col >= j) {
			if (curr_node->col == j) {
				value += 1;
			}
			break;
		}
		curr_node = curr_node->next;
	}

	return value;
}
/*--------------------------B_cals-----------------------------------*/
double inside_cal_norm_k_M_not_0(rowListNode* curr_node, int max_vertex, double k_M, int* k, int* curr_g_node, int size) {
	int curr_col, curr_vertex, ind;
	double row_sum;
	curr_vertex = *(curr_g_node);
	row_sum = 0;
	ind = 0;
	while (curr_node != NULL)
	{
		curr_col = curr_node->col;

		if (curr_col > max_vertex) {
			break;
		}

		while (curr_col > curr_vertex) {
			row_sum += k_M * (*(k + curr_vertex));
			curr_g_node++;
			curr_vertex = *(curr_g_node);
			ind++;
		}

		if (curr_col == curr_vertex) {
			row_sum += fabs(1 - (k_M * (*(k + curr_col)))); /* equal add_val = (curr_node->item) - (k_M * (*(k + curr_col)));*/
			curr_g_node++;
			curr_vertex = *(curr_g_node);
			ind++;
		}

		curr_node = curr_node->next;
	}
	while (ind < size)
	{
		row_sum += k_M * (*(k + curr_vertex));
		curr_g_node++;
		curr_vertex = *(curr_g_node);
		ind++;
	}

	return row_sum;
}

double inside_cal_norm_k_M_is_0(rowListNode* curr_node, int max_vertex, int* curr_g_node) {
	int curr_col, curr_vertex;
	double row_sum;
	curr_vertex = *(curr_g_node);
	row_sum = 0;

	while (curr_node != NULL)
	{
		curr_col = curr_node->col;

		if (curr_col > max_vertex) {
			break;
		}

		while (curr_col > curr_vertex) {
			curr_g_node++;
			curr_vertex = *(curr_g_node);
		}

		if (curr_col == curr_vertex) {
			row_sum += 1; /* equal add_val = (curr_node->item) - (k_M * (*(k + curr_col)));*/
			curr_g_node++;
			curr_vertex = *(curr_g_node);
		}

		curr_node = curr_node->next;
	}

	return row_sum;
}

void cal_norm1_new(struct _spmat* C, int size, int* g_nodes) {
	int* k;
	int i, M, max_vertex;
	double k_M, max_row_sum, row_sum;
	rowLinkedList* row_list;
	rowListNode* curr_node;

	row_list = ((rowLinkedList*)(C->private));
	k = C->k;
	M = C->M;

	max_vertex = *(g_nodes + size - 1);
	max_row_sum = 0;

	/*call all values*/
	for (i = 0; i < size; i++)
	{
		k_M = (double)*(k + i) / M;
		row_sum = 0;
		curr_node = (row_list + *(g_nodes + i))->head;

		if (k_M != 0) {
			row_sum = inside_cal_norm_k_M_not_0(curr_node, max_vertex, k_M, k, g_nodes, size);
		}

		else {
			row_sum = inside_cal_norm_k_M_is_0(curr_node, max_vertex, g_nodes);
		}
		
		if (row_sum > max_row_sum) {
			max_row_sum = row_sum;
		}
	}
	
	C->norm1 = max_row_sum;
}

/* mult B[g]_hat matrix with s (B[g]_h * s)
* RESULT = vector
*/
void B_g_hat_mult_s(double* s, spmat* B_g_hat, linked_list* g, double* result, char c_tag) {
	int size, i, curr_node;
	int* nodes;

	size = g->group_size;
	nodes = g->vertexes_group;

	for (i = 0; i < size; i++)
	{
		curr_node = *(nodes + i);
		*(result + curr_node) = mat_row_mult_vec_double(B_g_hat, curr_node, s, g, *(s + curr_node), c_tag);
	}
}

void B_g_hat_mult_s_char(char* s, spmat* B_g_hat, linked_list* g, double* result) {
	int size, i, curr_node;
	int* nodes;

	size = g->group_size;
	nodes = g->vertexes_group;

	for (i = 0; i < size; i++)
	{
		curr_node = *(nodes + i);
		*(result + curr_node) = mat_row_mult_vec_char(B_g_hat, curr_node, s, g, 0);
	}
}

/* -----------------------------------SCORE FUNCTIONS-------------------------------*/

double add_norm_val(double norm_val, int row_num, void* vec) {
	double result;
	double vec_val;

	result = 0;
	vec_val = *((double*)vec + row_num);

	result = norm_val * vec_val;

	return result;
}

double mult_without_k_char(rowListNode* curr_node, int* curr_g_node, int size, char* vec, double sub_val);
double mult_without_k_double(rowListNode* curr_node, int* curr_g_node, int size, double* vec, double sub_val);

double mat_row_mult_vec_double(spmat* C, int row_num, double* vec, linked_list* g, double sub_val, char c_tag) {
	rowLinkedList* row_list;
	rowListNode* curr_node;
	int size, ind, curr_vertex;
	int* g_nodes, * k, *curr_n_g;
	double result, k_M, norm_val;

	row_list = (rowLinkedList*)C->private;
	curr_node = (row_list + row_num)->head;
	size = g->group_size;
	g_nodes = g->vertexes_group;
	curr_n_g = g_nodes;
	k = C->k;
	k_M = (double)*(k + row_num) / C->M;
	norm_val = C->norm1;
	ind = 0; result = 0;

	if (k_M != 0) {
		while (ind < size) {
			curr_vertex = *(curr_n_g);
			result -= (*(vec + curr_vertex) - sub_val) * (*(k + curr_vertex));
			curr_n_g++;
			ind++;
		}

		result = result * k_M;
	}

	result += mult_without_k_double(curr_node, g_nodes, size, vec, sub_val);

	/* choose needed add_norm_func*/
	if (c_tag) {
		result += add_norm_val(norm_val, row_num, vec);
	}

	return result;
}

double mat_row_mult_vec_char(spmat* C, int row_num, char* vec, linked_list* g, double sub_val) {
	rowLinkedList* row_list;
	rowListNode* curr_node;
	int size, ind, curr_vertex;
	int* g_nodes, * k, * curr_n_g;
	double result, k_M;

	row_list = (rowLinkedList*)C->private;
	curr_node = (row_list + row_num)->head;
	size = g->group_size;
	g_nodes = g->vertexes_group;
	curr_n_g = g_nodes;
	k = C->k;
	k_M = (double)*(k + row_num) / C->M;
	ind = 0; result = 0;

	if (k_M != 0) {
		while (ind < size) {
			curr_vertex = *(curr_n_g);
			result -= (*(vec + curr_vertex) - sub_val) * (*(k + curr_vertex));
			curr_n_g++;
			ind++;
		}

		result = result * k_M;
	}

	result += mult_without_k_char(curr_node, g_nodes, size, vec, sub_val);

	return result;
}

double mult_without_k_char(rowListNode* curr_node, int* curr_g_node, int size, char* vec, double sub_val) {
	int curr_col, curr_vertex, max_vertex;
	double result;

	result = 0;
	max_vertex = *(curr_g_node + size - 1);
	curr_vertex = *(curr_g_node);
	while (curr_node != NULL)
	{
		curr_col = curr_node->col;

		if (curr_col > max_vertex) {
			break;
		}

		while (curr_col > curr_vertex) {
			curr_g_node++;
			curr_vertex = *(curr_g_node);
		}

		if (curr_col == curr_vertex) {
			result += *(vec + curr_col) - sub_val;
			curr_g_node++;
			curr_vertex = *(curr_g_node);
		}

		curr_node = curr_node->next;

	}

	return result;
}

double mult_without_k_double(rowListNode* curr_node, int* curr_g_node, int size, double* vec, double sub_val) {
	int curr_col, curr_vertex, max_vertex;
	double result;

	result = 0;
	max_vertex = *(curr_g_node + size - 1);
	curr_vertex = *(curr_g_node);
	while (curr_node != NULL)
	{
		curr_col = curr_node->col;
		
		if (curr_col > max_vertex) {
			break;
		}

		while (curr_col > curr_vertex) {
			curr_g_node++;
			curr_vertex = *(curr_g_node);
		}

		if (curr_col == curr_vertex) {
			result += *(vec + curr_col) - sub_val;
			curr_g_node++;
			curr_vertex = *(curr_g_node);
		}

		curr_node = curr_node->next;
		
	}

	return result;
}
/* -----------------------------------SCORE FUNCTIONS-------------------------------*/
double vec_mult_vec(double* vec1, double* vec2, linked_list* g) {
	int size, curr_vertex, i;
	int* g_nodes;
	double result;

	size = g->group_size;
	g_nodes = g->vertexes_group;
	curr_vertex = *(g_nodes);
	result = 0;


	for (i = 0; i < size; i++)
	{
		curr_vertex = *(g_nodes);
		result += (*(vec1 + curr_vertex)) * (*(vec2 + curr_vertex));
		g_nodes += 1;
	}

	return result;
}

/*--------------------------END B_cals-----------------------------------*/




