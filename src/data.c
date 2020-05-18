#include "data.h"

static char *data_names = NULL;

Data *init_data(const char *const buffer) {
    char *buf = NULL, *token1, *save1, *token2, *save2;
    size_t size_line = 0, size_col = 0;
    Data *data = NULL;

    buf = malloc(strlen(buffer) * sizeof(char));
    if (buf == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    strcpy(buf, buffer);

    data = malloc(sizeof(Data));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        free(buf);
        return NULL;
    }

    token1 = strtok_r(buf, "\n", &save1);
    while (token1 != NULL) {
        if (size_col == 0) {
            token2 = strtok_r(token1, ",", &save2);
            while (token2 != NULL) {
                token2 = strtok_r(NULL, ",", &save2);
                ++size_col;
            }
        }

        ++size_line;
        token1 = strtok_r(NULL, "\n", &save1);
    }

    data->classes = NULL;
    data->data_size = size_line;
    data->values_data_size = size_col - 1;
    data->classes_size = 0;

    data->data = malloc(data->data_size * sizeof(Data_t));
    if (data->data == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        free(buf);
        free(data);
        return NULL;
    }

    for (size_t i = 0; i < data->data_size; ++i) {
        data->data[i].values = malloc(data->values_data_size * sizeof(double));
        if (data->data[i].values == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            free(buf);
            free(data);
            return NULL;
        }

        data->data[i].norm = 0.0;
    }

    free(buf);
    return data;
}

int load_data_configuration(const Config_t *const config) {
    size_t size = strlen(config->data_names);

    if (size == 0) return 0;

    data_names = malloc(size);
    if (data_names == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }
    strcpy(data_names, config->data_names);

    return 0;
}

void free_data(Data *data) {
    for (size_t i = 0; i < data->data_size; ++i) {
        free(data->data[i].values);
    }
    free_list(data->classes);
    free(data->data);
    free(data);
    free(data_names);
}

int format_data(char *const buffer, Data *const data) {
    size_t i = 0;
    char *token, *end;
    List *class = NULL;

    token = strtok(buffer, ",\n");
    while (token != NULL) {
        for (size_t j = 0; j < data->values_data_size; ++j) {
            data->data[i].values[j] = strtod(token, &end);
            if (end == token) {
                printf("Invalid character line %ld.\n", i + 1);
                return -1;
            }
            token = strtok(NULL, ",\n");
        }

        class = existing_class(data->classes, token);
        if (class == NULL) {
            class = create_list_item(strlen(token) * sizeof(char));
            strcpy((char *)class->data, token);

            data->classes = push_front(data->classes, class);
            if (data->classes == NULL) return -1;
            data->classes_size++;
            class = data->classes;
        }

        data->data[i].class = class;
        ++i;
        token = strtok(NULL, ",\n");
    }
    return 0;
}

void normalize_data(Data *const data) {
    for (size_t i = 0; i < data->data_size; ++i) {
        for (size_t j = 0; j < data->values_data_size; ++j) {
            data->data[i].norm += data->data[i].values[j] * data->data[i].values[j];
        }

        data->data[i].norm = sqrt(data->data[i].norm);

        if (data->data[i].norm != 0) {
            for (size_t j = 0; j < data->values_data_size; ++j) {
                data->data[i].values[j] /= data->data[i].norm;
            }
        }
    }
}

double *calculate_data_averages(const Data *const data) {
    double *averages = malloc(data->values_data_size * sizeof(double));
    if (averages == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    memset(averages, 0, data->values_data_size * sizeof(double));

    for (size_t i = 0; i < data->values_data_size; ++i) {
        for (size_t j = 0; j < data->data_size; ++j) {
            averages[i] += data->data[j].values[i];
        }
    }

    for (size_t i = 0; i < data->values_data_size; ++i) {
        averages[i] /= (double)data->data_size;
    }

    return averages;
}

void print_data(const Data *const data) {
    char *token, *buffer = NULL;

    printf(" N \t ");

    if (data_names != NULL) {
        buffer = malloc(strlen(data_names));
        if (buffer == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            return;
        }
        strcpy(buffer, data_names);

        token = strtok(buffer, ",");
        while (token != NULL) {
            printf("%s \t ", token);
            token = strtok(NULL, ",");
        }
    }

    // printf("Norm \n");
    printf("\n");

    for (size_t i = 0; i < data->data_size; ++i) {
        printf(" %ld \t ", i);

        for (size_t j = 0; j < data->values_data_size; ++j) {
            printf("%f \t  ", data->data[i].values[j]);
        }

        printf("%s\n", (char *)data->data[i].class->data);
        // printf("%f\n", data->data[i].norm); // buggy wtf
    }
    printf("Data size: %ld\t Values data size: %ld\t Number of classes: %ld\n", data->data_size, data->values_data_size, data->classes_size);

    free(buffer);
}

void print_classes(const Data *const data) {
    List *c = data->classes;

    printf("Id \t Name\n");

    while (c != NULL) {
        printf("%d \t %s\n", c->id, (char *)c->data);
        c = c->next;
    }

    printf("Number of classes: %ld\n", data->classes_size);
}
