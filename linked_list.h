#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct node {
    void *data;
    struct node * next;
};

struct linked_list {
    int length;
    struct node * first;
};

/**
 * @brief create a new list (FIFO)
 * call list_free() to free allocated memory later
 * @result struct linked_list *
 */
struct linked_list * list_init(void);

/**
 * @brief free all the allocated memory of a linked-list
 * @param list pointer to the list to free
 */
void list_free(struct linked_list *list);

/**
 * @brief add an element at the top of the list
 * @param x data to add to the list
 * @param size_of_data size of x, used to allocate enough memory for x
 */
void list_push(struct linked_list *list, void *x, int size_of_data);

/**
 * @brief return the data first element of the list
 * @param list list from which to get the first element
 * @result void * data of the first element
 */
void * list_get_first(struct linked_list *list);

/**
 * @brief delete the first element of a list (don't return the data)
 * also free memory allacted for the first element
 * @param list, list from which to pop the first element
 */
void list_pop(struct linked_list *list);

/**
 * @brief return length of a list
 * @param list, list from which to get the length
 * @result length of the list
 * if list is a NULL pointer then len is 0 by default
 */
int list_len(struct linked_list *list);

#endif