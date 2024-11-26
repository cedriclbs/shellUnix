# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = fsh
LIBS = -lreadline

# Répertoires
SRC_DIR = src
INCLUDE_DIR = include
SRC_ROOT = .  # Répertoire racine pour fsh.c

# Fichiers sources
SOURCES = $(wildcard $(SRC_DIR)/*.c) $(SRC_ROOT)/fsh.c  # Ajouter fsh.c à la liste des sources
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o) $(SRC_ROOT)/fsh.o  # Ajouter fsh.o aux objets

# Cible principale
all: $(TARGET)

# Construction de l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

# Règle générique pour compiler les fichiers objets
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Règle spécifique pour fsh.o (puisque fsh.c est à la racine)
$(SRC_ROOT)/fsh.o: $(SRC_ROOT)/fsh.c $(INCLUDE_DIR)/builtins.h
	$(CC) $(CFLAGS) -c $(SRC_ROOT)/fsh.c -o $(SRC_ROOT)/fsh.o

# Nettoyage des fichiers générés
clean:
	rm -f $(TARGET) $(OBJECTS)

# Nettoyage complet
distclean: clean

.PHONY: all clean distclean
