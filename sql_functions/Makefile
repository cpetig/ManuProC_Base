CFLAGS= -Wall -fPIC  -fno-stack-protector 

CXXFLAGS= -Wall -fPIC  -fno-stack-protector -fno-exceptions -fno-rtti

CPPFLAGS=-I/usr/include/postgresql/9.5/server \
	-I/usr/include/postgresql/9.3/server \
	-I/usr/include/postgresql/9.1/server \
	-I/usr/include/postgresql/8.4x/server \
	-I/usr/include/postgresql/8.3x/server \
	-I/usr/include/postgresql/8.2/server \
        -I/usr/include/postgresql/8.4/server \
        -I/usr/include/postgresql/8.3/server \
        -I/usr/include/postgresql/8.2/server \
        -I/usr/include/postgresql/8.1/server \
        -I/usr/include/postgresql/8.0/server \
        -I/usr/include/postgresql/server \
        -I/usr/include/postgresql \
	-I/usr/lib/postgresql/include \
	-I../..

ALL=kw2date.so atoi.so ean_check.so ean_check_int.so
	
all: $(ALL)

%.so: %.o
	ld -shared -o $*.so $^

clean:
	rm -f *~ *.o $(ALL)

install:
	mkdir -p /usr/lib/postgresql/lib
	cp $(ALL) /usr/lib/postgresql/lib
