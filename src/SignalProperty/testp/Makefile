
CC=g++
CFLAGS=-I.
DEPS = MpcProperty.hh MpcSignal.hh

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

testprop: testprop.o
	$(CC) -o testprop testprop.o

propchain: propchain.o
	$(CC) -o propchain propchain.o
	
clean:
	rm *.o
