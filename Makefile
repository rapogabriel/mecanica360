
# Diretórios e arquivos
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
BIN_DIR = bin
APP_NAME = app
APP_SRC = $(wildcard $(SRC_DIR)/*.cpp)
APP_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(APP_SRC))
DLL_EXT = .so
QT_INCLUDE = -isystem /usr/include/x86_64-linux-gnu/qt6 -isystem /usr/include/x86_64-linux-gnu/qt6/QtCore -isystem /usr/include/x86_64-linux-gnu/qt6/QtGui -isystem /usr/include/x86_64-linux-gnu/qt6/QtWidgets
QT_LIBS = -lQt6Widgets -lQt6Gui -lQt6Core
MOC_BIN = /usr/lib/qt6/libexec/moc

# Variáveis
CXX = g++
COMPILE_FLAGS = -I$(INCLUDE_DIR) -std=c++23
DLL_COMPILE_FLAGS = -shared -fPIC

all: run

run: build
	./$(BIN_DIR)/$(APP_NAME)

build: $(BIN_DIR)/$(APP_NAME)

$(BIN_DIR)/$(APP_NAME): $(APP_OBJ)
	$(CXX) $(APP_OBJ) -o $@ $(QT_LIBS) -L$(LIB_DIR)


$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(COMPILE_FLAGS) $(QT_INCLUDE)

build-dll:
	@for pasta in $(SRC_DIR)/*/; do \
		if [ -d "$$pasta" ]; then \
			nome=$$(basename "$$pasta"); \
			$(MAKE) moc PASTA="$$pasta" NOME="$$nome"; \
			$(CXX) $(DLL_COMPILE_FLAGS) $$pasta/*.cpp -o $(LIB_DIR)/lib$$nome$(DLL_EXT) -DCOMPILANDO_A_BIBLIOTECA $(QT_INCLUDE) $(QT_LIBS) $(CXX_COMPILE_FLAGS); \
			rm -rf $$pasta/moc_$$nome.cpp; \
		fi; \
	done

moc:
	@$(MOC_BIN) $(INCLUDE_DIR)/$(NOME).hpp -o $(PASTA)/moc_$(NOME).cpp

clean:
	rm -rf $(BIN_DIR)/$(APP_NAME) $(BIN_DIR)/*$(DLL_EXT) $(LIB_DIR)/*$(DLL_EXT) $(BIN_DIR)/*.o