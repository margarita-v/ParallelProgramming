#include <iostream>
#include <mpi.h>

using namespace std;

#define N 7 // Размерность матрицы

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Создать описатель типа и использовать его при передаче данных
 * в качестве шаблона для следующего преобразования:
 *
 * первая строка, последний столбец, главная диагональ.
 *
 * A11 A12 ... A1n
 * a21 A22 ... A2n
 * ... ... ... ...
 * an1 an2 ... Ann
 */

const int MASTER_ID = 0; // Нулевой процесс
const int WORKER_ID = 1;

const string BEFORE_MATRIX_MESSAGE = "Matrix before:";
const string AFTER_MATRIX_MESSAGE = "Matrix after:";

void printMatrix(int matrix[N][N], string message);

int main(int argc, char *argv[]) {

    int rank, tag = MASTER_ID;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int blockSize = 2 * (N - 1);
    int blockLengthArray[blockSize];  // Массив, содержащий число элементов в каждом блоке
    int displacementArray[blockSize]; // Массив смещений каждого блока от начала размещения элемента нового типа

    // Инициализация размеров блоков
    blockLengthArray[0] = N;
    for (int i = 1; i < blockSize; i++) {
        blockLengthArray[i] = 1;
    }

    // Инициализация массива смещений
    displacementArray[0] = 0;
    for (int i = 2, row = 1; i < blockSize; i += 2, row++) {
        displacementArray[i - 1] = row * (N + 1);
        displacementArray[i] = N * (row + 1) - 1;
    }
    displacementArray[blockSize - 1] = N * N - 1;

    MPI_Datatype customMatrixType;
    MPI_Type_indexed(blockSize, blockLengthArray, displacementArray, MPI_INT, &customMatrixType);
    MPI_Type_commit(&customMatrixType);

    // Все элементы матрицы равны рангу текущего процесса
    int matrix[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rank;
        }
    }

    if (rank == MASTER_ID) {
        MPI_Send(&matrix[0][0], 1, customMatrixType, WORKER_ID, tag, MPI_COMM_WORLD);
    } else if (rank == WORKER_ID) {
        printMatrix(matrix, BEFORE_MATRIX_MESSAGE);
        MPI_Status status;
        MPI_Recv(&matrix[0][0], 1, customMatrixType, MASTER_ID, tag, MPI_COMM_WORLD, &status);
        printMatrix(matrix, AFTER_MATRIX_MESSAGE);
    }

    MPI_Type_free(&customMatrixType);
    MPI_Finalize();
    return 0;
}

/**
 * Функция для печати матрицы и ее описания
 */
void printMatrix(int matrix[N][N], string description) {
    cout << description << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << to_string(matrix[i][j]) << " ";
        }
        cout << endl;
    }
    cout << endl;
}