all:
	g++ src/bin/main.cpp -std=c++11 -Iinclude -o server
	mkdir executable
	mkdir -p received
	mv server executable
clean:
	rm -rf executable *.so *.a *.o main
	rm -rf received