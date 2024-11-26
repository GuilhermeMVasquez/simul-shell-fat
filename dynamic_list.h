#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

typedef struct {
    char **data;      // Pointer to the array of strings
    int size;         // Current number of elements
    int capacity;     // Maximum capacity of the array
} DynamicList;

// Initializes a dynamic list with a given capacity
void init_list(DynamicList *list, int initial_capacity);

// Frees all memory allocated by the list
void free_list(DynamicList *list);

// Gets the size of the list
int get_size(DynamicList *list);

// Adds a string to the end of the list
void append(DynamicList *list, const char *str);

// Retrieves a string at a given index
const char* get_at(DynamicList *list, int index);

#endif // DYNAMIC_LIST_H