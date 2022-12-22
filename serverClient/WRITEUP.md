Writeup (Works Cited Below.)

How to build the client:

1. Open up a system terminal not in VS Code to run the server.

2. Run this command to run the server: ./bin/fileserver-mac 

* NOTE: If you are on MAC OS you must run these commands first:
    1.  chmod +x ./fileserver-mac
    2.  ./bin/fileserver-mac &

3. Open up VS Code terminal and run this command: make clean

4. Run this command: make

5. Execute the executable file by running this command in this format: ./build/bin/release/client --hostname localhost:8081 [--send or --request] [filename directory]

*NOTE: In order to request a file you must send it first.

**NOTE: If localhost:8081 does not work you may use 127.0.0.1:[Port listed on system terminal]



Process of Building in this Project & Decisions made explained:
- First I started this project by making a source folder and creating a main.cpp file.
- I worked on the flags needed and made sure there was a specific format to run the client
- I also kept track of whether or not they wanted to do a request or send by a variable and kept track of the file that they would want to send or request
- Afterwards I connected to the server and followed along Professor Montella and the source (tutorialpoint.com) he provided's code to connect to a server
- If the user wants to send a file I first tried to open the file that they wanted to send by running fopen and if it doesn't I exited out of the program, because send won't work if the file does not open.
- I then made sure the file was not empty and if it was I exited out of the program so there won't be issues later on
- I then serialize the file's name and I then place that and the file size in a structure I created called file
- I also serialize the file using the methods I placed in lib. I created the method based on Professor Montella's Person serialization
- I then encrypt the file using a specific key
- Afterwards I placed that in an unsigned char which I will then fill in with the information in the vector I serialized the file into
- I then write to the server that serialized file and if didn't serialize properly it will return an error.
- Now if a user wants to serialize I first place the name of the file into a struct I created called Request
- I then serialize and encrypt the file that they want to serialize
- Afterwards I create a buffer and fill it up with the elements of the encrypted file
- I then write this file to the server.
- I then read the response from the server and it tells me whether or not the operation was successful
- I then create a vector to the store the elements from the buffer
- If this vector is empty it means that the file did not encrypt properly so I exit out with an error
- Otherwise, I decrypt the file response and place it into a file struct
- I then try to open the file using a command called fopen and if it is not empty then it means it worked fine
- I then save the file into a folder called received by using a command called fputc
- The program then has completed all possible tasks.

How did I construct my serialize, deserialize, and encrypt functions?
- I tried to base them off Professor Montella's homework solutions and changed slight details such as variable names and made them fit into the parameters needed for this assignment to store file name and file size.
- I created my encrypt function by looping through a vector of bytes and encyrpting based on a specific key.




Works Cited: 
- https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm
- Professor Montella's Homework Solution
- https://www.tutorialspoint.com/c_standard_library/c_function_fseek.htm 
(Used to see into file and make sure it works)