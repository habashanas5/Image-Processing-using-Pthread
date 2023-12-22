CC=g++
CFLAGS=-I –O3 -Wall -pthread
OBJ = program.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	

test1: serial
	./serial oimage.txt edge.txt output.txt


test2: parallel
	./parallel oimage.txt sharpen.txt outputpthread.txt 20
	
clean:
	rm -f *.o *~ core
