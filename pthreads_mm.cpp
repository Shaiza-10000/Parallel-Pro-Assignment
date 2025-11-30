#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <pthread.h> 
#include <cmath> 

using namespace std;

const int M = 500; 
const int K = 500;
const int P = 500;

// Global matrices (since threads run in the same process and share memory)
vector<vector<int>> A(M, vector<int>(K));
vector<vector<int>> B(K, vector<int>(P));
vector<vector<int>> C(M, vector<int>(P, 0));

// Structure to pass data to each thread
struct ThreadData {
    int thread_id;
    int start_row;
    int end_row;
};

// Thread function: performs matrix multiplication for a range of rows
void* multiply_rows(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // Each thread computes rows from start_row to end_row
    for (int i = data->start_row; i < data->end_row; ++i) {
        for (int j = 0; j < P; ++j) {
            for (int k = 0; k < K; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

void initialize_matrix(vector<vector<int>>& mat, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat[i][j] = rand() % 10;
        }
    }
}

int main(int argc, char* argv[]) {
    // Get number of threads from command line argument
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <NUM_THREADS>" << endl;
        return 1;
    }
    int NUM_THREADS = stoi(argv[1]);
    if (NUM_THREADS > M) NUM_THREADS = M; // Cannot have more threads than rows of A/C

    cout << "Running Pthreads MM (" << M << "x" << K << " * " << K << "x" << P << ", Threads: " << NUM_THREADS << ")" << endl;

    // 1. Setup
    srand(time(0));
    initialize_matrix(A, M, K);
    initialize_matrix(B, K, P);

    pthread_t threads[NUM_THREADS];
    ThreadData td[NUM_THREADS];
    
    // Calculate row distribution (handle remainders)
    int rows_per_thread = M / NUM_THREADS;
    int remainder = M % NUM_THREADS;
    int start_row = 0;

    // 2. Start Timing
    auto start = chrono::high_resolution_clock::now();

    // 3. Create Threads and Distribute Work
    for (int i = 0; i < NUM_THREADS; ++i) {
        int end_row = start_row + rows_per_thread + (i < remainder ? 1 : 0);
        
        td[i].thread_id = i;
        td[i].start_row = start_row;
        td[i].end_row = end_row;

        pthread_create(&threads[i], NULL, multiply_rows, (void*)&td[i]);
        
        start_row = end_row;
    }

    // 4. Join Threads (wait for all threads to finish)
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // 5. Stop Timing and Output
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Execution Time: " << duration.count() << " seconds" << endl;

    return 0;
}
