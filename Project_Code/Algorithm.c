//#define EPSILON 0.00001
#include "spmat.h"
#include "linkedList.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define NO_STDIO_REDIRECT
#define EPSILON 0.00001


/* ---------------------- eigenpair------------------------------*/
/*This function creates a random vector if needed*/
void insertRandomVector(double* vector, int size, int* g_nodes) {
	int i;

	for (i = 0; i < size; i++) {
		*(vector + *(g_nodes)) = (double)rand();
		g_nodes += 1;
	}
}

double mechaneCalc(double* mone, double mechane, int size, int* g_nodes)
{
	int k;
	double mult_val;

	for (k = 0; k < size; k++) {
		mult_val = *(mone + *(g_nodes));
		mechane += mult_val * mult_val;
		g_nodes += 1;
	}
	return mechane;
}

double inside_of_loop_of_PI(spmat* C, linked_list* g, int size, int* g_nodes, double* input_v,  double* output_v) {
	double mechane, h, delta;
	int i, *curr_node, curr_vertex;
	curr_node = g_nodes;

	B_g_hat_mult_s(input_v, C, g, output_v, 1); /*mult spmat with vector*/
	mechane = 0.0;
	mechane = mechaneCalc(output_v, mechane, size, g_nodes);
	mechane = sqrt(mechane);

	curr_vertex = *(curr_node);
	*(output_v + curr_vertex) /= mechane;
	curr_node++;
	h = fabs(*(output_v + curr_vertex) - *(input_v + curr_vertex));
	for (i = 1; i < size; i++) {
		curr_vertex = *(curr_node);
		*(output_v + curr_vertex) /= mechane;
		curr_node++;
		delta = fabs(*(output_v + curr_vertex) - *(input_v + curr_vertex));
		if (h < delta) {
			h = delta;
		}
			
	}

	return h;
}

/*find the leading eigenvector using power iteration (dominant vector of C')
	RETURN: leading eigenvector
*/
double* find_leading_eigenvector(spmat* C, linked_list* g) {
	double h;
	double* vector, * mone;
	int* g_nodes, size;

	h = 1;
	size = g->group_size;
	g_nodes = g->vertexes_group;
	mone = (double*)calloc_array(C->n, sizeof(double));
	vector = (double*)calloc_array(C->n, sizeof(double));
	insertRandomVector(vector, size, g_nodes);
	cal_norm1_new(C, size, g_nodes);

	while (1)
	{
		h = inside_of_loop_of_PI(C, g, size, g_nodes, vector, mone);
		
		if (h < EPSILON) {
			free(vector);
			return mone;
		}

		h = inside_of_loop_of_PI(C, g, size, g_nodes, mone, vector);
		
		if (h < EPSILON) {
			free(mone);
			return vector;
		}

	}
}

/*find the leading eigenvalue
	RETURN: leading eigenvalue
*/
double find_leading_eigenvalue(double* eigenvector, spmat* C, linked_list* g) {
	double* result;
	double eigenValue;

	result = (double*)malloc_struct(sizeof(double) * C->n);
	B_g_hat_mult_s(eigenvector, C, g, result, 1);
	eigenValue = vec_mult_vec(result, eigenvector, g);
	eigenValue -= C->norm1;
	free(result);
	return eigenValue;
}
/* ----------------------END eigenpair------------------------------*/

/* ---------------------- Modularity Maximization------------------------------*/

/* 2. creat the unmoved group as linked-list
* RETURN: head of the list
*/
vertex_head_list* create_unmoved_list(int* unmoved, int size) {
	vertex_linked_list* unmoved_node;
	vertex_head_list* head_unmoved;
	int i;

	unmoved_node = allocate_vertex_node(*unmoved, 0);
	head_unmoved = allocate_vertex_head(unmoved_node, size);

	for (i = 1; i < size; i++)
	{
		unmoved++;
		unmoved_node = allocate_vertex_node(*(unmoved), i);
		add_FIXED_node_to_vertex_list(head_unmoved, unmoved_node);
	}
	return head_unmoved;
}

/* 2. creat the unmoved group as linked-list
* RETURN: head of the list
*/
void rewind_unmoved_list(vertex_head_list* head_unmoved) {
	vertex_linked_list* unmoved_node;

	unmoved_node = head_unmoved->first_FIXED;
	head_unmoved->first = unmoved_node;
	head_unmoved->tail = unmoved_node;

	unmoved_node = unmoved_node->next_FIXED;
	
	while (unmoved_node != NULL)
	{	
		add_UNFIXED_node_to_vertex_list(head_unmoved, unmoved_node);	
		unmoved_node = unmoved_node->next_FIXED;
	}
}

/* 6. to 10. compute delta Q (score) for each unmoved vertex*/
void compute_score_of_unmoved(vertex_head_list* head_unmoved, char* s, spmat* B_hat, int* unmoved, linked_list* g, int moved_node) {
	double score_value;
	int v_num;
	vertex_linked_list* unmoved_node;

	unmoved_node = head_unmoved->first;

	while (unmoved_node != NULL)
	{
		v_num = unmoved_node->vertex_num;
		score_value = (unmoved_node->score_val) - (4*(double)(*(s + v_num))*(*(s+ moved_node))*(get_specific_node_val(B_hat, v_num, moved_node)));

		/* maintaine the maximun score*/
		if (score_value > head_unmoved->max_score) {
			head_unmoved->max_score = score_value;
			head_unmoved->max_score_node = unmoved_node;
		}
		unmoved_node->score_val = score_value;
		unmoved_node = unmoved_node->next;
	}
}

/* set the start score values*/
void initialized_score(spmat* B_g, linked_list* g, char* s, vertex_head_list* head_unmoved) {
	double* x, k_val, score_value;
	int size, v_num, * k, M;

	size = g->group_size; M = B_g->M; k = B_g->k;
	x = (double*)calloc_array(B_g->n, sizeof(double));
	B_g_hat_mult_s_char(s, B_g, g, x);

	vertex_linked_list* unmoved_node;

	unmoved_node = head_unmoved->first;

	while (unmoved_node != NULL)
	{
		v_num = unmoved_node->vertex_num;
		k_val = *(k + v_num);
		score_value = -2 * ((*(s + v_num) * (*(x + v_num))) + (k_val * k_val) / M);

		/* maintaine the maximun score*/
		if (score_value > head_unmoved->max_score) {
			head_unmoved->max_score = score_value;
			head_unmoved->max_score_node = unmoved_node;
		}
		unmoved_node->score_val = score_value;
		unmoved_node = unmoved_node->next;
	}

	free(x);
}

/* 11. to 19. move the vertex with the maximal score and set improve and indices values*/
void move_maximal_score_vertex(vertex_head_list* head_unmoved, char* s, int i) {
	vertex_linked_list* max_score;
	int j;
	double improve_val;

	max_score = head_unmoved->max_score_node;
	j = max_score->vertex_num;
	*(s + j) = -(*(s + j));

	*(head_unmoved->indices + i) = j;
	if (i == 0) {
		improve_val = max_score->score_val;
		*(head_unmoved->improve) = improve_val;
	}
	else {
		improve_val = *(head_unmoved->improve + i - 1) + max_score->score_val;
		*(head_unmoved->improve + i) = improve_val;
	}

	if (improve_val > head_unmoved->max_improve_val) {
		head_unmoved->max_improve_val = improve_val;
		head_unmoved->max_improve_index = i;
	}

	head_unmoved->max_score = -10000;
	remove_node_not_FIXED(max_score, head_unmoved);
}

/* 21. to 30. find the maximum improvement of s and update s accordingly*/
void find_best_improve(vertex_head_list* head_unmoved, char* s, int size, double* delta_Q) {
	int i_tag, i;
	int* indices;
	char* sj;
	double improve_val;

	i_tag = head_unmoved->max_improve_index;
	improve_val = head_unmoved->max_improve_val;;
	indices = head_unmoved->indices;

	for (i = size - 1; i > i_tag; i--) {
		sj = s + (*(indices + i));
		*(sj) = -(*(sj));
	}

	if (i_tag == size - 1) {
		*(delta_Q) = 0;
	}
	else {
		*(delta_Q) = improve_val;
	}

	head_unmoved->max_improve_val = -10000;
	head_unmoved->max_improve_index = -1;
}

/* Modularity Maximization algotithm - improve the divition*/
void improve_modularity(char* s, spmat* B_hat, linked_list* g) {
	int* unmoved;
	int size, i;
	double delta_Q;
	vertex_head_list* head_unmoved;

	unmoved = g->vertexes_group;
	size = g->group_size;

	delta_Q = 1;

	/* 1 and 31 - repeat until delta is non positive*/
	head_unmoved = create_unmoved_list(unmoved, size);
	while (delta_Q > EPSILON) {
		/*2. creat the unmoved group as linked-list*/
		rewind_unmoved_list(head_unmoved);
		initialized_score(B_hat, g, s, head_unmoved);

		/* 3. to 20. repeat until all vertexes moved*/
		for (i = 0; i < size; i++)
		{
			//compute_score_of_unmoved_old(head_unmoved, s, B_hat, unmoved, g); //TEMP!!!
			/* 11. to 19. move the vertex with the maximal score */
			move_maximal_score_vertex(head_unmoved, s, i);

			/* 6. to 10. compute delta Q (score) for each unmoved vertex*/
			compute_score_of_unmoved(head_unmoved, s, B_hat, unmoved, g, head_unmoved->max_score_node->vertex_num);			
		}

		/* 21. to 30. find the maximum improvement of s and update s accordingly*/
		find_best_improve(head_unmoved, s, size, &delta_Q);
	}
	free(head_unmoved->improve);
	free(head_unmoved->indices);
	free_vertexes_list(head_unmoved);
}
/* ----------------------END Modularity Maximization------------------------------*/


/*--------------------------Algorithm 2-----------------------------------*/

/* ALMOST AS B_g_hat_mult_s
* RETURN = modularity value, delta Q
*/
double cal_delta_modularity(spmat* B_g_hat, char* s, linked_list* g) {
	int size, i, curr_node, s_const_val;
	int* nodes;
	double modularity;

	size = g->group_size;
	nodes = g->vertexes_group;
	modularity = 0;


	for (i = 0; i < size; i++)
	{
		curr_node = *(nodes + i);
		s_const_val = (int)*(s + curr_node);
		modularity += s_const_val * mat_row_mult_vec_char(B_g_hat, curr_node, s, g, s_const_val);
	}
	return modularity;
}

/* classify to groups according to s */
void classify_the_groups(linked_list* g, char* s, linked_list* g1, linked_list* g2) {
	int i, size, curr_vertex;
	int* g_nodes, * g1_nodes, * g2_nodes;
	size = g->group_size;
	g1_nodes = g1->vertexes_group; g2_nodes = g2->vertexes_group; g_nodes = g->vertexes_group;

	for (i = 0; i < size; ++i) {
		curr_vertex = *(g_nodes);
		if (*(s + curr_vertex) == 1) {
			*(g1_nodes) = *(g_nodes);
			g1_nodes++;
		}
		else {
			*(g2_nodes) = *(g_nodes);
			g2_nodes++;
		}
		g_nodes++;
	}
}

void find_g1_g2_size(int* p_counter, int* n_counter, char* s, int size, int* g_nodes) {
	int p_c, n_c, i;

	p_c = 0;
	n_c = 0;
	for (i = 0; i < size; ++i) {
		if (*(s + *(g_nodes)) < 0) {
			n_c += 1;
		}
		else {
			p_c += 1;
		}
		g_nodes++;
	}
	*(p_counter) = p_c;
	*(n_counter) = n_c;
}

void divide_to_two_free_func(double* eigenvector_u1)
{
	free(eigenvector_u1);
}

/* Divide a group into two
	RETURN: two groups - g1, g2
*/
void divide_to_two(spmat* B_hat, char* s, linked_list* g, linked_list** g1, linked_list** g2) {
	int size, p_counter, n_counter, i, curr_v;
	int* g_nodes;
	double eigenvalue_beta1;
	double* eigenvector_u1;
	/*clock_t start, end;*/
	/*start = clock(); /* start compute eigen*/

	size = (g->group_size);
	g_nodes = g->vertexes_group;
	/* 1. compute eigenpair */

	eigenvector_u1 = find_leading_eigenvector(B_hat, g);
	eigenvalue_beta1 = find_leading_eigenvalue(eigenvector_u1, B_hat, g);

	/*end = clock(); /* end compute eigen*/
	/*cal_and_print_time("EIGENPAIR", start, end);*/

	/* 2. if negative set indivisible*/
	if (eigenvalue_beta1 <= EPSILON) {
		*g1 = allocate_list_object_empty();
		*g2 = allocate_list_object_empty();
		divide_to_two_free_func(eigenvector_u1);
		return;
	}

	/* 3. compute s according to u */
	p_counter = 0;
	n_counter = 0;
	for (i = 0; i < size; ++i) {
		curr_v = *(g_nodes + i);
		if (*(eigenvector_u1 + curr_v) < 0) {
			*(s + curr_v) = -1;
		}
		else {
			*(s + curr_v) = 1;
		}
	}

	/*start = clock(); /* start improve_modularity*/
	/* improve modularity*/
	improve_modularity(s, B_hat, g);
	/*end = clock(); /* start improve_modularity*/
	/*cal_and_print_time("improve_modularity", start, end);*/
	find_g1_g2_size(&p_counter, &n_counter, s, size, g_nodes);

	/* 4. check if modularity is not positive */
	if (cal_delta_modularity(B_hat, s, g) < 0) {
		*g1 = allocate_list_object_empty();
		*g2 = allocate_list_object_empty();
		divide_to_two_free_func(eigenvector_u1);
		return;
	}

	/* 5. divide according to s */
	*g1 = allocate_list_object(p_counter);
	*g2 = allocate_list_object(n_counter);

	classify_the_groups(g, s, *g1, *g2);
	divide_to_two_free_func(eigenvector_u1);
}

/*--------------------------END of Algorithm 2-----------------------------------*/


/*--------------------------Algorithm 3-----------------------------------*/
/* add modularity groups to their relevant group*/
void add_groups_to_lists(head_list* O, head_list* P, linked_list** w1, linked_list** w2, linked_list* g) {
	linked_list* g1, * g2;

	g1 = *w1;
	g2 = *w2;

	/* 3.3 */
	if (g1->group_size == 0 || g2->group_size == 0) {
		add_node(O, g);
		free_node(g1);
		free_node(g2);
	}
	/* 3.4 */
	else {
		free_node(g);
		if (g1->group_size == 1) {
			add_node(O, g1);
		}
		else {
			add_node(P, g1);
		}
		if (g2->group_size == 1) {
			add_node(O, g2);
		}
		else {
			add_node(P, g2);
		}
	}
}

/* divide the graph to all its clusters */
head_list* divide_to_modularity_groups(spmat* B_hat, int size) {
	head_list* P, * O;
	linked_list* node_all, * g1, * g2, * g;
	char* s;
	int i;

	/* 1. set trivial division P */
	P = allocate_head_list();
	node_all = allocate_list_object(size);
	for (i = 0; i < size; ++i) {
		*(node_all->vertexes_group + i) = i;
	}
	node_all->group_size = size;
	P->next = node_all;
	P->list_size += 1;

	/* 2. define group O */
	O = allocate_head_list();

	/* 3. initialize group division O - Repeat until P is empty*/

	s = (char*)calloc_array(size, sizeof(char));

	while (P->list_size != 0) {
		/* 3.1 */
		g = remove_and_return_node(P);
		/* 3.2 */
		divide_to_two(B_hat, s, g, &g1, &g2);
		/* 3.3 + 3.4*/
		add_groups_to_lists(O, P, &g1, &g2, g);
	}
	free(s);
	free(P);
	/* 4. RETURN: division */
	return O;
}
/*-------------------------- END Algorithm 3-----------------------------------*/



