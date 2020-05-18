#include "utils.h"
#include <time.h>

char *read_file(const char *const path) {
    FILE *fp;
    char *buffer = NULL;
    size_t size;

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open file %s\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(fp);
        return NULL;
    }

    if (fread(buffer, size, (size_t)1, fp) < 1) {
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return buffer;
}

double euclidean_distance(const double *const p, const double *const q, size_t size) {
    double res = 0.0;

    for (size_t i = 0; i < size; ++i) {
        res += (p[i] - q[i]) * (p[i] - q[i]);
    }

    return sqrt(res);
}

void shuffle_array(size_t *const array, size_t size) {
    size_t rdm, tmp;

    for (size_t i = 0; i < size; ++i) {
        rdm = rand() % size;
        tmp = array[rdm];
        array[rdm] = array[i];
        array[i] = tmp;
    }
}

double random_double(double min, double max) {
    return (min + ((double)rand() / RAND_MAX) * (max - (min)));
}

List *push_front(List *list, List *new) {
    if (new == NULL) return list;
    if (list == NULL) return new;

    new->next = list;
    new->id = list->id + 1;

    return new;
}

List *create_list_item(size_t data_size) {
    List *new = malloc(sizeof(List));
    if (new == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    new->data = malloc(data_size);
    if (new->data == NULL) {
        free(new);
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    new->id = 1;
    new->next = NULL;

    return new;
}

List *existing_class(List *list, const char *const name) {
    if (list == NULL) return NULL;

    while (list != NULL) {
        if (strcmp(name, (char *)list->data) == 0) return list;
        list = list->next;
    }

    return NULL;
}

void free_list(List *list) {
    List *tmp = list;

    while (list != NULL) {
        tmp = list;
        list = list->next;
        free(tmp->data);
        free(tmp);
    }
}