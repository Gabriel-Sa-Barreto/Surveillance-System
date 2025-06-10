# Compilador
CC = gcc

# Diretórios do subprojeto
LOCAL_SRC_DIR := src
LOCAL_INCLUDE_DIR := src/include
BUILD_DIR := build

# Diretórios de includes e fontes externos
INCLUDE_DIRS := $(LOCAL_INCLUDE_DIR) ../libs
SRC_DIRS := $(LOCAL_SRC_DIR) src/include/src ../libs/src

# Nome do executável
TARGET := $(BUILD_DIR)/Cam_Control

# Flags de compilação
CFLAGS := -Wall -Wextra -O2 $(foreach dir,$(INCLUDE_DIRS),-I$(dir)) -DLOG_USE_COLOR
LDFLAGS := -lgpiod -lpaho-mqtt3c -lcjson

# Lista de arquivos .c
SRC_FILES := $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))

# Mapeia cada arquivo .c para um arquivo .o em build/, usando nome único
OBJECTS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRC_FILES)))

# Regras
all: $(TARGET)

# Link final
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Regra para compilar cada .c (qualquer origem) em um .o no build/
# Usa uma regra que compara só pelo nome do arquivo, evita caminhos relativos quebrados
$(BUILD_DIR)/%.o:
	@src=$(firstword $(filter %/$*.c, $(SRC_FILES))); \
	mkdir -p $(dir $@); \
	$(CC) $(CFLAGS) -c $$src -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)