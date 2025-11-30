#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

using namespace std;

const int M = 500; 
const int K = 500;
const int P = 500;

void initialize_matrix(vector<vector<int>>& mat, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat[i][j] = rand() % 10;
        }
    }
}

int main() {
    // 1. Setup
    vector<vector<int>> A(M, vector<int>(K));
    vector<vector<int>> B(K, vector<int>(P));
    vector<vector<int>> C(M, vector<int>(P, 0));

    srand(time(0));
    initialize_matrix(A, M, K);
    initialize_matrix(B, K, P);

    cout << "Running Sequential MM (" << M << "x" << K << " * " << K << "x" << P << ")" << endl;

    // 2. Start Timing
    auto start = chrono::high_resolution_clock::now();

    // 3. Multiplication Kernel: C[i][j] = sum(A[i][k] * B[k][j])
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < P; ++j) {
            for (int k = 0; k < K; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // 4. Stop Timing and Output
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Execution Time: " << duration.count() << " seconds" << endl;
    return 0;
}
