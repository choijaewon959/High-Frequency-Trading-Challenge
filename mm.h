//
// Created by Mohammed RHAZI on 16/05/2026.
//

#ifndef HIGH_FREQUENCY_TRADING_CHALLENGE_MM_H
#define HIGH_FREQUENCY_TRADING_CHALLENGE_MM_H
#include <vector>
using namespace std;

constexpr int N = 128;
constexpr int BS = 32;
constexpr int S = N*N;

class mm {
    vector<int> Aflat;
    vector<int> Bflat;
    vector<long long> Cflat;

public:
    mm() : Aflat(N*N), Bflat(N*N), Cflat(N*N, 0) {}

    void flatten(const vector<vector<int>>& A, const vector<vector<int>>& B) {
        for (int i=0; i<N; ++i) {
            for (int j=0; j<N; ++j) {
                Aflat[i*N + j] = A[i][j];
                Bflat[i*N + j] = B[i][j];
            }
        }
    }
    void matmul_128_blocked() {
        fill(Cflat.begin(), Cflat.end(), 0);

        const int* A = &Aflat[0];
        const int* B = &Bflat[0];
        long long* C = &Cflat[0];

        for (int ii=0; ii<N; ii+=BS) {
            for (int kk=0; kk<N; kk+=BS) {
                for (int jj=0; jj<N; jj+=BS) {

                    for (int i = ii; i<ii+BS; ++i) {
                        for (int k=kk ; k<kk+BS; ++k) {
                            double a = A[i*N + k];

                            for (int j = jj; j<jj+BS; ++j) {
                                C[i*N + j] += 1LL * a*B[k*N + j];
                            }
                        }
                    }
                }
            }
        }
    }
    const vector<long long>& result() const {
        return Cflat;
    }
    long long traceAB_flat() {
        long long ans=0;
        for (int i=0; i<N; ++i) {
            int row = i*N;

            for (int k=0; k<N; ++k) {
                ans+= 1LL * Aflat[row+k] * Bflat[k*N+i];
            }
        }
        return ans;
    }
};

#endif //HIGH_FREQUENCY_TRADING_CHALLENGE_MM_H