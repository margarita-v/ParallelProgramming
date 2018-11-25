#include <iostream>
#include "mpi.h"

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

void send(int vector[], int rank);

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
        // Нулевой процесс получает значения поэлементных произведений двух векторов
        // и находит максимум из них
        int currentProcess = 1; // Номер процесса, от которого будет получено значение
        for (int j = 0; j < processCount / 2; j++) {
            for (int i = 0; i < N; i++) {
                MPI_Recv(&vector[i], 1, MPI_INT, currentProcess, i, MPI_COMM_WORLD, &status);
            }
            printf("\nДля процессв %d и %d:\n", currentProcess, currentProcess + 1);
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
            currentProcess += 2;  // Получаем значения только от нечетных процессов
        }
    } else {
        // Инициализация генератора случайных чисел
        srand(time(0) - rank * 2);

        // Формирование случайного вектора длины N
        printf("\nПроцесс %d: \t\t", rank);
        for (int i = 0; i < N; i++)
            printf("%d\t", vector[i] = rand() % N);
        printf("\n");

        if (rank % 2 != 0) {
            // Нечетные процессы получают значения вектора от четных процессов
            // и отправляют произведение нулевому процессу
            int receivedValue;
            for (int i = 0; i < N; i++) {
                MPI_Recv(&receivedValue, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD, &status);
                vector[i] *= receivedValue;
            }
            send(vector, 0);
        } else {
            // Четные процессы отправляют значения вектора парному процессу
            send(vector, rank - 1);
        }
    }

    MPI_Finalize();
    return 0;
}

/**
 * Функция, выполняющая отправку значений вектора процессу с указанным рангом
 * @param vector вектор, значения которого будут отправлены другому процессу
 * @param rank ранг процесса, которому будут отправлены значения вектора
 */
void send(int vector[], int rank) {
    for (int i = 0; i < N; i++) {
        MPI_Send(&vector[i], 1, MPI_INT, rank, i, MPI_COMM_WORLD);
    }
}