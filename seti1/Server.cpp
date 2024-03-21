#define WIN32_LEAN_AND_MEAN // To speed up the build process: https://stackoverflow.com/questions/11040133/what-does-defining-win32-lean-and-mean-exclude-exactly

#include <iostream>
#include <windows.h>
#include <map>
#include <string>
// #include <winsock2.h>
#include <ws2tcpip.h> // WSADATA type; WSAStartup, WSACleanup functions and many more
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015" // a port is a logical construct that identifies a specific process or a type of network service - https://en.wikipedia.org/wiki/Port_(computer_networking)

#define PAUSE 1000

int main()
{
    map<const char*, const char*> question_answer_map;

    question_answer_map["Whats the weather like today?"] = "The weather today is sunny, with a temperature around 25 degrees Celsius.";
    question_answer_map["How much does delivery to my house cost?"] = "The cost of delivery depends on your location, please specify the address.";
    question_answer_map["What is the warranty period for this item?"] = "This item comes with a 1-year warranty.";
    question_answer_map["What promotions are currently active?"] = "We currently have the '20% Discount on the Second Item' promotion.";
    question_answer_map["Can I order takeout?"] = "Certainly, you can order takeout, our menu is available on the website.";
    question_answer_map["Where is the nearest branch of your company located?"] = "The nearest branch is located at: Lenin Street, No. 15.";
    question_answer_map["What payment methods do you accept?"] = "We accept payment in cash, by bank transfer, and through payment systems.";
    question_answer_map["How long will the delivery take?"] = "Delivery usually takes between 1 to 3 working days.";
    question_answer_map["Do you have loyalty programs for regular customers?"] = "Yes, we have a loyalty program that offers discounts and bonuses.";
    question_answer_map["What services do you offer for new customers?"] = "For new customers, we offer a free consultation and a discount on the first purchase.";

    setlocale(0, "");
    system("title SERVER SIDE");
    cout << "процесс сервера запущен!\n";

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize Winsock
    WSADATA wsaData; // The WSADATA structure contains information about the Windows Sockets implementation: https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // The WSAStartup function initiates use of the Winsock DLL by a process: https://firststeps.ru/mfc/net/socket/r.php?2
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << "\n";
        cout << "подключение Winsock.dll прошло с ошибкой!\n";
        return 1;
    }
    else {
        cout << "подключение Winsock.dll прошло успешно!\n";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct addrinfo hints; // The addrinfo structure is used by the getaddrinfo function to hold host address information: https://docs.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // The Internet Protocol version 4 (IPv4) address family
    hints.ai_socktype = SOCK_STREAM; // Provides sequenced, reliable, two-way, connection-based byte streams with a data transmission mechanism
    hints.ai_protocol = IPPROTO_TCP; // The Transmission Control Protocol (TCP). This is a possible value when the ai_family member is AF_INET or AF_INET6
    hints.ai_flags = AI_PASSIVE; // The socket address will be used in a call to the "bind" function

    // Resolve the server address and port
    struct addrinfo* result = NULL;
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << "\n";
        cout << "получение адреса и порта сервера прошло c ошибкой!\n";
        WSACleanup(); // The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll): https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
        return 2;
    }
    else {
        cout << "получение адреса и порта сервера прошло успешно!\n";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Create a SOCKET for connecting to server
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed with error: " << WSAGetLastError() << "\n";
        cout << "создание сокета прошло c ошибкой!\n";
        freeaddrinfo(result);
        WSACleanup();
        return 3;
    }
    else {
        cout << "создание сокета на сервере прошло успешно!\n";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); // The bind function associates a local address with a socket: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << "\n";
        cout << "внедрение сокета по IP-адресу прошло с ошибкой!\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 4;
    }
    else {
        cout << "внедрение сокета по IP-адресу прошло успешно!\n";
    }

    freeaddrinfo(result);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    iResult = listen(ListenSocket, SOMAXCONN); // The listen function places a socket in a state in which it is listening for an incoming connection: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed with error: " << WSAGetLastError() << "\n";
        cout << "прослушка информации от клиента не началась. что-то пошло не так!\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 5;
    }
    else {
        cout << "начинается прослушка информации от клиента. пожалуйста, запустите клиентское приложение! (client.exe)\n";
        // здесь можно было бы запустить некий прелоадер в отдельном потоке
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Accept a client socket
    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL); // The accept function permits an incoming connection attempt on a socket: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept failed with error: " << WSAGetLastError() << "\n";
        cout << "соединение с клиентским приложением не установлено! печаль!\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 6;
    }
    else {
        cout << "соединение с клиентским приложением установлено успешно!\n";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    while(true){

        char message[DEFAULT_BUFLEN];

        iResult = recv(ClientSocket, message, DEFAULT_BUFLEN, 0); // The recv function is used to read incoming data: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-recv
        if (iResult > 0) {
            message[iResult] = '\0';
            cout << "процесс клиента прислал сообщение: " << message << "\n";
            cout << "байтов получено: " << iResult << "\n";

            bool isFound = false;

            for (auto it = question_answer_map.begin(); it != question_answer_map.end(); ++it) {
                if (strcmp(message, it->first) == 0) { 
                    isFound = true;
                    cout << it->second << endl;
                    int iSendResult = send(ClientSocket, it->second, strlen(it->second), 0);

                    if (iSendResult == SOCKET_ERROR) {
                        cout << "send failed with error: " << WSAGetLastError() << "\n";
                        cout << "упс, отправка (send) ответного сообщения не состоялась ((\n";
                        closesocket(ClientSocket);
                        WSACleanup();
                        return 7;
                    }
                    else {
                        cout << "байтов отправлено: " << iSendResult << "\n";
                    }
                }
                if (isFound)
                    break;
            }

        }
        else if (iResult == 0) {
            cout << "соединение закрывается...\n";
        }
        else {
            cout << "recv failed with error: " << WSAGetLastError() << "\n";
            cout << "упс, получение (recv) ответного сообщения не состоялось ((\n";
            closesocket(ClientSocket);
            WSACleanup();
            return 8;
        }

        }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

