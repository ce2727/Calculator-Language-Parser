CSC 254 Assignment 2
Collaborators: Jacquelyn Moreno and Clayton Emmel

BUILD/RUN INSTRUCTIONS:
    cd parser
    make
    make test
    make clean

OUTPUT is written to file: 
    output.txt

From the 'parser' directory, type 'make' to build our program. 

Type 'make test' to run our test scenarios. There are 4 scenarios, 2 error-free calculator programs which should output a valid syntax tree, as well as 2 calculator programs which contain errors and will indicate these errors and continue parsing. To check the output of our tests, open the ‘output.txt’ file which will have the results.

You can also use your own test files(after doing ‘make’) by typing ‘./parse < input.txt’ where ‘input.txt’ is the file you are testing. OR, you can enter a program manually from the command line, typing ctrl-D to signify EOF.

Type ‘make clean’ to clean the directory of all the junk. 

