CC=gcc
CCFLAGS=-Wall -Wextra -Werror

OUT=compilerIFJ2020
CODEFILE=inputFile.go

RUN=$(OUT) $(CODEFILE)

make:
	$(CC) -o $(OUT) $(CCFLAGS) ifj2020.c

makeAndRun:
	$(CC) -o $(OUT) $(CCFLAGS) ifj2020.c
	./$(RUN)

clean:
	rm $OUT