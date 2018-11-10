#include <iostream>
#include "mpi.h"
#include "ctime"

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Два вектора А и В размерности N распределены между процессами:
 * значения векторов процессов с четными номерами составляют вектор А,
 * значения векторов процессов с нечетными номерами составляют вектор В.
 * Вычислить max(A[i] * B[i]), i = 0..N-1
 * Два процесса, содержащие соответствующие компоненты векторов,
 * обмениваются сообщениями, один из них вычисляет локальный максимум
 * и отправляет нулевому процессу, который получает и выводит результат.
 */

const int N = 10; // Размер вектора

const int MASTER_ID = 0; // Нулевой процесс

const int MIN_COUNT_OF_PROCESSES = 3;
const int ERROR_CODE = -1;

int main(int argc, char *argv[]) {

    // Ранг и количество процессов
    int rank, processCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    if (processCount < MIN_COUNT_OF_PROCESSES) {
        printf("Количество процессов должно быть не менее %d.\n", MIN_COUNT_OF_PROCESSES);
        MPI_Abort(MPI_COMM_WORLD, ERROR_CODE);
    }

    // Инициализация генератора случайных чисел
    srand(time(NULL) - rank * 2);

    // Формирование случайного вектора длины N
    int vector[N];
    printf("\nProcess %d : \t\t", rank);
    for (int i = 0; i < N; i++)
        printf("%d ", vector[i] = rand() % N);
    printf("\n");

    if (rank == MASTER_ID) {
        //todo receive and print result
    } else if (rank % 2 != 0) {
        //todo receive value from another process
        //todo find max and send to MASTER
    } else {
        //todo send value to (rank-1) process
    }
    MPI_Finalize();
    return 0;
}