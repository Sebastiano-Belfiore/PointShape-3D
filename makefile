CC = gcc
TARGET = main.exe

all: $(TARGET)

# Il compilatore ora trova SDL2 da solo grazie a pacman!
$(TARGET): main.c
	$(CC) -g main.c -lmingw32 -lSDL2main -lSDL2 -lm -o $(TARGET) -Wall -W

# Questo comando compila (se necessario) e avvia l'app
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)