# Compiler configuration
CXX = g++
MPICXX = mpic++
CXXFLAGS = -Wall -std=c++11 -O3

# Executables for Matrix Multiplication (MM)
MM_EXES = sequential_mm pthreads_mm openmp_mm mpi_mm

# Executables for Bubble Sort (Sort)
SORT_EXES = sequential_sort pthreads_sort openmp_sort mpi_sort

# Main target to build all 8 programs
all: $(MM_EXES) $(SORT_EXES)

# === Matrix Multiplication Targets ===
sequential_mm: sequential_mm.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

pthreads_mm: pthreads_mm.cpp
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

openmp_mm: openmp_mm.cpp
	$(CXX) $(CXXFLAGS) -fopenmp $< -o $@

mpi_mm: mpi_mm.cpp
	$(MPICXX) $(CXXFLAGS) $< -o $@

# === Bubble Sort Targets ===

# Sequential Sort
sequential_sort: sequential_sort.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Pthreads Sort: Uses 4 threads to sort sub-arrays
pthreads_sort: pthreads_sort.cpp
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

# OpenMP Sort: Attempts parallelization
openmp_sort: openmp_sort.cpp
	$(CXX) $(CXXFLAGS) -fopenmp $< -o $@

# MPI Sort: Uses scatter/gather to distribute sorting
mpi_sort: mpi_sort.cpp
	$(MPICXX) $(CXXFLAGS) $< -o $@

# Clean up compiled executables
clean:
	rm -f $(MM_EXES) $(SORT_EXES)
