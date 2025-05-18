CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/main.c src/file.c src/file_header.c src/parse.c
OUT = mydb

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
