#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <ctime>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

long powermod(long a, long b, long q)
{
    long res = 1;
    for (long i = 0; i < b; i++)
    {
        res = (res * a) % q;
    }
    return res;
}

int randInRange(int low, int high) // excluding high and low
{
    return rand() % (high - (low + 1)) + (low + 1);
}

int createServer(int port) // TCP connection
{
    WSADATA wsaData;
    SOCKET sersock = INVALID_SOCKET;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed." << endl;
        return 1;
    }

    sersock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sersock == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sersock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        cout << "bind() failed." << endl;
        closesocket(sersock);
        WSACleanup();
        return 1;
    }

    cout << "\nServer Online. Waiting for client...." << endl;

    if (listen(sersock, 5) == SOCKET_ERROR) {
        cout << "Error listening on socket." << endl;
        closesocket(sersock);
        WSACleanup();
        return 1;
    }

    sock = accept(sersock, NULL, NULL);
    if (sock == INVALID_SOCKET) {
        cout << "accept failed." << endl;
        closesocket(sersock);
        WSACleanup();
        return 1;
    }

    cout << "Connection Established." << endl;
    return sock;
}

int main() {
    int port = 4444;  // Fixed port number
    int sock = createServer(port);

    long q, alpha;
    cout << "\nEnter a prime number, q : "; cin >> q;
    cout << "Enter primitive root of q, alpha : "; cin >> alpha;

    long Yc;
    recv(sock, (char*)&Yc, sizeof(Yc), 0); // recv client's public key
    cout << "\nClient's public key,  Yc = " << Yc << endl;

    srand(time(NULL));
    long Xs = randInRange(1, q); // server's private key
    cout << "\nServer's private key, Xs = " << Xs << endl;

    long Ys = powermod(alpha, Xs, q); // server's public key
    send(sock, (char*)&Ys, sizeof(Ys), 0); // send server's public key
    cout << "Server's public key,  Ys = " << Ys << endl;

    long k = powermod(Yc, Xs, q);
    cout << "\nSecret Key, k = " << k << endl;

    long msg;
    cout << "\nEnter a message (number) to send : "; cin >> msg;

    long cipher = msg ^ k; // encryption
    send(sock, (char*)&cipher, sizeof(cipher), 0);
    cout << "Encrypted msg sent to client: " << cipher << endl << endl;

    closesocket(sock);
    WSACleanup();

    return 0;
}
