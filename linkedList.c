#include "linkedList.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>

/*--------------------------Vertexes Linked List-----------------------------------*/

/* create a vertex node to the linked list*/
vertex_linked_list* allocate_vertex_node(int vertex_num, int index_num) {
	vertex_linked_list* vertex_node;
	vertex_node = (vertex_linked_list*)malloc_struct(sizeof(vertex_linked_list));
	vertex_node->vertex_num = vertex_num;
	vertex_node->index_num = index_num;
	vertex_node->next = NULL;
	vertex_node->prev = NULL;
	vertex_node->next_FIXED = NULL;
	vertex_node->prev_FIXED = NULL;

	return vertex_node;
}

void add_FIXED_node_to_vertex_list(vertex_head_list* head, vertex_linked_list* node_to_add) {
	/* FIXED POINTERS*/
	head->tail_FIXED->next_FIXED = node_to_add;
	node_to_add->prev_FIXED = head->tail_FIXED;
	head->tail_FIXED = node_to_add;
}

void add_UNFIXED_node_to_vertex_list(vertex_head_list* head, vertex_linked_list* node_to_add) {
	/* NOF FIXED POINTERS*/
	head->tail->next = node_to_add;
	node_to_add->prev = head->tail;
	head->tail = node_to_add;
}
vertex_head_list* allocate_vertex_head(vertex_linked_list* first_node, int size) {
	vertex_head_list* head_unmoved;
	head_unmoved = (vertex_head_list*)malloc_struct(sizeof(vertex_head_list));
	head_unmoved->first = NULL;
	head_unmoved->tail = NULL;
	head_unmoved->first_FIXED = first_node;
	head_unmoved->tail_FIXED = first_node;

	head_unmoved->max_score_node = NULL;
	head_unmoved->max_score = -10000;

	head_unmoved->max_improve_val = -10000;
	head_unmoved->improve = (double*)calloc_array(size, sizeof(double));;

	head_unmoved->indices = (int*)calloc_array(size, sizeof(int));

	return head_unmoved;
}

/* remove the node from the dynamic list, node var still exist*/
void remove_node_not_FIXED(vertex_linked_list* node, vertex_head_list* head) {
	vertex_linked_list* prev_node, * next_node;

	prev_node = node->prev;
	next_node = node->next;

	if ((prev_node == NULL) && (next_node == NULL)) {
		head->first = NULL;
		head->tail = NULL;
	}
	else if (prev_node == NULL) {
		head->first = next_node;
		next_node->prev = NULL;
	}
	else if (next_node == NULL) {
		prev_node->next = NULL;
		head->tail = prev_node;
	}
	else {
		prev_node->next = next_node;
		next_node->prev = prev_node;
	}
}

void free_vertexes_list(vertex_head_list* head) {
	vertex_linked_list* remove_node, *next_remove;
	remove_node = head->first_FIXED;
	while (remove_node != NULL) {
		next_remove = remove_node->next_FIXED;
		free(remove_node);
		remove_node = next_remove;
	}
	free(head);
}
/*--------------------------END Vertexes Linked List-----------------------------------*/

/*--------------------------group linked list-----------------------------------*/

linked_list* allocate_list_object(int size) {
	linked_list* list_o;
	list_o = (linked_list*)malloc_struct(sizeof(linked_list));
	list_o->group_size = size;
	list_o->vertexes_group = calloc_array(size, sizeof(int));
	list_o->next = NULL;

	return list_o;
}

linked_list* allocate_list_object_empty() {
	linked_list* list_o;
	list_o = (linked_list*)malloc_struct(sizeof(linked_list));
	list_o->vertexes_group = NULL;
	list_o->group_size = 0;
	list_o->next = 0;
	return list_o;
}

head_list* allocate_head_list() {
	head_list* head;
	head = (head_list*)malloc_struct(sizeof(head_list));
	head->list_size = 0;
	head->next = NULL;
	return head;
}

void add_node(head_list* head, linked_list* node) {
	linked_list* cluster;
	cluster = head->next;
	head->next = node;
	node->next = cluster;
	head->list_size += 1;
}

linked_list* remove_and_return_node(head_list* head) {
	linked_list* node;
	node = head->next;
	head->next = node->next;
	head->list_size -= 1;
	return node;
}

void free_linked_list(head_list* head) {
	linked_list* curr_node;
	linked_list* release_node;
	curr_node = head->next;
	free(head);

	while (curr_node != NULL) {
		release_node = curr_node;
		curr_node = curr_node->next;
		free(release_node->vertexes_group);
		free(release_node);
	}
}

void free_node(linked_list* node) {
	if (node != NULL && node->vertexes_group != NULL)
		free(node->vertexes_group);
	free(node);
}
/*--------------------------group linked list-----------------------------------*/
