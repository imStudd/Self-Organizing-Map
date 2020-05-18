#ifndef IA_DATA_H
#define IA_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "utils.h"

typedef struct Data_t {
    double *values;
    double norm;
    List *class;
} Data_t;

typedef struct Data {
    Data_t *data;
    List *classes;
    size_t data_size;
    size_t values_data_size;
    size_t classes_size;
} Data;

Data *init_data(const char *const buffer);
int load_data_configuration(const Config_t *const config);
void free_data(Data *data);

int format_data(char *const buffer, Data *const data);
void normalize_data(Data *const data);
double *calculate_data_averages(const Data *const data);

void print_data(const Data *const data);
void print_classes(const Data *const data);

#endif  // IA_DATA_H
