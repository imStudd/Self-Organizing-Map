#include "config.h"

int config_handler(const char *const path, Config_t *configs) {
    dictionary *ini;
    const char *s;

    ini = iniparser_load(path);
    if (ini == NULL) {
        fprintf(stderr, "Error configuration file: '%s'", path);
        return -1;
    }

    if (configs->data_path == NULL) {
        s = iniparser_getstring(ini, "data:data_path", NULL);
        configs->data_path = malloc(sizeof(char) * strlen(s));
        if (configs->data_path == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            return -1;
        }
        strcpy(configs->data_path, s);
    }

    s = iniparser_getstring(ini, "data:data_path", NULL);
    configs->data_names = malloc(sizeof(char) * strlen(s));
    if (configs->data_names == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }
    strcpy(configs->data_names, s);

    configs->neurons_number = iniparser_getlongint(ini, "neurons:neurons_number", 0);
    if (configs->neurons_number == 0) {
        fprintf(stderr, "Invalid or missing NEURONS_NUMBER\n");
        return -1;
    }

    configs->map_width = iniparser_getlongint(ini, "neurons:map_width", 0);
    if (configs->map_width == 0) {
        fprintf(stderr, "Invalid or missing MAP_WIDTH\n");
        return -1;
    }

    configs->map_height = iniparser_getlongint(ini, "neurons:map_height", 0);
    if (configs->map_height == 0) {
        fprintf(stderr, "Invalid or missing MAP_HEIGHT\n");
        return -1;
    }

    configs->neighborhood_radius = iniparser_getlongint(ini, "training:neighborhood_radius", 0);
    configs->nb_iteration_phase_1 = iniparser_getlongint(ini, "training:nb_iteration_phase_1", 0);
    configs->nb_iteration_phase_2 = iniparser_getlongint(ini, "training:nb_iteration_phase_2", 0);

    configs->learning_rate_phase_1 = iniparser_getdouble(ini, "training:learning_rate_phase_1", 0.0);
    if (configs->learning_rate_phase_1 <= 0) {
        fprintf(stderr, "Invalid or missing LEARNING_RATE_PHASE_1\n");
        return -1;
    }

    configs->learning_rate_phase_2 = iniparser_getdouble(ini, "training:learning_rate_phase_2", 0.0);
    if (configs->learning_rate_phase_2 <= 0) {
        fprintf(stderr, "Invalid or missing LEARNING_RATE_PHASE_2\n");
        return -1;
    }

    configs->random_min = iniparser_getdouble(ini, "training:random_min", 0.0);
    configs->random_max = iniparser_getdouble(ini, "training:random_max", 0.0);
    configs->shuffle = iniparser_getboolean(ini, "training:shuffle", true);
    configs->gaussian = iniparser_getboolean(ini, "training:gaussian", true);

    iniparser_freedict(ini);
    return 0;
}

void options(int argc, const char **const argv, Config_t *const config) {
    char *end;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
            printf(
                "Usage: %s [OPTIONS]\n\n"
                "Options:\n"
                "\t-h, --help\t\t\t\tDisplay options.\n"
                "\t-d, --data <file>\t\t\tSet data file (Data path in configuration file if not specified).\n"
                "\t-t, --text <file> <max features> <0|1>\tUse text data file, it will be preprocessed. Mode 0 keep m recurrent terms for reduce dimensionality and mode 1 use random projection\n"
                "\t-c, --config <file>\t\t\tSet configuration file (config.ini by default).\n"
                "\t-l, --load <file> \t\t\tLoad neurons file (.dat file).\n"
                "\t-v, --verbose \t\t\t\tPrint progression.\n"
                "\t--print_data \t\t\t\tPrint all data.\n"
                "\t--print_classes \t\t\tPrint all classes.\n"
                "\t--no_print_map \t\t\t\tDo not print the map.\n"
                "\nConfiguration file:\n"
                "\tDATA_PATH=<string> \t\t\tPath of data file.\n"
                "\tDATA_NAMES=<string> \t\t\tNames of each data column (csv format). (OPTIONAL)\n"
                "\tNEURONS_NUMBER=<unsigned int> \t\tNumber of neurons.\n"
                "\tMAP_WIDTH=<unsigned int> \t\tNeuron map width.\n"
                "\tMAP_HEIGHT=<unsigned int> \t\tNeuron map height.\n"
                "\tNEIGHBORHOOD_RADIUS=<unsigned int> \tInitial neighborhood radius.\n"
                "\tNB_ITERATION_PHASE_2=<unsigned int> \tNumber of training iteration.\n"
                "\tNB_ITERATION_PHASE_1=<unsigned int> \tNumber of training iteration for the first phase.\n"
                "\tLEARNING_RATE_PHASE_1=<unsigned int> \tLearning rate for the first phase.\n"
                "\tLEARNING_RATE_PHASE_2=<unsigned int> \tLearning rate for the second phase.\n"
                "\tRANDOM_MIN=<double> \t\t\tMinimum random value for neurons initialization. (OPTIONAL)\n"
                "\tRANDOM_MAX=<double> \t\t\tMaximum random value for neurons initialization. (OPTIONAL)\n"
                "\tSHUFFLE=<boolean> \t\t\tGet random data during training.\n"
                "\tGAUSSIAN=<boolean> \t\t\tGaussian neighbourhood function.\n",
                argv[0]);

            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--data") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing argument for '%s'\n", argv[i]);
                free_config(config);
                exit(EXIT_FAILURE);
            }

            if (config->text != NULL) {
                fprintf(stderr, "Please indicate only one data file\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }

            config->data_path = malloc(strlen(argv[i + 1]));
            if (config->data_path == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }
            strcpy(config->data_path, argv[i + 1]);

            ++i;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing argument for '%s'\n", argv[i]);
                free_config(config);
                exit(EXIT_FAILURE);
            }

            config->config_path = malloc(strlen(argv[i + 1]));
            if (config->config_path == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }
            strcpy(config->config_path, argv[i + 1]);

            ++i;
        } else if (strcmp(argv[i], "--print_data") == 0) {
            config->print_data = true;
        } else if (strcmp(argv[i], "--print_classes") == 0) {
            config->print_classes = true;
        } else if (strcmp(argv[i], "--no_print_map") == 0) {
            config->print_map = false;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--load") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing argument for '%s'\n", argv[i]);
                free_config(config);
                exit(EXIT_FAILURE);
            }

            config->load_neurons = true;

            config->neurons_path = malloc(strlen(argv[i + 1]));
            if (config->neurons_path == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }
            strcpy(config->neurons_path, argv[i + 1]);

            ++i;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--text") == 0) {
#ifndef PY_
            fprintf(stderr, "Text preprocessing is not available because Python 3.8 is not.\nPlease install Python 3.8, nltk, scikit-Learn and recompile.\n");
            free_config(config);
            exit(EXIT_FAILURE);
#endif
            if (i + 3 >= argc) {
                fprintf(stderr, "Missing argument for '%s'\n", argv[i]);
                free_config(config);
                exit(EXIT_FAILURE);
            }

            if (config->data_path != NULL) {
                fprintf(stderr, "Please indicate only one data file\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }

            config->text = malloc(strlen(argv[i + 1]));
            if (config->text == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }
            strcpy(config->text, argv[i + 1]);

            errno = 0;

            config->max_features = strtoull(argv[i + 2], &end, 10);
            if (*end || errno == ERANGE) {
                fprintf(stderr, "Invalid max features\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }

            config->reduce_mode = atoi(argv[i + 3]);
            if (config->reduce_mode > 1) {
                fprintf(stderr, "Invalid reduce mode\n");
                free_config(config);
                exit(EXIT_FAILURE);
            }

            i += 3;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            config->verbose = true;
        } else {
            fprintf(stderr, "Unknown option: '%s'\nTry --help for more information.\n", argv[i]);
            free_config(config);
            exit(EXIT_FAILURE);
        }
    }
}

void free_config(Config_t *config) {
    free(config->config_path);
    free(config->data_path);
    free(config->data_names);
    free(config->text);
    free(config->neurons_path);
}