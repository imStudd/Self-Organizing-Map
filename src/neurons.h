#ifndef IA_NEURONS_H
#define IA_NEURONS_H

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "data.h"
#include "utils.h"

typedef struct Neuron_t {
    size_t id;
    double *data;
} Neuron_t;

typedef struct Neurons {
    Neuron_t *neuron;
    size_t number_neurons;
    size_t data_size;
} Neurons;

Neurons *init_neurons();
int init_data_neurons(Neurons *const neurons, const Data *const data);
int load_neurons_configuration(const Config_t *const config);
void free_neurons(Neurons *neurons);
int save_neurons(Neurons *neurons);
int load_neurons(Neurons *neurons, const char *const path);
void train_neurons(Neurons *const neurons, const Data *data);
size_t best_match_unit(const Neurons *const neurons, const Data_t data);
List *all_best_match_units(const Neurons *const neurons, const Data_t data);
void spread_to_neighbors(Neurons *const neurons, const Data_t data, size_t bmu, double learning_rate, size_t t_total, size_t t, size_t level);
void teach_neuron(Neurons *const neurons, const Data_t data, size_t bmu, size_t neighbor, double learning_rate, size_t t_total, size_t t, size_t level);
int label_neurons(Neurons *const neurons, const Data *const data);
void print_neurons_map(const Neurons *const neurons);
void print_neurons_data(const Neurons *const neurons);

#endif  // IA_NEURONS_H
