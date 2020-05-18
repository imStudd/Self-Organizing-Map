# Self-Organizing Map
## Prerequisites
- Python 3.8 (OPTIONAL)
    - nltk
    - numpy

Python is an optional prerequisite, it will be used for preprocessing if the data to be processed is text.<br/>
The method used for preprocessing is [TF-IDF](https://en.wikipedia.org/wiki/Tf%E2%80%93idf) which is a fairly basic method.

## Usage

`````````````````````````````
make
./som.out [OPTIONS]
`````````````````````````````

### Options

| Option | Argument(s) | Description |
| -----------| :-----------: | ----------- |
| -h, --help | - | Display options. |
| -d, --data | \<path\> | Set data file (If not specified here, the data ). |
| -t, --text | \<path\> <max_features> <0\|1> | Use text data file, it will be preprocessed.<br/>Mode 0 keep m recurrent terms for reduce dimensionality<br/>and mode 1 use random projection.|
| -c, --config | \<path\> | Set configuration file (config.ini by default). |
| -l, --load | \<path\> | Load neurons file (.dat file). |
| -v, --verbose | - | Print progression. |
| --print_data | - | Print all data. |
| --print_classes | - | Print all classes. |
| --no_print_map | - | Do not print the map. |


### Configuration
| Name | Type | Description |
| -----------| ----------- | ----------- |
| DATA_PATH | string | Path of data file. |
| DATA_NAMES | string | Names of each data column (csv format). (OPTIONAL) |
| NEURONS_NUMBER | unsigned int | Number of neurons. |
| MAP_WIDTH | unsigned int | Neuron map width. |
| MAP_HEIGHT | unsigned int | Neuron map height. |
| NEIGHBORHOOD_RADIUS | unsigned int | Initial neighborhood radius. |
| NB_ITERATION_PHASE_2 | unsigned int | Number of training iteration. |
| NB_ITERATION_PHASE_1 | unsigned int | Number of training iteration for the first phase. |
| LEARNING_RATE_PHASE_1 | unsigned int | Learning rate for the first phase. |
| LEARNING_RATE_PHASE_2 | unsigned int | Learning rate for the second phase. |
| RANDOM_MIN | double | Minimum random value for neurons initialization. (OPTIONAL) |
| RANDOM_MAX | double | Maximum random value for neurons initialization. (OPTIONAL) |
| SHUFFLE | boolean | Get random data during training. |
| GAUSSIAN | boolean | Gaussian neighbourhood function. |

Use [Iniparser](https://github.com/ndevilla/iniparser) library as INI file parser.

### Data format

Data must be in csv format. <br/>
Each value must be a numerical value except the last one which corresponds to the class, can be a string.

`````````````````````````````
4.9, 3.0, 1.4, 0.2, Iris-setosa
4.7, 3.2, 1.3, 0.2, Iris-setosa
4.6, 3.1, 1.5, 0.2, Iris-setosa
...
`````````````````````````````

## Example
Example with [Fisher's Iris data set](https://en.wikipedia.org/wiki/Iris_flower_data_set) ([downloadable here](https://archive.ics.uci.edu/ml/datasets/Iris)).

![Result](https://user-images.githubusercontent.com/51682641/82221648-8fdf4a00-9920-11ea-82e3-1bbab6445391.png)


## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.