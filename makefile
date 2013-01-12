CC=gcc
CFLAGS=
OUTPUT=gmadconv

build: gmadconv
gmadconv: gmadconv.c
	$(CC) -o $(OUTPUT) gmadconv.c $(CFLAGS)
	@echo "Build Completed."

clean:
	@echo "Cleaning up."
	@rm -f $(OUTPUT)
