#ifndef IA_OPTIONS_H
#define IA_OPTIONS_H

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/iniparser/iniparser.h"
#include "utils.h"

typedef struct Config_t {
    size_t max_features;
    size_t neurons_number;
    size_t neighborhood_radius;
    size_t map_height;
    size_t map_width;
    size_t nb_iteration_phase_1;
    size_t nb_iteration_phase_2;
    double learning_rate_phase_1;
    double learning_rate_phase_2;
    double random_min;
    double random_max;
    uint reduce_mode : 1;
    bool print_data;
    bool print_classes;
    bool print_map;
    bool load_neurons;
    bool verbose;
    bool shuffle;
    bool gaussian;
    char *config_path;
    char *data_path;
    char *data_names;
    char *text;
    char *neurons_path;
} Config_t;

int config_handler(const char *const path, Config_t *configs);
// int config_handler(IniDispatch *disp, void *v_other);

void options(int argc, const char **const argv, Config_t *const config);
void free_config(Config_t *config);

#endif  // IA_OPTIONS_H
