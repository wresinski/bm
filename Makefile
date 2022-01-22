CC = gcc

bm:bm.c
	$(CC) bm.c -o bm

clean:
	rm -rf bm *.o 

