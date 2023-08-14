#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <ws2tcpip.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

void CreateSocket(int* socketDescriptor, int portNum) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup error" << std::endl;
        return;
    }
    //SOCK_DGRAM, soket türü udp olduðu için 
    //AF_INET, IPv4
    *socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0); 
    if (*socketDescriptor == -1) {
        std::cerr << "The socket could not be created" << std::endl;
        return;
    }

    struct sockaddr_in serverAddress {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNum);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //Soketin belirtilen adrese baðlanmasý
    if (bind(*socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "The socket could not be connected" << std::endl;
        return;
    }

    std::cout << "The socket was successfully connected" << std::endl;
}

//Belirtilen IP adresi ve port numarasýyla iliþkilendirme
void SendMessage(int socketDescriptor, const std::string& ipAddress, int portNum, const std::string& message) {
    struct sockaddr_in destAddress {};
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = htons(portNum);
    inet_pton(AF_INET, ipAddress.c_str(), &(destAddress.sin_addr));

    int result = sendto(socketDescriptor, message.c_str(), message.length(), 0,
        (struct sockaddr*)&destAddress, sizeof(destAddress));

    if (result == -1) {
        std::cerr << "The message could not be sent" << std::endl;
    }
    else {
        std::cout << "The message was sent successfully" << std::endl;
    }
}

//Soketten veri alma
void ReceiveMessage(int socketDescriptor, std::string& receivedMessage) {
    const int bufferSize = 1024;
    char receivedBytes[bufferSize];

    struct sockaddr_in fromAddress {};
    socklen_t fromAddressLength = sizeof(fromAddress);

    int bytesRead = recvfrom(socketDescriptor, receivedBytes, bufferSize, 0,
        (struct sockaddr*)&fromAddress, &fromAddressLength);

    if (bytesRead == -1) {
        std::cerr << "Data could not be retrieved" << std::endl;
    }
    else {
        receivedMessage = std::string(receivedBytes, bytesRead);
        std::cout << "Received message: " << receivedMessage << std::endl;
    }
}

int main() {
    std::cout << "Type 'C' to start the application as a client (client), 'S' to start it as a server (server)." << std::endl;

    std::string choice;
    std::cin >> choice;

    /*Ýstemci seçildiði durumda; 
    1-) Sunucu ile soket oluþturulur,
    2-) Sunucuya mesaj gönderilir, 
    3-) Sunucudan mesaj alýnýr */
    if (choice == "C" || choice == "c") {
        std::cout << "Starting client" << std::endl;
        int clientSocket;
        CreateSocket(&clientSocket, 60000);

        std::string serverIPAddress = "127.0.0.1";
        int serverPortNum = 60000;

        std::string messageToSend = "Merhaba Dünya"; //Sunucuya göndeilren mesaj 
        SendMessage(clientSocket, serverIPAddress, serverPortNum, messageToSend);

        std::string receivedMessage;
        ReceiveMessage(clientSocket, receivedMessage);

        closesocket(clientSocket);
    }
     /*Sunucu seçildiði durumda; 
    1-) Sunucu ile soket oluþturulur,
    2-) Ýstemciden gelen mesaj alýnýr */
    else if (choice == "S" || choice == "s") {
        std::cout << "Starting server" << std::endl;
        int serverSocket;
        CreateSocket(&serverSocket, 50000);

        std::string receivedMessage;
        ReceiveMessage(serverSocket, receivedMessage);

        struct sockaddr_in clientAddress {};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int bytesRead = recvfrom(serverSocket, nullptr, 0, 0, (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (bytesRead == -1) {
            std::cerr << "Data could not be retrieved" << std::endl;
        }
        else {
            std::cout << "The message was sent successfully" << std::endl;
        }
        closesocket(serverSocket);
    }
    else {
        std::cout << "Invalid election. The application is being closed." << std::endl;
        return 1;
    }

    return 0;
}
