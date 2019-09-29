# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

CXXFLAGS = -std=c++11
# Note that rule for goal (parse) must be the first one in this file.

parse: parse.o scan.o
	g++ $(CXXFLAGS) -o parse parse.o scan.o

clean:
	rm *.o parse output.txt

test:
	echo "TEST CASE 1: \n" >> output.txt
	./parse < testSuccess1.txt >> output.txt
	echo "\nTEST CASE 2: \n" >> output.txt
	./parse < testSuccess2.txt >> output.txt
	echo "\nTEST CASE 3: \n" >> output.txt
	./parse < testError1.txt >> output.txt	
	echo "\nTEST CASE 4: \n" >> output.txt
	./parse < testError2.txt >> output.txt

parse.o: scan.h
scan.o: scan.h
