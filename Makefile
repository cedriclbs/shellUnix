# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = fsh
LIBS = -lreadline

# Récupère automatiquement les fichiers sources et génère la liste des objets correspondants
SRC_DIR = src
INCLUDE_DIR = include
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

# Cible principale
all: $(TARGET)

# Construction de l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

# Règle générique pour compiler les fichiers objets
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -f $(TARGET) $(OBJECTS)