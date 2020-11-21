#
# File: Makefile
# Author: Dominik Horky
# Date: 2020-11-18
# 

# compiler, flags
CC=gcc
CCFLAGS=-std=c99 -Wall -Wextra -Werror -g -I $(SRC) -I $(TEST)

# executable name
OUT=compilerIFJ2020

# main module
MAIN=ifj2020

# source and test folder
SRC=src/
TEST=tests/

#
# MAKE
#

make: src $(MAIN)

# PARTS

scanner.o: $(SRC)scanner.c $(SRC)scanner.h $(SRC)str.h
	$(CC) $(CCFLAGS) -c $< -o $(SRC)scanner.o

parser.o: $(SRC)parser.c $(SRC)parser.h $(SRC)symtable.h $(SRC)str.h $(SRC)returns.h
	$(CC) $(CCFLAGS) -c $< -o $(SRC)parser.o

returns.o: $(SRC)returns.c $(SRC)returns.h
	$(CC) $(CCFLAGS) -c $< -o $(SRC)returns.o

str.o: $(SRC)str.c $(SRC)str.h
	$(CC) $(CCFLAGS) -c $< -o $(SRC)str.o

symtable.o: $(SRC)symtable.c $(SRC)symtable.h $(SRC)str.h
	$(CC) $(CCFLAGS) -c $< -o $(SRC)symtable.o

# MAIN

$(MAIN).o: $(MAIN).c $(MAIN).h $(SRC)scanner.h $(SRC)str.h $(SRC)symtable.h $(SRC)parser.h $(SRC)returns.h
	$(CC) $(CCFLAGS) -c $<
	
$(MAIN): $(MAIN).o $(SRC)scanner.o $(SRC)str.o $(SRC)symtable.o $(SRC)parser.o $(SRC)returns.o
	$(CC) $(CCFLAGS) $(MAIN).o $(SRC)scanner.o $(SRC)str.o $(SRC)symtable.o $(SRC)parser.o $(SRC)returns.o -o $(OUT)

#
# ADDITIONAL FEATURES
#

# ENABLE DEBUG MESSAGES

debug: $(MAIN)-d

$(SRC)scanner-d.o: $(SRC)scanner.c $(SRC)scanner.h $(SRC)str.h
	$(CC) $(CCFlAGS) -DDEBUG -c $< -o $(SRC)scanner-d.o

$(SRC)parser-d.o: $(SRC)parser.c $(SRC)parser.h $(SRC)symtable.h $(SRC)str.h $(SRC)returns.h
	$(CC) $(CCFlAGS) -DDEBUG -c $< -o $(SRC)parser-d.o

$(MAIN)-d: $(MAIN).o $(SRC)scanner-d.o $(SRC)str.o $(SRC)symtable.o $(SRC)returns.o $(SRC)parser-d.o
	$(CC) $(CCFLAGS) -DDEBUG $(MAIN).o $(SRC)scanner-d.o $(SRC)str.o $(SRC)symtable.o $(SRC)returns.o $(SRC)parser-d.o -o $(OUT)

# CLEAN MAKE OUTPUT(S)

clean:
	rm -rf src/*.o test/*.o *.o $(OUT)

# HELP

help:
	@printf "Usage of this Makefile:\n"
	@printf 	" make      \tStandard project compilation.\n"
	@printf 	" make debug\tCompile with DEBUG defined. Executable will output debug messages to stdout/stderr.\n"
	@printf 	" make clean\tRemove all makefile outputs. (executables, *.o files)\n"
	@printf 	" make test\tCompile and run (all) tests.\n"
	@printf 	" make test-c\tCompile only (all) tests.\n"


#
# COMPILE AND RUN TESTS
#

test-c: symtest

# SYMTABLE TEST

$(TEST)symtest.o: $(TEST)symtest.c $(SRC)symtable.h
	$(CC) $(CCFLAGS) -c $< -o $(TEST)symtest.o

symtest: $(TEST)symtest.o symtable.o
	$(CC) $(CCFLAGS) $(TEST)symtest.o $(SRC)symtable.o -o t-symtable

# RUN TESTS

test: test-c runtests

runtests:
	./t-*
