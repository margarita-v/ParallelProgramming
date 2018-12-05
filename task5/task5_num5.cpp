#include <iostream>
#include "mpi.h"

using namespace std;

#define N 10 // Размер массива

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Каждый процесс заполняет свой массив размера n случайными числами.
 * Для решения задачи использовать операции приведения с собственной функцией для решения задачи.
 * Результат - вектор размера n, каждый элемент которого получен по следующему правилу:
 *
 * Найти произведение положительных.
 */

void findPositiveMult(int *in, int *max, int *len, MPI_Datatype *datatype);

int main(int argc, char *argv[]) {

    int rank, processCount;
    int *arr = new int[N], *result = new int[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Генерация и печать массива для текущего процесса
    srand(time(0) + rank * 10);
    cout << "Process " << rank << ":\t";
    for (int i = 0; i < N; i++) {
        arr[i] = -N / 2 + rand() % (2 * N);
        cout << arr[i] << "\t";
    }
    cout << endl;

    MPI_Op operation;
    MPI_Op_create((MPI_User_function *) findPositiveMult, 0, &operation);
    MPI_Reduce(arr, result, N, MPI_INT, operation, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Result:\t\t";
        for (int i = 0; i < N; i++)
            cout << result[i] << "\t";
        cout << endl;
    }

    MPI_Op_free(&operation);
    MPI_Finalize();
    return 0;
}

/**
 * Функция, высчитывающая произведение положительных элементов переданных массивов.
 * @param in входные данные
 * @param positiveMult результат функции
 * @param len длина массивов
 * @param datatype параметр, необходимый для соответствия функции сигнатуре MPI_User_function
 */
void findPositiveMult(int *in, int *positiveMult, int *len, MPI_Datatype *datatype) {
    for (int i = 0; i < *len; i++) {
        if (in[i] > 0 && positiveMult[i] > 0) {
            positiveMult[i] *= in[i];
        } else {
            positiveMult[i] = 0;
        }
    }
}