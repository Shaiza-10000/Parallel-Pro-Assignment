#include <iostream>
#include <vector>
#include <cstdlib>
#include <mpi.h> 

using namespace std;

const int M = 500; 
const int K = 500;
const int P = 500;

void initialize_matrix(int* mat, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat[i * cols + j] = rand() % 10;
        }
    }
}

int main(int argc, char* argv[]) {
    // 1. MPI Initialization
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (M % world_size != 0) {
        if (rank == 0) {
            cerr << "Error: Matrix A's rows (M=" << M << ") must be divisible by the number of processes (" << world_size << ")." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Rows each process will handle
    int rows_per_proc = M / world_size;
    
    // Chunk sizes for communication (in number of elements)
    int chunk_size_A = rows_per_proc * K; // Rows of A * K columns
    int chunk_size_C = rows_per_proc * P; // Rows of C * P columns
    
    // Allocate memory for matrices on the master process (rank 0)
    int* A = (rank == 0) ? new int[M * K] : nullptr;
    int* B = new int[K * P]; // B is needed fully by all processes
    int* C = (rank == 0) ? new int[M * P] : nullptr;

    // Local buffer for the chunk of A and C for each worker process
    int* local_A = new int[chunk_size_A];
    int* local_C = new int[chunk_size_C];

    double start_time = 0;

    if (rank == 0) {
        cout << "Running MPI MM (" << M << "x" << K << " * " << K << "x" << P << ", Processes: " << world_size << ")" << endl;
        
        srand(time(0));
        initialize_matrix(A, M, K);
        initialize_matrix(B, K, P);
        
        start_time = MPI_Wtime(); // MPI-specific timing function
    }

    // 2. Data Distribution
    // Broadcast B to all processes (B is needed completely by every process)
    MPI_Bcast(B, K * P, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter A: Send a portion of A's rows (local_A) to each process
    MPI_Scatter(A, chunk_size_A, MPI_INT, local_A, chunk_size_A, MPI_INT, 0, MPI_COMM_WORLD);

    // 3. Local Multiplication Kernel
    for (int i = 0; i < rows_per_proc; ++i) {
        for (int j = 0; j < P; ++j) {
            local_C[i * P + j] = 0; 
            for (int k = 0; k < K; ++k) {
                // local_A[i][k] * B[k][j]
                local_C[i * P + j] += local_A[i * K + k] * B[k * P + j];
            }
        }
    }

    // 4. Data Collection
    // Gather all local_C chunks back into the global C matrix on the master process (rank 0)
    MPI_Gather(local_C, chunk_size_C, MPI_INT, C, chunk_size_C, MPI_INT, 0, MPI_COMM_WORLD);

    // 5. Output Results (Master Process Only)
    if (rank == 0) {
        double end_time = MPI_Wtime();
        cout << "Execution Time: " << end_time - start_time << " seconds" << endl;
        delete[] A;
        delete[] C;
    }

    delete[] B;
    delete[] local_A;
    delete[] local_C;

    // 6. MPI Finalization
    MPI_Finalize();
    return 0;
}
