CC := g++
CFLAGS := -g -Wall

SRCDIR := src
BUILDDIR := build
BINDIR := bin

TARGET := $(BINDIR)/jstrings
INC := -I include

SRCEXT := cpp
SRC := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJ := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.o))

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR)/* $(TARGET)

init:
	mkdir $(BUILDDIR)
	mkdir $(BINDIR)

