CC := g++
CFLAGS := -g -Wall

SRCDIR := src
BUILDDIR := build
TARGET := bin/jstrings

OBJ := jstrings.o main.o
INC := -I include

SRCEXT := cpp
SRC := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJ := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.o))

$(TARGET): $(OBJ)
	$(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR)/* $(TARGET)

init:
	mkdir build
	mkdir bin