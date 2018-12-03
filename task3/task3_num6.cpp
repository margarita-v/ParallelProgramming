#include <mpi.h>
#include <string>
#include <iostream>

using namespace std;

#define m 7
#define n 7

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Создать описатель типа и использовать его при передаче данных
 * в качестве шаблона для следующего преобразования:
 *
 * первая строка, последний столбец, левая диагональ.
 *
 * A11 A12 ... A1n
 * a21 A22 ... A2n
 * ... ... ... ...
 * am1 am2 ... Amn
 */

const int MASTER_ID = 0; // Нулевой процесс
const int WORKER_ID = 1;

const string BEFORE_MATRIX_MESSAGE = "Matrix before:";
const string AFTER_MATRIX_MESSAGE = "Matrix after:";

void printMatrix(int (*arr)[n], string message);

int main(int argc, char *argv[]) {

    int rank, tag = MASTER_ID;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int blocklength[m - 1];
    int displacement[m - 1];

    blocklength[0] = n + 2;
    for (int i = 1; i < m - 2; i++) {
        blocklength[i] = 3;
    }
    blocklength[m - 2] = n + 1;

    displacement[0] = 0;
    for (int i = 1; i < m - 1; i++) {
        displacement[i] = (i + 1) * n - 1;
    }

    MPI_Datatype customMatrixType;
    MPI_Type_indexed(m - 1, blocklength, displacement, MPI_INT, &customMatrixType);
    MPI_Type_commit(&customMatrixType);

    int matrix[m][n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
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
void printMatrix(int matrix[m][n], string description) {
    cout << description << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << to_string(matrix[i][j]) << " ";
        }
        cout << endl;
    }
    cout << endl;
}