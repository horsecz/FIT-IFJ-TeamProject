CC=gcc
CCFLAGS=-std=c99 -Wall -Wextra -Werror -g

OUT=compilerIFJ2020
MAIN=ifj2020
CODEFILE=inputFile.go

RUN=$(OUT) $(CODEFILE)

make: $(MAIN)

# PARTS

scanner.o: scanner.c scanner.h str.h
	$(CC) $(CFLAGSS) -c $<

#parser.o: parser.c parser.h symtable.h str.h
#	$(CC) $(CFLAGSS) -c $<

str.o: str.c str.h
	$(CC) $(CFLAGSS) -c $<

#symtable.o: symtable.c symtable.h str.h
#	$(CC) $(CFLAGSS) -c $<

# MAIN

$(MAIN).o: $(MAIN).c $(MAIN).h scanner.h str.h
	$(CC) $(CFLAGS) -c $<
	
$(MAIN): $(MAIN).o scanner.o str.o
	$(CC) $(CFLAGS) $(MAIN).o scanner.o str.o -o $(OUT)


# ADDITIONAL FEATURES

makeAndRun: $(MAIN)
	./$(RUN)

clean:
	rm -rf *.o $(OUT)
