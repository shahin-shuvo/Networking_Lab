all: hello

hello: Mail_Server.o Mail_Client.o
	g++ Mail_Server.o Mail_Client.o -o hello

Mail_Server.o: Mail_Server.cpp 
	g++ Mail_Server.cpp
	./a.out 2222
	 

Mail_Client.o: Mail_Client.cpp
	g++ Mail_Client.cpp
	./a.out 127.0.0.1 shuvo@shuvo-cse-du:2222 "Intro" client2

clean:
	rm -rf *o hello
