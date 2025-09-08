CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

SRCDIR = src
OBJDIR = build
BINDIR = bin
OBJ = $(OBJDIR)/main.o
OUT = $(BINDIR)/program

all: $(OUT)

# Build the final executable
$(OUT): $(OBJ) | $(BINDIR)
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)

# Compile the source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create Build dir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Create Bin dir
$(BINDIR):
	mkdir -p $(BINDIR)

# Remove all generated files and directories
clean:
	rm -rf $(OBJDIR) $(BINDIR)