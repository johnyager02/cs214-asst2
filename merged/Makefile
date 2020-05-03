all:	main.c recursiveD.o stringFunc.o manifestFunc.o WTF.o sendAndReceive.o ./client/clientSocket.c ./server/serverSocket.c
	gcc -g main.c -o main recursiveD.o stringFunc.o manifestFunc.o WTF.o sendAndReceive.o -lssl -lcrypto;
	gcc -g ./client/clientSocket.c -o ./client/WTF WTF.o recursiveD.o stringFunc.o manifestFunc.o sendAndReceive.o -lssl -lcrypto
	gcc -g ./server/serverSocket.c -o ./server/WTFserver recursiveD.o stringFunc.o manifestFunc.o sendAndReceive.o -lssl -lcrypto

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
	rm main; rm recursiveD.o; rm stringFunc.o; rm manifestFunc.o; rm WTF.o; rm sendAndReceive.o; rm ./client/WTF; rm ./server/WTFserver

cleanManifest:	main
	rm ./proj0/.Manifest

cleanProj0:
	rm ./server/proj0/.Manifest; rmdir ./server/proj0