

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

int connectToServer(int port)
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed." << endl;
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    // Convert "127.0.0.1" to a wide string
    wchar_t ipAddr[] = L"127.0.0.1";
    InetPtonW(AF_INET, ipAddr, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "\nRun server program first." << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    else {
        cout << "\nClient is connected to Server." << endl;
    }
    return sock;
}

int main() {
    int port = 4444;  // Fixed port number
    int sock = connectToServer(port);

    long q, alpha;
    cout << "\nEnter a prime number, q : "; cin >> q;
    cout << "Enter primitive root of q, alpha : "; cin >> alpha;

    srand(static_cast<unsigned int>(time(NULL)));
    long Xc = randInRange(1, q); // client's private key (1<Xa<q)
    cout << "\nClient's private key, Xc = " << Xc << endl;

    long Yc = powermod(alpha, Xc, q); // client's public key
    send(sock, (char*)&Yc, sizeof(Yc), 0); // send client's public key
    cout << "Client's public key,  Yc = " << Yc << endl;

    long Ys;
    recv(sock, (char*)&Ys, sizeof(Ys), 0); // recv server's public key
    cout << "\nServer's public key,  Ys = " << Ys << endl;

    long k = powermod(Ys, Xc, q);
    cout << "\nSecret Key, k = " << k << endl;

    long cipher;
    recv(sock, (char*)&cipher, sizeof(cipher), 0);
    cout << "\nMessage received from Server  : " << cipher << endl;

    long decipher = cipher ^ k; // decryption
    cout << "Decrypted message : " << decipher << endl << endl;

    closesocket(sock);
    WSACleanup();

    return 0;
}
