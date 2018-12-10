#include <iostream>
#include "mpi.h"

using namespace std;

#define dimension 1

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Даны две матрицы размера n * n (n - количество запущенных процессов).
 * Матрицы распределены между процессами. Каждый процесс генерирует строку матрицы А
 * и строку (или столбец) матрицы В. Память отводится только для строки А и строки (столбца) В.
 * Для вычисления по формуле процесс использует свою строку матрицы А, а строки (столбцы) матрицы В
 * передаются последовательно каждому процессу, при этом используется виртуальная топология "кольцо".
 * Результат - каждый процесс вычисляет одно значение и либо выводит его сам,
 * либо передает процессу-мастеру, который выводит все.
 *
 * res(i) = max(j) sum(k) ( a(ik) + b(kj) )
 */

const int MASTER_ID = 0; // Нулевой процесс

int *createIntArray(int size);

int sum(int *a, int *b, int len);

int main(int argc, char *argv[]) {

    int processCount, rank, predRank, nextRank, max, current;
    int matrixSize = processCount * processCount;

    MPI_Comm newComm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    int dims[dimension], periods[dimension], newCoords[dimension];

    for (int i = 0; i < dimension; i++) {
        dims[i] = 0;
        periods[i] = 1;
    }

    MPI_Dims_create(processCount, dimension, dims);
    MPI_Cart_create(MPI_COMM_WORLD, dimension, dims, periods, 0, &newComm);
    MPI_Cart_coords(newComm, rank, dimension, newCoords);
    MPI_Cart_shift(newComm, 0, -1, &predRank, &nextRank);

    int *A = createIntArray(processCount), *B = createIntArray(processCount);

    srand(time(0) + rank * 10);
    for (int i = 0; i < processCount; i++) {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
    }

    //Нулевой процесс выводит сгенерированные матрицы
    int *recvbufA = createIntArray(matrixSize);
    int *recvbufB = createIntArray(matrixSize);

    MPI_Gather(A, processCount, MPI_INT, recvbufA, processCount, MPI_INT, MASTER_ID, newComm);
    MPI_Gather(B, processCount, MPI_INT, recvbufB, processCount, MPI_INT, MASTER_ID, newComm);

    if (rank == MASTER_ID) {
        cout << "A" << endl;
        for (int i = 0; i < processCount; i++) {
            int sumstr = 0;
            for (int j = 0; j < processCount; j++) {
                int currentElement = recvbufA[processCount * i + j];
                cout << currentElement << " ";
                sumstr += currentElement;
            }
            cout << "| sumstr = " << sumstr << endl;
        }
        cout << "\nB" << endl;
        for (int i = 0; i < processCount; i++) {
            int sumstr = 0;
            for (int j = 0; j < processCount; j++) {
                int currentElement = recvbufB[processCount * i + j];
                cout << currentElement << " ";
                sumstr += currentElement;
            }
            cout << "| sumstr = " << sumstr << endl;
        }
    }

    current = sum(A, B, processCount);
    max = current;
    //if (rank == MASTER_ID) {
        //cout << "\nResults:\n\tsum(A[" << rank << ",j] + B[0,j]) = " << current << endl;
    //}

    cout << "\nResults:\n";
    for (int k = 0; k < processCount; k++) {
        MPI_Status status;
        MPI_Sendrecv_replace(B, processCount, MPI_INT, nextRank, 1, predRank, 1, newComm, &status);
        current = sum(A, B, processCount);
        if (current > max) {
            max = current;
        }
        //if (rank == MASTER_ID) {
            cout << "\tsum(A[" << rank << ",j] + B[" << k << ",j]) = " << current << endl;
        //}
    }
    //if (rank == MASTER_ID) {
        cout << "Maximum = " << max << endl;
    //}

    free(A);
    free(B);
    MPI_Comm_free(&newComm);
    MPI_Finalize();
    return 0;
}

/**
 * Функция, создающая массив целых чисел заданного размера
 * @param size размер созданного массива
 * @return массив целых чисел
 */
int *createIntArray(int size) {
    return (int *) malloc(size * sizeof(int));
}

/**
 * Функция, вычисляющая сумму элементов двух массивов
 */
int sum(int *a, int *b, int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += a[i] + b[i];
    }
    return sum;
}
