CFLAGS = -lstdc++ -std=c++17 -O3 -g

main.o: main.cpp
	gcc $(CFLAGS) -o main.o -c main.cpp

phraser.app: main.o
	gcc $(CFLAGS) -o phraser.app main.o

run: phraser.app
	./phraser.app --update -h - ./hello ./dest
	./phraser.app --update -vrok - ./hello ./dest ./ignored

clean:
	rm *.o *.app
