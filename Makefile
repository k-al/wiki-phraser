CFLAGS = -lstdc++ -std=c++17 -O3 -g

main.o: main.cpp string_helper.h
	gcc $(CFLAGS) -o main.o -c main.cpp

phraser.app: main.o
	gcc $(CFLAGS) -o phraser.app main.o

run: phraser.app
	./phraser.app --update -h - ./hello ./dest
	./phraser.app --update -vrouk - ./hello ./dest ./ignored

clean:
	rm *.o *.app
