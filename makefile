OBJDIR := obj
SRCDIR := src

CC := g++

FLAGS := -Wall
FLAGS += -Wextra
FLAGS += -pedantic
#FLAGS += -Weffc++
FLAGS += -Wold-style-cast
FLAGS += -Woverloaded-virtual
FLAGS += -std=c++20

LIB := -lssl
LIB += -lcrypto
LIB += -lboost_json

DEPENDS := $(OBJDIR)/main.o
DEPENDS += $(OBJDIR)/websocket.o
DEPENDS += $(OBJDIR)/commands.o
DEPENDS += $(OBJDIR)/client.o
DEPENDS += $(OBJDIR)/util.o

all: $(OBJDIR) a.out

$(OBJDIR):
	mkdir $@

a.out: $(DEPENDS)
	$(CC) $(FLAGS) $(DEPENDS) -o $@ $(LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(SRCDIR)/%.h
	$(CC) $(FLAGS) -c $< -o $@ $(LIB)

$(OBJDIR)/main.o: $(SRCDIR)/main.cc
	$(CC) $(FLAGS) -c $< -o $@ $(LIB)

clean:
	rm a.out $(DEPENDS)
