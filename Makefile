ifeq ($(OS), Windows_NT) 
    PLATFORM := Windows
	PROGRAM_NAME := splitter.exe
else
    PLATFORM := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
	PROGRAM_NAME := splitter
endif

CC := gcc
I := include/
S := src/
B := bin/

FIESTA_PATH := third-party/fiesta

ifeq ($(PLATFORM), Windows)
	RAYLIB_PATH := third-party/raylib-5.5_win64_mingw-w64/
else
	RAYLIB_PATH := third-party/raylib-5.5_linux_amd64/
endif

RAYLIB_LIB_PATH := $(RAYLIB_PATH)lib
RAYLIB_INCLUDE_PATH := $(RAYLIB_PATH)include

FLAGS := -I$(I) -I$(RAYLIB_INCLUDE_PATH) -I$(FIESTA_PATH)/include -std=c23 -L$(FIESTA_PATH)/lib -lfiesta -L$(RAYLIB_LIB_PATH) -lraylib

ifeq ($(PLATFORM), Windows)
	FLAGS += -lopengl32 -lgdi32 -lwinmm
else
	FLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
endif

OBJ_FILES = $(B)splitter.o $(B)array.o

$(B)$(PROGRAM_NAME): $(OBJ_FILES)
	$(CC) -o $@ $^ $(FLAGS)

$(B)%.o: $(S)%.c
	$(CC) -c -o $@ $^ $(FLAGS)

dbg: FLAGS += -g
dbg: $(B)$(PROGRAM_NAME)

opt: FLAGS += -O2
opt: $(B)$(PROGRAM_NAME)

clean:
	$(RM) $(B)$(PROGRAM_NAME) $(OBJ_FILES)