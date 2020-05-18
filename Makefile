CC = gcc
EXEC = som.out
RM = rm -rf

OBJDIR = obj

SRC = main.c \
	  data.c \
	  neurons.c \
	  config.c \
	  utils.c \
	  script.c 

OBJ := $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))

CFLAGS = -O3 -Wall -Wextra -I./lib/iniparser/
LDFLAGS = -lm -L./lib/iniparser/ -liniparser

ifeq ($(shell find . -name "obj"),)
  $(shell mkdir -p obj)
endif


CONFIG = $(shell find . -name "config.ini")
PYTHONCFLAGS = $(shell python3-config --libs --embed --cflags)
PYTHONLDFLAGS = $(shell python3-config --libs --embed --ldflags)
ifneq ($(PYTHONCFLAGS),)
  ifneq ($(PYTHONCFLAGS),)
    CFLAGS += -D PY_ $(PYTHONCFLAGS) -fPIC
    LDFLAGS += $(PYTHONLDFLAGS)
  endif
endif

ifeq ($(CONFIG),)
  $(file >config.ini,[Data])
  $(file >>config.ini,DATA_PATH=)
  $(file >>config.ini,DATA_NAMES= ; OPTIONAL)
  $(file >>config.ini,)
  $(file >>config.ini,[Neurons])
  $(file >>config.ini,NEURONS_NUMBER=)
  $(file >>config.ini,MAP_WIDTH=)
  $(file >>config.ini,MAP_HEIGHT=)
  $(file >>config.ini,)
  $(file >>config.ini,[Training])
  $(file >>config.ini,NEIGHBORHOOD_RADIUS=)
  $(file >>config.ini,NB_ITERATION_PHASE_1=)
  $(file >>config.ini,NB_ITERATION_PHASE_2=)
  $(file >>config.ini,LEARNING_RATE_PHASE_1=)
  $(file >>config.ini,LEARNING_RATE_PHASE_2=)
  $(file >>config.ini,RANDOM_MIN= ; OPTIONAL)
  $(file >>config.ini,RANDOM_MAX= ; OPTIONAL)
  $(file >>config.ini,SHUFFLE=)
  $(file >>config.ini,GAUSSIAN=)
endif

$(EXEC): $(OBJ) 
	$(CC) -o $@ $(OBJ) $(LDFLAGS) 
	
$(OBJDIR)/%.o:	src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	$(RM) $(OBJDIR) script/__pycache__

cleanall:
	$(RM) $(OBJDIR) $(EXEC) script/__pycache__ neurons.dat config.ini