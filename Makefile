CC=gcc
CCFLAGS=-std=c99 -Wall -Wextra -Werror -g

OUT=compilerIFJ2020
MAIN=ifj2020
CODEFILE=inputFile.go

RUN=$(OUT) <$(CODEFILE)

make: $(MAIN)

# PARTS

scanner.o: scanner.c scanner.h str.h
	$(CC) $(CCFLAGS) -c $<

#parser.o: parser.c parser.h symtable.h str.h
#	$(CC) $(CCFLAGS) -c $<

str.o: str.c str.h
	$(CC) $(CCFLAGS) -c $<

symtable.o: symtable.c symtable.h str.h
	$(CC) $(CCFLAGS) -c $<

# MAIN

$(MAIN).o: $(MAIN).c $(MAIN).h scanner.h str.h symtable.h
	$(CC) $(CCFLAGS) -c $<
	
$(MAIN): $(MAIN).o scanner.o str.o
	$(CC) $(CCFLAGS) $(MAIN).o scanner.o str.o symtable.o -o $(OUT)
	
# RUN WITH DEBUG


# ADDITIONAL FEATURES

debug: $(MAIN-D)

scanner-d.o: scanner.c scanner.h str.h
	$(CC) $(CCFLAGS) -DDEBUG -c $<

$(MAIN-D): $(MAIN).o scanner-d.o str.o
	$(CC) $(CCFLAGS) -DDEBUG $(MAIN).o scanner-d.o str.o symtable.o -o $(OUT)
	

run: $(MAIN)
	./$(RUN)

clean:
	rm -rf *.o $(OUT)
