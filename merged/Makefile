all:	main.c recursiveD.o stringFunc.o manifestFunc.o WTF.o sendAndReceive.o
	gcc -g main.c -o main recursiveD.o stringFunc.o manifestFunc.o WTF.o sendAndReceive.o -lssl -lcrypto

recursiveD.o:	recursiveD.c stringFunc.o manifestFunc.o
	gcc -c recursiveD.c

stringFunc.o:	stringFunc.c
	gcc -c stringFunc.c

manifestFunc.o: manifestFunc.c stringFunc.o
	gcc -c manifestFunc.c

WTF.o:	WTF.c manifestFunc.o stringFunc.o recursiveD.o sendAndReceive.o
	gcc -c WTF.c

sendAndReceive.o:	sendAndReceive.c manifestFunc.o stringFunc.o
	gcc -c sendAndReceive.c

clean:
	rm main; rm recursiveD.o; rm stringFunc.o; rm manifestFunc.o; rm WTF.o; rm sendAndReceive.o

cleanManifest:	main
	rm ./proj0/.Manifest
