# Variáveis
CXX = g++
FLAGS = -std=c++23

# Diretórios e arquivos
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
BIN_DIR = bin
APP_NAME = app
APP_SRC = $(wildcard $(SRC_DIR)/*.cpp)
APP_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(APP_SRC))

all: run

run: build
	./$(BIN_DIR)/$(APP_NAME)

build: $(BIN_DIR)/$(APP_NAME)

$(BIN_DIR)/$(APP_NAME): $(APP_OBJ)
	$(CXX) $(APP_OBJ) -o $@


$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(FLAGS)