#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "linked_list.h"
#include "scalpa.h"

struct linked_list * list_init(void) {
    struct linked_list *list= malloc(sizeof(struct linked_list));
    MCHECK(list);
    list->length = 0;
    list->first = NULL;
    return list;
}

void list_free(struct linked_list *list) {
    if (list == NULL) {
        return;
    }
    struct node * temp = list->first;
    while (temp != NULL) {
        temp = temp->next;
        list_pop(list);
    }
    free(list);
}

void list_push(struct linked_list *list, void *x, int size_of_data) {
    if (list == NULL) {
        handle_error("list_push() can't push an element to a NULL "
            "pointer\n");
    }
    struct node *new_element = malloc(sizeof(struct node));
    MCHECK(new_element);
    list->length ++;
    new_element->data = malloc(size_of_data);
    MCHECK(new_element->data);
    for (int i = 0; i < size_of_data; i++) {
        *((uint8_t *)(new_element->data + i)) = *((uint8_t *)(x + i));
    }
    new_element->next = list->first;
    list->first = new_element;
}

void * list_get_first(struct linked_list *list) {
    if (list == NULL) {
        handle_error("list_get_first() can't get data of first element of a "
            "NULL pointer\n");
    }
    return list->first->data;
}

void list_pop(struct linked_list *list) {
    if (list == NULL) {
        handle_error("list_pop() can't pop an element from a NULL "
            "pointer\n");
    }
    if (list->length == 0) {
            return;
    }
    struct node *first = list->first;
    list->length --;
    list->first = first->next;
    free(first->data);
    free(first);
}

int list_len(struct linked_list *list) {
    if (list == NULL) {
        return 0;
    }
    return list->length;
}