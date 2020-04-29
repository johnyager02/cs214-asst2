all:	main.c recursiveD.o stringFunc.o
	gcc -g main.c -o main recursiveD.o stringFunc.o

recursiveD.o:	recursiveD.c stringFunc.o
	gcc -c recursiveD.c

stringFunc.o:	stringFunc.c
	gcc -c stringFunc.c

clean:
	rm main; rm recursiveD.o; rm stringFunc.o

