#include <stdio.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "../include/pack109.hpp"
#include "../src/lib.cpp"


using namespace std;
int main(int argc, char **argv)
{
    //CITED FROM HOMEWORK 8
    // Check for user input in command line and ensure it is in the right format
    if (argc != 3)
    {
        std::cout << "Must be in this format: ./executable/server --hostname [address:port]";
        exit(1);
    }
    if (strcmp(argv[1], "--hostname") == 0)
    {
    }
    else
    {
        std::cout << "Must be in this format: ./executable/server --hostname [address:port]";
        exit(1);
    }

    // Now we want to grab the address and port from the command line if the input was correct
    //CITED FROM HOMEWORK 8
    std::string address;
    std::string port;
    int temp = 0;
    while (argv[2][temp] != ':')
    {
        address += argv[2][temp];
        temp++;
    }
    temp++;
    while (argv[2][temp] != NULL)
    {
        port += argv[2][temp];
        temp++;
    }
    // convert it into an integer
    int portNo;
    portNo = stoi(port);

    // Now let's connect to the server
    // CODE CITED FROM TUTORIALSPOINT (https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm)
    int sockfd, newsockfd, clilen;
    int n;
    struct sockaddr_in serv_addr, cli_addr;
    //CONNECT TO A SOCKET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error:");
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNo);
    // Bind host address using bind()
    int bind_temp = ::bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (bind_temp < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }
    printf("Server listening on: %s:%d\n", address.c_str(), portNo);
    ::listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    //ACCEPT THE CONNECTION FROM THE CLIENT
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
    {
        perror("ERROR accepting");
        exit(1);
    }

    // Read message to a buffer
    vector<unsigned char> message;
    //large enough to make it work with all files
    char buffer[65536];
    //fill buffer with zero's
    bzero(buffer, 65536);
    n = read(newsockfd, buffer, 65536);
    if (n < 0)
    {
        perror("Error reading from the socket");
    }
    for (int i = 0; i < sizeof(buffer); i++)
    {
        if (buffer[i] != NULL)
        {
            message.push_back(buffer[i]);
        }
        else
        {
            break;
        }
    }

    std::cout << "Reading ";
    std::cout << message.size();
    std::cout << " bytes.\n";
    // Decrypt the message
    std::vector<unsigned char> decrypted = pack109::encrypt(message);
    //CHECK IF REQUEST OR SEND BASED ON THE THIRD INDEX

    //SEND == 4
    if (decrypted[3] == 4)
    {
        File file = pack109::deserialize_file(decrypted);
        std::cout << "Received File: ";
        std::cout << file.name.c_str();
        string file_dir = "received/";
        file_dir += file.name.c_str();
        ofstream file_temp(file_dir);
        vec final;
        for (int i = 0; i < decrypted.size(); i++)
        {
            final.push_back(decrypted[i]);
        }
        file.bytes = final;
        char send[final.size()];
        //INFO TO WRITE TO THE FOLDER AND STORE SO THAT WE COULD READ THIS LATER AND SEND IT BACK TO THE CLIENT
        for (int i = 0; i < final.size(); i++)
        {
            if (final[i] != NULL)
            {
                send[i] = final[i];
            }
        }
        //WORKED CITED TO WRITE TO THE FILE
        //https://cplusplus.com/reference/ostream/ostream/write/
        file_temp.write(send, decrypted.size());
        file_temp.close();
    }
    //REQUEST ==7 
    else if (decrypted[3] == 7)
    {
        Request request = pack109::deserialize_request(decrypted);
        std::cout << "Received Request: ";
        std::cout << request.name.c_str();
        string file_dir = "received/";
        file_dir += request.name.c_str();
        //WORK CITED TO CHECK IF FILE EXISTS
        //https://linuxhint.com/check-file-exists-cpp/
        std::ifstream myFile(file_dir);
        if(myFile){

        }
        else{
            std::cout << "\nEntered file doesn't exist";
            exit(1);
        }
        //READ DATA FROM THE FILE "RECEIVED/[FILE]"
        FILE *t = fopen(file_dir.c_str(), "rb");
        //WORK CITED USED TO GET LENGTH
        //https://www.geeksforgeeks.org/fseek-in-c-with-example/
        fseek(t, 0, SEEK_END);
        myFile.open(file_dir);
        long file_size = ftell(t);
        fseek(t, 0, SEEK_SET);
        char *string = (char *)malloc(file_size + 1);
        //WORK CITED TO READ DATA FROM FILE
        //https://www.geeksforgeeks.org/fread-function-in-c/
        fread(string, file_size, 1, t);
        fclose(t); 
        //FILL INFO FROM THE FILE TO A VECTOR TO SEND BACK TO THE CLIENT
        vec final;
        for (int i = 0; i < file_size; i++)
        {
            final.push_back((u8)string[i]);
        }
        File final_send;
        final_send.name = request.name.c_str();
        final_send.bytes = final;
        vec encrypted = pack109::encrypt(final_send.bytes);
        vec serialized = pack109::serialize(encrypted);
        //USE BUFFER TO SEND TO THE CLIENT BECAUSE WRITE METHOD ONLY WORKS WITH CHAR []
        char sendBuffer[encrypted.size()];
        bzero(sendBuffer, encrypted.size());

        for (int i = 0; i < encrypted.size(); i++)
        {
            sendBuffer[i] = encrypted[i];
        }
        printf("\nSending: %s",final_send.name.c_str());
        printf("\nMessage sent.");
        //SEnD MESSAGE TO THE CLIENT
        //FROM TUTORIALSPOINT
        n = write(newsockfd, sendBuffer, encrypted.size());
        if (n < 0)
        {
            perror("ERROR");
            exit(1);
        }
    }
}
