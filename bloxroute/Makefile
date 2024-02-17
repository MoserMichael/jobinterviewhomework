HEADERS = bt.hpp util.hpp cryptoutil.hpp
OBJECTS = bt.o util.o main.o
LIBS=-lstdc++ -lcrypto

default: program

%.o: %.cpp $(HEADERS)
	gcc -g -c $< -o $@

program: $(OBJECTS)
	gcc $(OBJECTS) -o $@ $(LIBS)

clean:
	-rm -f $(OBJECTS)
	-rm -f program
