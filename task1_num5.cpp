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

    int rank, processCount;
    int vector[N];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    if (processCount < MIN_COUNT_OF_PROCESSES || processCount % 2 == 0) {
        printf("Количество процессов должно быть нечетным и больше либо равно %d.\n", MIN_COUNT_OF_PROCESSES);
        MPI_Abort(MPI_COMM_WORLD, ERROR_CODE);
    }

    if (rank == MASTER_ID) {
        for (int i = 0; i < N; i++) {
            MPI_Recv(&vector[i], 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
        }
        printf("\nВектор произведений: \t");
        int max = vector[0];
        for (int i = 0; i < N; i++) {
            printf("%d\t", vector[i]);
            if (vector[i] > max) {
                max = vector[i];
            }
        }
        printf("\n\n");
        printf("Максимум: \t\t%d\n", max);
    } else {
        // Инициализация генератора случайных чисел
        srand(time(NULL) - rank * 2);

        // Формирование случайного вектора длины N
        printf("\nПроцесс %d: \t\t", rank);
        for (int i = 0; i < N; i++)
            printf("%d\t", vector[i] = rand() % N);
        printf("\n");

        if (rank % 2 != 0) {
            int receivedValue; // Полученное значение другого вектора
            for (int i = 0; i < N; i++) {
                MPI_Recv(&receivedValue, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD, &status);
                vector[i] *= receivedValue;
            }
            for (int i = 0; i < N; i++) {
                MPI_Send(&vector[i], 1, MPI_INT, 0, i, MPI_COMM_WORLD);
            }
        } else {
            for (int i = 0; i < N; i++) {
                MPI_Send(&vector[i], 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
    return 0;
}