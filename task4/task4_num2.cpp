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

const string MATRIX_A = "A";
const string MATRIX_B = "B";

int sum(int *a, int *b, int len);

int *createIntArray(int size);

string arrayToString(int *array, int size, string name, int rank, bool isRow);

string sumToString(int rank, int index, int sum);

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

    string message = arrayToString(A, processCount, MATRIX_A, rank, true) +
                     arrayToString(B, processCount, MATRIX_B, rank, false);

    current = sum(A, B, processCount);
    max = current;
    message += "Results:\n" + sumToString(rank, 0, current);

    for (int k = 1; k < processCount; k++) {
        MPI_Status status;
        MPI_Sendrecv_replace(B, processCount, MPI_INT, nextRank, 1, predRank, 1, newComm, &status);
        current = sum(A, B, processCount);
        if (current > max) {
            max = current;
        }
        message += sumToString(rank, k, current);
    }
    message += "Maximum = ";
    cout << message << max << endl;

    free(A);
    free(B);
    MPI_Comm_free(&newComm);
    MPI_Finalize();
    return 0;
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

/**
 * Функция, создающая массив целых чисел заданного размера
 * @param size размер созданного массива
 * @return массив целых чисел
 */
int *createIntArray(int size) {
    return (int *) malloc(size * sizeof(int));
}

/**
 * Функция, возвращающая строковое представление массива с дополнительной информацией для печати
 * @param array массив, который будет напечатан
 * @param size размер массива
 * @param name имя массива
 * @param rank ранг текущего процесса
 * @param isRow флаг, значение которого равно true, если данный массив является строкой матрицей
 * @return строковое представление массива с дополнительной информацией для печати
 */
string arrayToString(int *array, int size, string name, int rank, bool isRow) {
    string result = isRow ? "row\t" : "column\t";
    result += name + "[" + to_string(rank) + "] = [ ";
    int sum = 0;
    for (int i = 0; i < size; i++) {
        int currentElement = array[i];
        result += to_string(currentElement) + " ";
        sum += currentElement;
    }
    string arrayType = isRow ? "sumrow" : "sumcol";
    return result += "], " + arrayType + " = " + to_string(sum) + "\n";
}

/**
 * Функция, возвращающая строку для печати суммы строки и столбца двух матриц
 * @param rank ранг текущего процесса
 * @param index индекс столбца матрицы, который получил текущий процесс
 * @param sum значений суммы
 * @return строка для печати суммы строки и столбца двух матриц
 */
string sumToString(int rank, int index, int sum) {
    return "\tsum(" + MATRIX_A + "[" + to_string(rank) + ",j] + " +
           MATRIX_B + "[j," + to_string(index) + "]) = " + to_string(sum) + "\n";
}