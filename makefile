CC := gcc
TARGET := AFibCutter
CFLAGS = -std=c99 -I $(IDIR) -pthread

ODIR := ./obj
SDIR := ./src
IDIR := ./inc

_DEPS := argParser.h fileHandler.h thread.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := main.o argParser.o fileHandler.o thread.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(TARGET)
