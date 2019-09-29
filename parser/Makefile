# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

CXXFLAGS = -std=c++11
# Note that rule for goal (parse) must be the first one in this file.

parse: parse.o scan.o
	g++ $(CXXFLAGS) -o parse parse.o scan.o

clean:
	rm *.o parse

test:
	./parse < testSuccess1.txt > output.txt
	./parse < testSuccess2.txt 
	./parse < testError1.txt	
	./parse < testError2.txt

parse.o: scan.h
scan.o: scan.h
