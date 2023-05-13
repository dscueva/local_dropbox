# Local Drop Box Instructions
This project will create a local server that is able to store files. It stores files by serializing the file based on its type and then it encrypts it. It also has the capability of sending files to a client from the ones it has stored. This project utilizes data structures to store files in the server and has a variety of algorithms to serialize and deserialize files. This project also creates a client which can send and request files to the serverl. It also serializes and deserializes files as it sends and requests files.

## How to run local drop box:
1. Open two system terminals
2. Run this command: cd server
3. Run this command: make clean
4. Run this command: make

### Starting a local server
6. Run this command: ./executable/server --hostname localhost:8081

### Connecting client to local server
8. Go to second terminal
9. Run this command: cd client
10. Run this command: make clean
11. Run this command: make

#### Sending files to server
13. Run this command: ./build/bin/release/client --hostname localhost:8081 --send (desired file you want to send to local server and make sure it is in the files folder)

#### Requesting files from server
15. Run this command: ./build/bin/release/client --hostname localhost:8081 --request (desired folder you want to pull from server) 
