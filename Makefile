CFLAGS = -lstdc++ -std=c++17 -O3 -g

main.o: main.cpp string_helper.hpp phraser.hpp args.hpp logger.hpp phraser_helper.hpp
	gcc $(CFLAGS) -o main.o -c main.cpp

logger.o: logger.cpp logger.hpp string_helper.hpp
	gcc $(CFLAGS) -o logger.o -c logger.cpp

args.o: args.cpp args.hpp string_helper.hpp
	gcc $(CFLAGS) -o args.o -c args.cpp

string_helper.o: string_helper.hpp string_helper.cpp
	gcc $(CFLAGS) -o string_helper.o -c string_helper.cpp

phraser.o: phraser.cpp phraser.hpp phraser_helper.hpp logger.hpp args.hpp string_helper.hpp
	gcc $(CFLAGS) -o phraser.o -c phraser.cpp

phraser_helper.o: phraser_helper.cpp phraser_helper.hpp string_helper.hpp
	gcc $(CFLAGS) -o phraser_helper.o -c phraser_helper.cpp

phraser.app: main.o logger.o args.o string_helper.o phraser.o phraser_helper.o
	gcc $(CFLAGS) -o phraser.app main.o logger.o args.o string_helper.o phraser.o phraser_helper.o

run: phraser.app
	./phraser.app --update update.sh -vds - ./testbox ./dest

clean:
	rm -f *.o *.app
