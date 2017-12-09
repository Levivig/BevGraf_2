# Declaration of variables
CC = g++
INC_DIR = ./include
CC_FLAGS = -std=c++11 -Wno-deprecated

CFLAGS=-I$(INC_DIR)

ifeq ($(shell uname),Darwin)
	LIBS = -lm -framework OpenGL -framework GLUT
else ifeq ($(shell uname -o),Cygwin)
	LIBS = -lm -lopengl32 -lglu32 -lglut32
	LDFLAGS += -static-libgcc
endif

# File names
EXEC = kockaTorusz
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
