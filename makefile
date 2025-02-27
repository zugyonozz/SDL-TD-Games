# Compiler
CXX = g++

# Direktori
SDL_DIR = Src
INCLUDE_DIR = $(SDL_DIR)/include/SDL2
LIB_DIR = $(SDL_DIR)/lib

# Flags
CXXFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra
LDFLAGS = -L$(LIB_DIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# Target
TARGET = main.exe # File yang ingin telah di compile
SRC = main.cpp # Sesuaikan nama file yang ingin di compile
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	rm -f $(OBJ)
	./$(TARGET)
