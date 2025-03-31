CC = gcc
CFLAGS = -Wall -I../include

PARSER_SRC = ../src/parser/parser.c
LEXER_SRC = ../src/lexer/lexer.c
SEMANTIC_SRC = ../src/semantic/semantic.c
MAIN_SRC = main.c
OBJ = parser.o lexer.o semantic.o main.o

TARGET = compiler.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

parser.o: $(PARSER_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

lexer.o: $(LEXER_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

semantic.o: $(SEMANTIC_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: $(MAIN_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	del /Q $(OBJ) $(TARGET) 2>nul || echo "Files already cleaned"

.PHONY: all clean