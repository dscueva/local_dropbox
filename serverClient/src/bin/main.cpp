#include <stdio.h>
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
//*** CODE CITED FROM TUTORIALSPOINT TO CONNECT TO SERVER
int main(int argc, char **argv)

{
    char *hostChars = NULL;
    int portNo = 0;
    int sockfd, n;
    bool check_send = false;
    bool check_request = false;
    std::string hostString;
    std::string temp_string;
    // Check for the flags:
    if (argc != 5)
    {
        printf("Must be in this format:\n./build/bin/release/client --hostname localhost:8081 [--send or --request] [filename]");
        exit(1);
    }
    if ((strcmp(argv[1], "--hostname") == 0))
    {
        if ((strcmp(argv[3], "--send") == 0))
        {
            check_send = true;
        }
        else if ((strcmp(argv[3], "--request") == 0))
        {
            check_request = true;
        }
        else
        {
            printf("Must be in this format:\n./build/bin/release/client --hostname localhost:8081 [--send or --request] [filename]");
            exit(1);
        }
        int temp = 0;
        while (argv[2][temp] != ':')
        {
            hostString += argv[2][temp];
            temp++;
        }
        temp++;
        while (argv[2][temp] != NULL)
        {
            temp_string += argv[2][temp];
            temp++;
        }
        hostChars = (char *)hostString.c_str();
        portNo = stoi(temp_string);
    }
    //***
    // Connect to the server
    printf("Connecting to %s:%d.\n", hostChars, portNo);
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int newsockfd = sockfd;
    if (sockfd < 0)
    {
        perror("Error");
        exit(1);
    }
    server = gethostbyname(hostChars);
    if (server == NULL)
    {
        printf("Failed to connect.");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portNo);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }
    listen(sockfd, 5);
    printf("Connected to server.\n");
    std::string temp;
    const char *item_sent;
    int shift = 0;
    while (argv[4][shift] != NULL)
    {
        temp += argv[4][shift];
        shift++;
    }
    item_sent = temp.c_str();
    std::string request_string = (char *)temp.c_str();

    // will check if user wants to send or request a file
    if (check_send == true)
    {
        FILE *file_sent;
        long size;
        char *buffer;
        size_t final;
        file_sent = fopen(item_sent, "rb");
        if (file_sent == NULL)
        {
            printf("Failed to open file.");
            exit(1);
        }
        fseek(file_sent, 0, SEEK_END);
        size = ftell(file_sent);
        rewind(file_sent);
        buffer = (char *)malloc(sizeof(char) * size);
        if (buffer == NULL)
        {
            printf("Memory error");
            exit(1);
        }
        final = fread(buffer, 1, size, file_sent);
        if (final != size)
        {
            printf("Error when reading.");
            exit(1);
        }
        std::vector<unsigned char> bytes;
        int i = 0;
        while (buffer[i] != NULL)
        {
            bytes.push_back(buffer[i]);
            i++;
        }
        fclose(file_sent);
        free(buffer);
        // serialize the file
        std::string new_string = std::string(item_sent);
        std::string new_string_temp = new_string.substr(new_string.find_last_of("/") + 1);
        std::string file_name = new_string_temp;
        // fill in the struct for file
        struct File file_send = {.name = file_name, .bytes = bytes};
        // serialize the file
        std::vector<unsigned char> file_ser = pack109::serialize(file_send);
        file_ser = pack109::encrypt(file_ser);
        //this is what we'll return
        unsigned char final_send[file_ser.size()];
        for (int i = 0; i < file_ser.size(); i++)
        {
            final_send[i] = file_ser[i];
        }
        n = write(sockfd, final_send, file_ser.size());
        if (n < 0)
        {
            perror("ERROR");
            exit(1);
        }
        printf("Message sent.");
    }
    else if (check_request == true)
    {
        // name of the file
        std::string request_file_name = request_string.substr(request_string.find_last_of("/") + 1);
        struct Request request_file = {.name = request_file_name};
        // here we serialize the file then encrypt it
        std::vector<unsigned char> serialized_file = pack109::serialize(request_file);
        std::vector<unsigned char> encrypted_file = pack109::encrypt(serialized_file);
        // keep track of the size of the encrypted file
        int encrypted_size = encrypted_file.size();
        char buffer[65535];
        // loop through the size of the encrypted file then add it to the buffer
        for (int i = 0; i < encrypted_size; i++)
        {
            buffer[i] = encrypted_file[i];
        }
        // write to the server
        n = write(newsockfd, buffer, encrypted_file.size());
        if (n < 0)
        {
            perror("ERROR");
        }
        std::cout << "Message Sent.";
        bzero(buffer, 256);
        // read the response from the server
        n = read(sockfd, buffer, 0xfffff);
        std::vector<unsigned char> encrypted_server_response;
        unsigned char responseBuff[1000];
        if (n > 0)
        {
            for (int i = 0; i < n; i++)
            {
                encrypted_server_response.push_back(buffer[i]);
            }
        }
        if (encrypted_server_response.empty())
        {
            printf("No file received");
        }
        else
        {
            // decrypt the file response
            std::vector<unsigned char> decrypted_response = pack109::encrypt(encrypted_server_response);
            // deserialize the file before sending it to received
            File responseFile = pack109::deserialize_file(decrypted_response);
            FILE *file_req;
            string newName = "received/" + responseFile.name;
            file_req = fopen(newName.c_str(), "wb");
            if (file_req == NULL)
            {
                printf("Failed to open file: \"%s\"\nDone.\n", responseFile.name.c_str());
                exit(1);
            }
            //save the file
            for (int i = 0; i < responseFile.bytes.size(); i++)
            {
                //From a tutorial's point tutorial
                fputc(responseFile.bytes[i], file_req);
            }
            fclose(file_req);
            std::cout << "\n";
            printf("File was saved in received folder.");
        }
    }
}