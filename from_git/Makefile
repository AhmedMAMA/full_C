# Répertoires du projet

BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj


# Compilateur et éditeur de liens

CC = gcc
LD = gcc


# Options de compilation et de linkage

INC = -I$(INC_DIR)

CFLAGS = $(INC) -Wall -Wextra -std=c99 -O0 -g -pg
LDFLAGS = -lm -pg


# Liste des fichiers objets

OBJ_FILES = \
	$(OBJ_DIR)/main.o \
	$(OBJ_DIR)/decodage_MCU.o \
	$(OBJ_DIR)/image_ppm.o \
	$(OBJ_DIR)/decode_JPEG.o \
	$(OBJ_DIR)/jpeg_reader.o \
	$(OBJ_DIR)/bitstream.o \
	$(OBJ_DIR)/huffman.o


# Exécutable final

TARGET = $(BIN_DIR)/jpeg2ppm


# Cible par défaut

all: $(TARGET)


# Édition des liens

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $(TARGET)


# Compilation des fichiers source

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/decodage_MCU.o: $(SRC_DIR)/decodage_MCU.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/image_ppm.o: $(SRC_DIR)/image_ppm.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/decode_JPEG.o: $(SRC_DIR)/decode_JPEG.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/jpeg_reader.o: $(SRC_DIR)/jpeg_reader.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/bitstream.o: $(SRC_DIR)/bitstream.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/huffman.o: $(SRC_DIR)/huffman.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# Nettoyage

.PHONY: all clean

clean:
	rm -f $(TARGET) $(OBJ_FILES)
	rm -f images/*.pgm images/*.ppm