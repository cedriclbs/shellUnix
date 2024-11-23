# Variables
CC = gcc
CFLAGS = -Wall -Wextra -I.
TARGET = fsh
LIBS = -lreadline
OBJS = fsh.o \
       prompt.o \
       exit.o

# Cible principale
all: $(TARGET)

# Construction de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Règles pour chaque fichier objet
fsh.o: fsh.c
	$(CC) $(CFLAGS) -c $< -o $@

prompt.o: prompt.c prompt.h
	$(CC) $(CFLAGS) -c $< -o $@

exit.o: exit.c exit.h
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -f $(TARGET) $(OBJS)
