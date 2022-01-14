#include <iostream>
#include <cstdlib>
#include <chrono>
#include <cstring>

// const int VERTEX_COUNT = 5000;
// const int ITERATION_COUNT = 300;
const double DAMPING = 0.85;

// Matrix for link weights: M[from][to]
double ** M;

// Double-buffered vector for PageRank
// Current PR (in time t) is always PR_1
// PR_2 is used to hold previous values (from time t-1)
double * PR_1;
double * PR_2;

template<typename T> void printVector(T *, int);
template<typename T> void printMatrix(T **, int, int);

int main(int argc, char * argv[]) {
    std::srand(1);

    const int VERTEX_COUNT = std::atoi(argv[1]);
    const int ITERATION_COUNT = std::atoi(argv[2]);
    bool log = false;

    if (argc > 3 && strcmp(argv[3], "log") == 0) {
        log = true;
    }

    // Init matrix M
    M = new double*[VERTEX_COUNT];
    for (int i = 0; i < VERTEX_COUNT; i++) {
        M[i] = new double[VERTEX_COUNT];

        for (int j = 0; j < VERTEX_COUNT; j++) {
            M[i][j] = 0;
        }
    }

    // Init PR
    PR_1 = new double[VERTEX_COUNT];
    PR_2 = new double[VERTEX_COUNT];

    // Init links
    for (int i = 0; i < VERTEX_COUNT; i++) {
        // At least one outgoing link in each vertex to prevent sinks
        int linkCount = std::rand() % (VERTEX_COUNT / 2) + 1;

        for (int link = 0; link < linkCount; link++) {
            // Initialized to i to jump into the while loop
            int targetIndex = i;

            // No double links or self links
            while (targetIndex == i || M[targetIndex][i] != 0) {
                targetIndex = rand() % VERTEX_COUNT;
            }

            // M[targetIndex][i] = (double)1 / linkCount;
            M[targetIndex][i] = (double)1 / linkCount;
        }
    }

    // ? LOG
    if (log) {
        std::cout << "Matrix M: " << std::endl;
        printMatrix<double>(M, VERTEX_COUNT, VERTEX_COUNT);
    }

    // Init page ranks
    for (int i = 0; i < VERTEX_COUNT; i++) {
        PR_1[i] = (double)1 / VERTEX_COUNT;
        PR_2[i] = (double)1 / VERTEX_COUNT;
    }

    // ? LOG
    if (log) {
        std::cout << "PR_1: " << std::endl;
        printVector<double>(PR_1, VERTEX_COUNT);
    }

    //! Initialization done!

    // Timer
    auto start = std::chrono::high_resolution_clock::now();

    // Iterate
    #pragma acc data copyin(M[:VERTEX_COUNT][:VERTEX_COUNT]) copy(PR_1[:VERTEX_COUNT]) copy(PR_2[:VERTEX_COUNT])
    for (int iter = 0; iter < ITERATION_COUNT; iter++) {
        #pragma acc parallel loop
        for (int vertex = 0; vertex < VERTEX_COUNT; vertex++) {
            double mult = 0;

            for (int i = 0; i < VERTEX_COUNT; i++) {
                mult += M[vertex][i] * PR_2[i];
            }

            PR_1[vertex] = (DAMPING * mult) + ((1 - DAMPING) / VERTEX_COUNT);
        }

        // Commit changes from PR_1 to PR_2
        #pragma acc parallel loop
        for (int i = 0; i < VERTEX_COUNT; i++) {
            PR_2[i] = PR_1[i];
        }

        //? LOG (disable for performance)
        // printVector(PR_1, VERTEX_COUNT);
        // double sum = 0;
        // for (int i = 0; i < VERTEX_COUNT; i++) {
        //     sum += PR_1[i];
        // }
        // std::cout << "Sum: " << sum << std::endl;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    // ? LOG
    if (log) {
        std::cout << "DONE! Result: " << std::endl;
        printVector(PR_1, VERTEX_COUNT);
        double sum = 0;
        for (int i = 0; i < VERTEX_COUNT; i++) {
            sum += PR_1[i];
        }
        std::cout << "Sum: " << sum << std::endl;
    }

    // Log time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time: " << duration.count() << std::endl;
}

template<typename T>
void printMatrix(T ** matrix, int sizeX, int sizeY) {
    for (int y = 0; y < sizeY; y++) {
        printVector(matrix[y], sizeX);
    }
}

template<typename T>
void printVector(T * vector, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << vector[i] << ", ";    
    }
    std::cout << std::endl;
}
