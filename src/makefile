CC := g++

FLAGS := -Wall
FLAGS += -Wextra
FLAGS += -pedantic
#FLAGS += -Weffc++
FLAGS += -Wold-style-cast
FLAGS += -Woverloaded-virtual
FLAGS += -std=c++17

LIB := -lssl
LIB += -lcrypto
LIB += -lboost_json

DEPENDS := websocket.o
DEPENDS += commands.o
DEPENDS += client.o

all: a.out

a.out: main.o $(DEPENDS)
	$(CC) $(FLAGS) main.o $(DEPENDS) -o a.out $(LIB)

main.o: main.cc
	$(CC) $(FLAGS) -c main.cc -o $@ $(LIB)

%.o: %.cc %.h
	$(CC) $(FLAGS) -c $< -o $@ $(LIB)

clean:
	rm a.out main.o $(DEPENDS)
