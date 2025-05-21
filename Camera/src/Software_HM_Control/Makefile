# Compilador
CC = gcc

# Diretórios
SRC_DIR = ./src
SRC_H_DIR = ./include/src
INCLUDE_DIR = ./include
OBJ_DIR = ./build

# Flags de compilação
CFLAGS = -Wall -Wextra -O2 -I$(INCLUDE_DIR)
LDFLAGS = -lgpiod -DLOG_USE_COLOR -lpaho-mqtt3c -lcjson

# Nome do executável
TARGET = HM_Control

# Fontes e objetos
SOURCES = $(wildcard $(SRC_DIR)/*.c $(SRC_H_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regras
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)
