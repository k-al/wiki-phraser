CFLAGS = -lstdc++ -std=c++17 -O3 -g

main.o: main.cpp string_helper.hpp phraser.hpp args.hpp logger.hpp
	gcc $(CFLAGS) -o main.o -c main.cpp

logger.o: logger.cpp logger.hpp string_helper.hpp
	gcc $(CFLAGS) -o logger.o -c logger.cpp

args.o: args.cpp args.hpp string_helper.hpp
	gcc $(CFLAGS) -o args.o -c args.cpp

string_helper.o: string_helper.hpp string_helper.cpp
	gcc $(CFLAGS) -o string_helper.o -c string_helper.cpp

phraser.app: main.o string_helper.hpp logger.o args.o string_helper.o
	gcc $(CFLAGS) -o phraser.app main.o logger.o args.o string_helper.o

run: phraser.app
	./phraser.app --update update.sh -vods - ./hello ./dest ./ignored

clean:
	rm *.o *.app
