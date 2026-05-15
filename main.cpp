#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <cctype>

using namespace std;

#define PORT 12345
#define BUFFER_SIZE 4096

class IntReader {
private:
    int sock;
    string buf;

public:
    IntReader(int s) : sock(s) {}

    bool readInt(int& x) {
        while (true) {
            // Remove leading whitespace
            size_t i = 0;
            while (i < buf.size() && isspace((unsigned char)buf[i])) {
                i++;
            }
            if (i > 0) {
                buf.erase(0, i);
            }

            // Find end of integer token
            size_t j = 0;
            while (j < buf.size() && !isspace((unsigned char)buf[j])) {
                j++;
            }

            // We have a complete token only if it ends with whitespace
            if (j < buf.size()) {
                string token = buf.substr(0, j);
                buf.erase(0, j);

                if (!token.empty()) {
                    x = stoi(token);
                    return true;
                }
            }

            // Need more bytes
            char temp[BUFFER_SIZE];
            int bytes = recv(sock, temp, BUFFER_SIZE, 0);

            if (bytes <= 0) {
                return false;
            }

            buf.append(temp, bytes);
        }
    }
};

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket failed");
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect failed");
        close(sock);
        return 1;
    }

    cout << "Connected to server!" << endl;

    string name = "Team_MFJ";
    send(sock, name.c_str(), name.size(), 0);

    IntReader reader(sock);

    while (true) {
        int challengeId;
        int N;

        if (!reader.readInt(challengeId)) break;
        if (!reader.readInt(N)) break;

        if (N != 128) {
            cerr << "Bad N received: " << N << ". Stream got corrupted." << endl;
            break;
        }

        cout << "Received challenge " << challengeId << " with N = " << N << endl;

        vector<vector<int>> A(N, vector<int>(N));
        vector<vector<int>> B(N, vector<int>(N));

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (!reader.readInt(A[i][j])) {
                    cerr << "Failed reading A" << endl;
                    close(sock);
                    return 1;
                }
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (!reader.readInt(B[i][j])) {
                    cerr << "Failed reading B" << endl;
                    close(sock);
                    return 1;
                }
            }
        }

        long long answer = 0;

        // Example: C[0][0] of A * B
        for (int k = 0; k < N; k++) {
            answer += 1LL * A[0][k] * B[k][0];
        }

        string answerStr = to_string(answer) + "\n";
        send(sock, answerStr.c_str(), answerStr.size(), 0);

        cout << "Sent answer: " << answer << endl;
    }

    close(sock);
    return 0;
}