#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
// #include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
using namespace std;
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define PAUSE 1000


int main(int argc, char** argv) // имя сервера при желании можно будет указать через параметры командной строки
{

    setlocale(0, "");
    system("title CLIENT SIDE");
    cout << "процесс клиента запущен!\n";

    
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: "<<iResult << "\n";
        return 11;
    }
    else {
        cout << "подключение Winsock.dll прошло успешно!\n";
    }


    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const char* ip = "localhost";

    addrinfo* result = NULL;
    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);

    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 12;
    }
    else {
        cout << "получение адреса и порта клиента прошло успешно!\n";
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) { // серверов может быть несколько, поэтому не помешает цикл

        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            cout << "socket failed with error: " << WSAGetLastError() << "\n";
            WSACleanup();
            return 13;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        cout << "создание сокета на клиенте прошло успешно!\n";

        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "невозможно подключиться к серверу. убедитесь, что процесс сервера запущен!\n";
        WSACleanup();
        return 14;
    }
    else {
        cout << "подключение к серверу прошло успешно!\n";
    }

    vector<const char*> client_questions;

    client_questions.push_back("Whats the weather like today?");
    client_questions.push_back("How much does delivery to my house cost?");
    client_questions.push_back("What is the warranty period for this item?");
    client_questions.push_back("What promotions are currently active?");
    client_questions.push_back("Can I order takeout?");
    client_questions.push_back("Where is the nearest branch of your company located?");
    client_questions.push_back("What payment methods do you accept?");
    client_questions.push_back("How long will the delivery take?");
    client_questions.push_back("Do you have loyalty programs for regular customers?");
    client_questions.push_back("What services do you offer for new customers?");

    cout << "Вопросы клиента:" << endl;
    for (const auto& question : client_questions) {
        cout << question << endl;
    }
    while (true) {
        char message[DEFAULT_BUFLEN];
        cout << "Введите ваш вопрос ('end' для завершения): ";
        cin.getline(message, DEFAULT_BUFLEN);

        if (strcmp(message, "end") == 0) {
            cout << "Завершение работы по запросу пользователя.\n";
            break;
        }

        iResult = send(ConnectSocket, message, (int)strlen(message), 0);
        if (iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return 15;
        }

        char answer[DEFAULT_BUFLEN];

        iResult = recv(ConnectSocket, answer, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            answer[iResult] = '\0';
            cout << "Ответ от сервера: " << answer << "\n";
            cout << "байтов получено: " << iResult << "\n";
        }
        else if (iResult == 0)
            cout << "соединение с сервером закрыто.\n";
        else
            cout << "recv failed with error: " << WSAGetLastError() << "\n";

    }



    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    cout << "процесс клиента прекращает свою работу!\n";

    return 0;
}