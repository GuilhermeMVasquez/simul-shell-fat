#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_list.h"

#define RESIZE_FACTOR 2 // Factor by which to resize the list when full

void init_list(DynamicList *list, int initial_capacity) {
    list->data = (char **)malloc(initial_capacity * sizeof(char *));
    list->size = 0;
    list->capacity = initial_capacity;
}

void free_list(DynamicList *list) {
    for (int i = 0; i < list->size; i++) {
        free(list->data[i]);
    }
    free(list->data);
}

int get_size(DynamicList *list) {
    return list->size;
}

void append(DynamicList *list, const char *str) {
    if (list->size == list->capacity) {
        list->capacity *= RESIZE_FACTOR;
        list->data = (char **)realloc(list->data, list->capacity * sizeof(char *));
    }
    list->data[list->size] = strdup(str);
    list->size++;
}

const char* get_at(DynamicList *list, int index) {
    if (index < 0 || index >= list->size) {
        return NULL; // Return NULL if index is out of bounds
    }
    return list->data[index];
}
