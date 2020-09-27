/*
 * linkedList.h
 *
 *  Created on: Sep 1, 2020
 *      Author: ofir1
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

 /*----------------------------------- Vertexes Linked List ------------------------------*/

typedef struct _vertex_linked_list vertex_linked_list;
struct _vertex_linked_list {
	int vertex_num;
	int index_num;
	double score_val;
	double improve_val;
	vertex_linked_list* next;
	vertex_linked_list* prev;
	vertex_linked_list* next_FIXED;
	vertex_linked_list* prev_FIXED;
};

typedef struct _vertex_head_list {
	vertex_linked_list* first;
	vertex_linked_list* tail;
	vertex_linked_list* first_FIXED;
	vertex_linked_list* tail_FIXED;
	vertex_linked_list* max_score_node;
	double max_score;

	double* improve;
	double max_improve_val;
	int max_improve_index;

	int* indices;
}vertex_head_list;

/* create a vertex node to the linked list*/
vertex_linked_list* allocate_vertex_node(int vertex_num, int index_num);

/* create a vertex head to the linked list*/
vertex_head_list* allocate_vertex_head(vertex_linked_list* first_node, int size);

/* create a Fixed vertex node to the linked list  - will be free only when all list free*/
void add_FIXED_node_to_vertex_list(vertex_head_list* head, vertex_linked_list* node_to_add);

/* create a UnFixed vertex node to the linked list  - pointers change while running */
void add_UNFIXED_node_to_vertex_list(vertex_head_list* head, vertex_linked_list* node_to_add);

/* remove the node from the dynamic list, node var still exist*/
void remove_node_not_FIXED(vertex_linked_list* node, vertex_head_list* head);

/*free vertexes list- free all list include fixed node*/
void free_vertexes_list(vertex_head_list* head);
/*-----------------------------------END Vertexes Linked List ------------------------------*/

/*----------------------------------- Group Linked List ------------------------------*/

typedef struct _linked_list linked_list;
struct _linked_list {
	int* vertexes_group;
	int group_size;
	linked_list* next;
};

typedef struct _headlist {
	int list_size;
	linked_list* next;
} head_list;

/*create linked list struct - set group_size = size and calloc vertexes_group */
linked_list* allocate_list_object(int size);

/*create linked list struct - set group_size = 0 */
linked_list* allocate_list_object_empty();

/*create linked list head struct */
head_list* allocate_head_list();

/* add node to the linkedlist 'head' */
void add_node(head_list* head, linked_list* node);

/* remove first element in the list and return it*/
linked_list* remove_and_return_node(head_list* head);

/* free all list */
void free_linked_list(head_list* head);

/* free specific node */
void free_node(linked_list* node);

/*-----------------------------------END Group Linked List ------------------------------*/
#endif /* LINKEDLIST_H_ */
