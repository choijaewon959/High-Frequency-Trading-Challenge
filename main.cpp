#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Profiler.h"
#include "Parser.h"

using namespace std;

#define PORT 12345
#define PROFILE_OUTPUT "profile.csv"
#define MODEL_VERSION "int_reader_string_buffer_v1"

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

    // Parser reader(sock);
    FastParser reader(sock);

    Profiler profiler(PROFILE_OUTPUT, MODEL_VERSION);

    while (true) {
        auto t0 = profiler.now();

        int challengeId;
        int N;

        if (!reader.readInt(challengeId)) break;
        if (!reader.readInt(N)) break;

        auto t1 = profiler.now();

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

        auto t2 = profiler.now();

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (!reader.readInt(B[i][j])) {
                    cerr << "Failed reading B" << endl;
                    close(sock);
                    return 1;
                }
            }
        }

        auto t3 = profiler.now();

        long long answer = 0;

        // Example: C[0][0] of A * B
        for (int k = 0; k < N; k++) {
            answer += 1LL * A[0][k] * B[k][0];
        }

        auto t4 = profiler.now();

        string answerStr = to_string(answer) + "\n";
        send(sock, answerStr.c_str(), answerStr.size(), 0);

        auto t5 = profiler.now();

        long long header_us = profiler.usBetween(t0, t1);
        long long read_A_us = profiler.usBetween(t1, t2);
        long long read_B_us = profiler.usBetween(t2, t3);
        long long compute_us = profiler.usBetween(t3, t4);
        long long send_us = profiler.usBetween(t4, t5);
        long long total_us = profiler.usBetween(t0, t5);

        profiler.log(
            challengeId,
            N,
            header_us,
            read_A_us,
            read_B_us,
            compute_us,
            send_us,
            total_us,
            answer
        );

        cout << "Sent answer: " << answer << endl;
    }

    close(sock);
    return 0;
}