CFLAGS := -O2 \
	-Wall \
	-Wextra \
	-Werror \
	-fsanitize=address,undefined \
	-fno-common \
	-Winit-self \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-align \
	-Wstrict-prototypes \
	-Wstrict-overflow=5 \
	-Wwrite-strings \
	-Waggregate-return \
	-Wcast-qual \
	-Wswitch-default \
	-Wno-discarded-qualifiers \
	-Wno-aggregate-return

FILES := $(shell find . -name "*.c" -not -name "predict.c")
FILES_PREDICT := $(shell find . -name "*.c" -not -name "linreg.c")

build:
	$(CC) $(CFLAGS) $(FILES) -lm -g -o linreg
	$(CC) $(CFLAGS) $(FILES_PREDICT) -lm -g -o predict
