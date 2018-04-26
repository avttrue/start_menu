CC=g++
CFLAGS= -std=c++0x -pthread
SOURCES=main.cpp cfg.cpp mygetch.cpp
EXECUTABLE=s-menu

all: $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	$(RM) $(EXECUTABLE)
