#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "data.h"
#include "neurons.h"
#include "script.h"
#include "utils.h"

int main(int argc, const char **const argv) {
    Config_t configs = {
        .max_features = 0,
        .neurons_number = 0,
        .neighborhood_radius = 0,
        .map_height = 0,
        .map_width = 0,
        .nb_iteration_phase_1 = 0,
        .nb_iteration_phase_2 = 0,
        .learning_rate_phase_1 = 0.0,
        .learning_rate_phase_2 = 0.0,
        .random_min = 0.0,
        .random_max = 0.0,
        .reduce_mode = 0,
        .print_data = false,
        .print_classes = false,
        .print_map = true,
        .load_neurons = false,
        .verbose = false,
        .shuffle = true,
        .gaussian = true,
        .config_path = NULL,
        .data_path = NULL,
        .data_names = NULL,
        .text = NULL,
        .neurons_path = NULL
    };
    Data *data = NULL;
    Neurons *neurons = NULL;
    char *buffer = NULL;

    srand(time(NULL));

    options(argc, argv, &configs);

    if (configs.verbose) {
        printf("Initialize configuration: ");
    }

    if (config_handler(configs.config_path == NULL ? "config.ini" : configs.config_path, &configs) < 0) {
        fprintf(stderr, "Configuration failed.\n");
        free_config(&configs);
        exit(EXIT_FAILURE);
    }

    if (configs.text != NULL) {
        if (configs.verbose) {
            printf("done\n");
            printf("Text preprocessing: ");
        }

#ifdef PY_
        buffer = text_preprocessing(configs.text, configs.max_features, configs.reduce_mode);
        if (buffer == NULL) {
            fprintf(stderr, "Preprocessing failed.\n");
            free(configs.config_path);
            free(configs.data_path);
            free(configs.data_names);
            exit(EXIT_FAILURE);
        }
#endif

    } else {
        if (configs.verbose) {
            printf("done\n");
            printf("Read data file: ");
        }

        buffer = read_file(configs.data_path);
        if (buffer == NULL) {
            fprintf(stderr, "File reading failed.\n");
            free(configs.config_path);
            free(configs.data_path);
            free(configs.data_names);
            exit(EXIT_FAILURE);
        }
    }

    if (configs.verbose) {
        printf("done\n");
        printf("Load data configuration: ");
    }

    if (load_data_configuration(&configs) < 0) {
        fprintf(stderr, "Failed to load data configuration.\n");
    }

    if (configs.verbose) {
        printf("done\n");
        printf("Initialize data: ");
    }

    data = init_data(buffer);
    if (data == NULL) {
        fprintf(stderr, "Data initialization failed.\n");
        free(buffer);
        free(configs.config_path);
        free(configs.data_path);
        free(configs.data_names);
        exit(EXIT_FAILURE);
    }

    if (configs.verbose) {
        printf("done\n");
        printf("Format data: ");
    }

    if (format_data(buffer, data) < 0) {
        fprintf(stderr, "Data formatting error.\n");
        free(buffer);
        free_data(data);
        free(configs.config_path);
        free(configs.data_path);
        free(configs.data_names);
        exit(EXIT_FAILURE);
    }

    free(buffer);
    buffer = NULL;

    if (configs.verbose) {
        printf("done\n");
        printf("Normalize data: ");
    }

    normalize_data(data);

    if (configs.print_data) {
        print_data(data);
    }

    if (configs.verbose) {
        printf("done\n");
        printf("Load neurons configuration: ");
    }

    if (load_neurons_configuration(&configs) < 0) {
        fprintf(stderr, "Failed to load neurons configuration.\n");
        free_data(data);
        free(configs.config_path);
        free(configs.data_path);
        free(configs.data_names);
        exit(EXIT_FAILURE);
    }

    if (configs.verbose) {
        printf("done\n");
        printf("Initialize neurons: ");
    }

    neurons = init_neurons();
    if (neurons == NULL) {
        fprintf(stderr, "Neurons initialization failed.\n");
        free_data(data);
        free(configs.config_path);
        free(configs.data_path);
        free(configs.data_names);
        exit(EXIT_FAILURE);
    }

    if (!configs.load_neurons) {
        if (init_data_neurons(neurons, data) < 0) {
            fprintf(stderr, "Neurons data initialization failed.\n");
            free_neurons(neurons);
            free_data(data);
            free(configs.config_path);
            free(configs.data_path);
            free(configs.data_names);
            exit(EXIT_FAILURE);
        }

        if (configs.verbose) {
            printf("done\n");
            printf("Train neurons: ");
        }

        train_neurons(neurons, data);

        if (configs.verbose) {
            printf("done\n");
            printf("Label neurons: ");
        }

        label_neurons(neurons, data);
    } else {
        if (configs.verbose) {
            printf("done\n");
            printf("Load neurons: ");
        }

        if (load_neurons(neurons, configs.neurons_path) < 0) {
            fprintf(stderr, "Failed to load neurons.\n");
            free_neurons(neurons);
            free_data(data);
            free(configs.config_path);
            free(configs.data_path);
            free(configs.data_names);
            exit(EXIT_FAILURE);
        }
    }

    if (configs.verbose) {
        printf("done\n");
        printf("Save neurons: ");
    }

    if (save_neurons(neurons) < 0) {
        fprintf(stderr, "Failed to save neurons.\n");
    }

    if (configs.verbose) {
        printf("done\n");
    }

    if (configs.print_map) {
        print_neurons_map(neurons);
    }

    if (configs.print_classes) {
        print_classes(data);
    }

    free_data(data);
    free_neurons(neurons);
    free_config(&configs);
}
