all:
	g++ -std=c++17 -ggdb -c -o noentry_lib.o -fPIC noentry_lib.cpp
	ar rcs libnoentry.a noentry_lib.o

clean:
	rm -rf *.o
	rm -rf *.a