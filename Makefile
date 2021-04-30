# Paths to source files
INC_PATH = ./src/include/
OBJ_PATH = ./src/obj/
SRC_PATH = ./src/src/

# Name of created executable
PROJECT = mqttexp

# Names of files to compile (without extensions)
FILES = explorer gui

# Name of compressed archive
COMPRESS = n-xfabom01-xkeprt03.zip



#===========================================================================
LC_ALL = en_EN.utf8W
CFLAGS = -g -std=c++17 -O2 -pedantic -Wall -Wextra -I$(INC_PATH)
CPP = g++ $(CFLAGS) -o $@

OBJ_FILES = $(patsubst %, $(OBJ_PATH)%.o, $(FILES))

# Build only
build: $(PROJECT)

# Build & run
run: build
	./$(PROJECT)

# Create html documentation from source code
doxygen:
	@echo "Please for all that is holy, fix me!" > doc/hi

# Compresses needed files for compilation
pack: clean clean_doc
	zip -r $(COMPRESS) src examples doc Makefile

# Removes all object files
clean:
	rm -f $(OBJ_PATH)*

# Removes generated documentation
clean_doc:
	rm -r -f doc/*

# Create executable application
$(PROJECT): $(OBJ_FILES) $(OBJ_PATH)main.o
	$(CPP) $^

# Build main file
$(OBJ_PATH)main.o: $(SRC_PATH)main.cpp
	$(CPP) -c $<

# Build individual object files
$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp $(INC_PATH)%.h
	$(CPP) -c $<
