#ifndef IA_UTILS_H
#define IA_UTILS_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define uint unsigned int

typedef struct List_t {
    void *data;
    struct List_t *next;
    int id;
} List;

char *read_file(const char *const path);
double euclidean_distance(const double *const p, const double *const q, size_t size);
void shuffle_array(size_t *const array, size_t size);
double random_double(double min, double max);
List *push_front(List *list, List *new);
List *create_list_item(size_t data_size);
List *existing_class(List *list, const char *const name);
void free_list(List *list);
int is_empty_line(const char *const string);

#endif  // IA_UTILS_H