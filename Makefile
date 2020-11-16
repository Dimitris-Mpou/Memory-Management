OBJS = main.o hash.o
SOURCE = main.c
OUT = myexe
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT)

main.o: main.c
	$(CC) $(FLAG) main.c

hash.o: hash.c
	$(CC) $(FLAG) hash.c

clean:
	rm -f $(OBJS) $(OUT)
