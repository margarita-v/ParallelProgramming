#include <iostream>
#include "mpi.h"

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

int main(int argc, char *argv[]) {

    int rank, processCount;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    if (rank == MASTER_ID) {
        //todo
    } else {
        //todo
    }

    MPI_Finalize();
    return 0;
}
