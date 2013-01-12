CC=gcc
CFLAGS=
OUTPUT=bin/gmadconv

build: gmadconv
gmadconv: gmadconv.c
	@mkdir -p bin
	$(CC) -o $(OUTPUT) gmadconv.c $(CFLAGS)
	@echo "Build Completed."
clean:
	@echo "Cleaning up."
	@rm -f $(OUTPUT)
