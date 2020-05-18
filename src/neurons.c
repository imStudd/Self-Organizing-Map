#include "neurons.h"

static size_t neurons_number;
static size_t neighborhood_radius;
static size_t map_width;
static size_t map_height;
static size_t nb_iteration_phase_1;
static size_t nb_iteration_phase_2;
static double learning_rate_phase_1;
static double learning_rate_phase_2;
static double random_min;
static double random_max;
static bool shuffle;
static bool gaussian;

Neurons *init_neurons() {
    Neurons *neurons = malloc(sizeof(Neurons));
    if (neurons == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    neurons->neuron = malloc(neurons_number * sizeof(Neuron_t));
    if (neurons->neuron == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        free(neurons);
        return NULL;
    }

    neurons->number_neurons = neurons_number;
    neurons->data_size = 0;

    return neurons;
}

int init_data_neurons(Neurons *const neurons, const Data *const data) {
    double *averages;

    if (neurons == NULL || data == NULL) return -1;

    neurons->data_size = data->values_data_size;

    averages = calculate_data_averages(data);
    if (averages == NULL) {
        fprintf(stderr, "Failed to calculate averages.\n");
        return -1;
    }

    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        neurons->neuron[i].data = malloc(neurons->data_size * sizeof(double));
        if (neurons->neuron[i].data == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            free(neurons);
            free(averages);
            return -1;
        }

        neurons->neuron[i].id = 0;

        for (size_t j = 0; j < neurons->data_size; ++j) {
            neurons->neuron[i].data[j] = averages[j] + random_double(random_min, random_max);
        }
    }

    free(averages);
    return 0;
}

int load_neurons_configuration(const Config_t *const config) {
    neurons_number = config->neurons_number;
    neighborhood_radius = config->neighborhood_radius;
    map_width = config->map_width;
    map_height = config->map_height;
    nb_iteration_phase_1 = config->nb_iteration_phase_1;
    nb_iteration_phase_2 = config->nb_iteration_phase_2;
    learning_rate_phase_1 = config->learning_rate_phase_1;
    learning_rate_phase_2 = config->learning_rate_phase_2;
    random_min = config->random_min;
    random_max = config->random_max;
    shuffle = config->shuffle;
    gaussian = config->gaussian;

    if (map_width * map_height != neurons_number) {
        fprintf(stderr, "Invalid map size or neurons number.\n");
        return -1;
    }

    return 0;
}

void free_neurons(Neurons *neurons) {
    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        free(neurons->neuron[i].data);
    }
    free(neurons->neuron);
    free(neurons);
}

int save_neurons(Neurons *neurons) {
    FILE *fp;

    if (neurons == NULL || neurons->neuron == NULL) return -1;

    fp = fopen("neurons.dat", "wb");
    if (fp == NULL) {
        return -1;
    }

    // Configuration
    if (fwrite(&neurons_number, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }
    if (fwrite(&map_height, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }
    if (fwrite(&map_width, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }

    // Data
    if (fwrite(&neurons->data_size, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }

    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        if (fwrite(&neurons->neuron[i].id, sizeof(size_t), 1, fp) < 1) {
            fclose(fp);
            return -1;
        }

        if (fwrite(neurons->neuron[i].data, sizeof(double) * neurons->data_size, 1, fp) < 1) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

int load_neurons(Neurons *neurons, const char *const path) {
    FILE *fp;

    if (neurons == NULL || neurons->neuron == NULL)
        return -1;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open neurons data file\n");

        return -1;
    }

    // Configuration
    if (fread(&neurons_number, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }
    neurons->number_neurons = neurons_number;

    if (fread(&map_height, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }
    if (fread(&map_width, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }

    // Data
    if (fread(&neurons->data_size, sizeof(size_t), 1, fp) < 1) {
        fclose(fp);
        return -1;
    }

    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        neurons->neuron[i].data = malloc(neurons->data_size * sizeof(double));
        if (neurons->neuron[i].data == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            fclose(fp);
            return -1;
        }

        if (fread(&neurons->neuron[i].id, sizeof(size_t), 1, fp) < 1) {
            fclose(fp);
            return -1;
        }

        if (fread(neurons->neuron[i].data, sizeof(double) * neurons->data_size, 1, fp) < 1) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

void train_neurons(Neurons *const neurons, const Data *const data) {
    int winner;
    double alpha;
    size_t *index = NULL,
           level = neighborhood_radius,
           t2 = nb_iteration_phase_1 / level;

    index = malloc(data->data_size * sizeof(size_t));
    if (index == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return;
    }

    for (size_t i = 0; i < data->data_size; ++i) {
        index[i] = i;
    }

    // Phase 1
    for (size_t t = 0; t < nb_iteration_phase_1; ++t) {
        // Neighborhood linear decrease
        if (!gaussian && t == t2) {
            --level;
            t2 += t2;
        }

        // alpha = learning_rate_phase_1 * exp(-(double)t / (double)nb_iteration_phase_1);
        alpha = learning_rate_phase_1 * (1.0 - (double)t / (double)nb_iteration_phase_1);

        if (shuffle) shuffle_array(index, data->data_size);

        for (size_t i = 0; i < data->data_size; ++i) {
            winner = best_match_unit(neurons, data->data[index[i]]);
            if (winner < 0) {
                free(index);
                return;
            }

            teach_neuron(neurons, data->data[index[i]], winner, winner, alpha, nb_iteration_phase_1, t, level);
            spread_to_neighbors(neurons, data->data[index[i]], winner, alpha, nb_iteration_phase_1, t, level);
        }
    }

    level = 1;

    // Phase 2
    for (size_t t = 0; t < nb_iteration_phase_2; ++t) {
        // alpha = learning_rate_phase_2 * exp(-(double)t / (double)nb_iteration_phase_2);
        alpha = learning_rate_phase_2 * (1.0 - (double)t / (double)nb_iteration_phase_2);

        if (shuffle) shuffle_array(index, data->data_size);

        for (size_t i = 0; i < data->data_size; ++i) {
            winner = best_match_unit(neurons, data->data[index[i]]);
            if (winner < 0) {
                free(index);
                return;
            }

            teach_neuron(neurons, data->data[index[i]], winner, winner, alpha, nb_iteration_phase_2, t, level);
            spread_to_neighbors(neurons, data->data[index[i]], winner, alpha, nb_iteration_phase_2, t, level);
        }
    }

    free(index);
}

size_t best_match_unit(const Neurons *const neurons, const Data_t data) {
    size_t winner, random;
    List *winners = all_best_match_units(neurons, data),
         *tmp = winners;

    if (winners == NULL) {
        return -1;
    }

    if (winners->id > 1) {
        random = rand() % winners->id;
        for (size_t r = 0; r < random; ++r) tmp = tmp->next;
    }
    winner = *(size_t *)tmp->data;

    free_list(winners);
    return winner;
}

List *all_best_match_units(const Neurons *const neurons, const Data_t data) {
    double distance = DBL_MAX, result;
    List *winners = NULL;

    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        result = euclidean_distance(data.values, neurons->neuron[i].data, neurons->data_size);

        if (result < distance) {
            distance = result;

            if (winners != NULL) {
                free_list(winners);
                winners = NULL;
            }

            winners = create_list_item(sizeof(size_t));
            if (winners == NULL) {
                return NULL;
            }
            *(size_t *)winners->data = i;

        } else if (result == distance) {
            List *new = create_list_item(sizeof(size_t));
            if (new == NULL) {
                free(winners);
                return NULL;
            }
            *(size_t *)new->data = i;

            winners = push_front(winners, new);
        }
    }

    return winners;
}

void spread_to_neighbors(Neurons *const neurons, const Data_t data, size_t bmu, double learning_rate, size_t t_total, size_t t, size_t level) {
    size_t x = (bmu % map_width),
           y = (bmu / map_width);

    for (size_t i = 1; i <= level; ++i) {
        for (size_t j = 0; j < 2 * i; ++j) {
            long long top_x = ((x - i) + j),
                      top_y = (y - i),
                      right_x = (x + i),
                      right_y = ((y - i) + j),
                      bot_x = ((x + i) - j),
                      bot_y = (y + i),
                      left_x = (x - i),
                      left_y = ((y + i) - j);

            if (top_x >= 0 && top_x < (long long)map_width && top_y >= 0) teach_neuron(neurons, data, bmu, top_x + map_width * top_y, learning_rate, t_total, t, level);
            if (right_x < (long long)map_width && right_y >= 0 && right_y < (long long)map_height) teach_neuron(neurons, data, bmu, right_x + map_width * right_y, learning_rate, t_total, t, level);
            if (bot_x >= 0 && bot_x < (long long)map_width && bot_y < (long long)map_height) teach_neuron(neurons, data, bmu, bot_x + map_width * bot_y, learning_rate, t_total, t, level);
            if (left_x >= 0 && left_y >= 0 && left_y < (long long)map_height) teach_neuron(neurons, data, bmu, left_x + map_width * left_y, learning_rate, t_total, t, level);
        }
    }
}

void teach_neuron(Neurons *const neurons, const Data_t data, size_t bmu, size_t neighbor, double learning_rate, size_t t_total, size_t t, size_t level) {
    double h = 1.0;
    size_t x_bmu = bmu % map_width,
           y_bmu = bmu / map_width,
           x_neur = neighbor % map_width,
           y_neur = neighbor / map_width;

    // Gaussian neighborhood function
    if (gaussian) {
        double dist = sqrt(((x_bmu - x_neur) * (x_bmu - x_neur)) + ((y_bmu - y_neur) * (y_bmu - y_neur))),
               s = exp(-((double)t / (double)t_total)),
               sigma = (double)level * s;

        if (dist > sigma * sigma) return;

        h = exp(-((dist * dist) / (2.0 * (sigma * sigma))));
    }

    for (size_t i = 0; i < neurons->data_size; ++i) {
        neurons->neuron[neighbor].data[i] += h * learning_rate * (data.values[i] - neurons->neuron[neighbor].data[i]);
    }
}

int label_neurons(Neurons *const neurons, const Data *const data) {
    List *winners = NULL;

    size_t *index = malloc(data->data_size * sizeof(size_t));
    if (index == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }

    for (size_t i = 0; i < data->data_size; ++i) {
        index[i] = i;
    }

    shuffle_array(index, data->data_size);

    for (size_t i = 0; i < data->data_size; ++i) {
        winners = all_best_match_units(neurons, data->data[index[i]]);
        if (winners == NULL) {
            return -1;
        }

        for (List *iter = winners; iter != NULL; iter = iter->next) {
            neurons->neuron[*(size_t *)iter->data].id = data->data[index[i]].class->id;
        }

        free_list(winners);
    }

    free(index);
    return 0;
}

void print_neurons_map(const Neurons *const neurons) {
    size_t x = 0;

    if (neurons == NULL || neurons->neuron == NULL) return;

    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        printf("\e[0;%ldm%ld\e[0m", 40 + (neurons->neuron[i].id % 8), neurons->neuron[i].id);

        if (++x == map_width) {
            x = 0;
            printf("\n");
        }
    }
}

void print_neurons_data(const Neurons *const neurons) {
    if (neurons == NULL || neurons->neuron == NULL) return;

    printf("N \t Class \t Data\n");

    for (size_t i = 0; i < neurons->number_neurons; ++i) {
        printf("%ld \t %ld \t ", i, neurons->neuron[i].id);

        for (size_t j = 0; j < neurons->data_size; ++j) {
            printf("%f \t ", neurons->neuron[i].data[j]);
        }

        printf("\n");
    }
}
