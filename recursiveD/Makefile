all:	main.c recursiveD.o stringFunc.o manifestFunc.o
	gcc -g main.c -o main recursiveD.o stringFunc.o manifestFunc.o -lssl -lcrypto

recursiveD.o:	recursiveD.c stringFunc.o manifestFunc.o
	gcc -c recursiveD.c

stringFunc.o:	stringFunc.c
	gcc -c stringFunc.c

manifestFunc.o: manifestFunc.c stringFunc.o recursiveD.o
	gcc -c manifestFunc.c

clean:
	rm main; rm recursiveD.o; rm stringFunc.o; rm manifestFunc.o

cleanManifest:	main
	rm ./proj0/.Manifest
