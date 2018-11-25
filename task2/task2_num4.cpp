#include <iostream>
#include "mpi.h"
#include "math.h"

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Вычислить значения суммы ряда в n точках заданного интервала [A, B] с точностью eps.
 * Процесс-мастер вводит с клавиатуры A, B, eps и n, вычисляет аргументы
 * x1 = A, ... xn = B и рассылает каждому процессу n/k (k - количество запущенных процессов)
 * значений с помощью функции MPI_Scatter и eps с помощью функции MPI_Bcast.
 * Каждый процесс вычисляет значения функции в полученных точках и отправляет
 * процессу-мастеру с помощью функции MPI_Gather.
 * Процесс-мастер выводит полученные результаты и точные значения функции
 * в соответствующих точках в виде таблицы.
 *
 * arctg(x) = x - x^3/3 + x^5/5 - x^7/7 + x^9/9...
 */

const int MASTER_ID = 0; // Нулевой процесс

const char *INPUT_VALUES_STRING_FORMAT = "%lf%lf%lf%d";
const char *PRINT_INPUT_VALUES_STRING_FORMAT = "%.2f %.2f %.2f %d \n";
const char *RESULT_DOUBLE_VALUE_FORMAT = "%-15.3f ";

double f(double, double);
double fExact(double x);

double *createDoubleArray(int size);
double *calculateSeriesValues(int count, double *values, double eps);

template<typename ...Doubles>
void print(Doubles... values)
{
    double args[] = { values... };
    for (int i = 0; i < sizeof(args) / sizeof(double); i++) {
        printf(RESULT_DOUBLE_VALUE_FORMAT, args[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {

    int rank, processCount;
    int count; // Количество точек, которые должен обработать каждый процесс

    double
            eps,           // Точность
            *sendbufdots,  // Все точки из интервала [A,B] с шагом (B - A) / (n-1)
            *recbufdots,   // Точки, принятые процессом для вычисления суммы ряда
            *sendbuf,      // Вычисленная сумма ряда в этих точках
            *recvbuf;      // Вычисленная сумма ряда во всех точках

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    if (rank == MASTER_ID) {

        // Ввод данных
        printf("Please, Input A, B, eps, n\n");

        double A, B;
        int n;
        scanf(INPUT_VALUES_STRING_FORMAT, &A, &B, &eps, &n);
        printf(PRINT_INPUT_VALUES_STRING_FORMAT, A, B, eps, n);

        // Вычисление точек
        sendbufdots = createDoubleArray(n);
        recbufdots = createDoubleArray(n);
        double step = (B - A) / (n - 1);
        sendbufdots[0] = A;
        for (int i = 1; i < n; i++)
            sendbufdots[i] = (A += step);

        count = n / processCount;
        int recvCount = count * processCount;
        recvbuf = createDoubleArray(count * processCount);

        // Отправка всем процессам количества значений, которые они должны будут принять и обработать
        MPI_Bcast(&count, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);

        // Отправка eps
        MPI_Bcast(&eps, 1, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);

        // Отправка значений точек
        MPI_Scatter(sendbufdots, count, MPI_DOUBLE, recbufdots, count, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);

        // Вычисление значений
        sendbuf = calculateSeriesValues(count, recbufdots, eps);

        // Отправляем значения и одновременно получаем их
        MPI_Gather(sendbuf, count, MPI_DOUBLE, recvbuf, count, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);

        // Печать результатов
        printf("Dot\t\tExact value\tFound value\n");
        for (int i = 0; i < recvCount; i++) {
            double currentValue =  sendbufdots[i];
            print(currentValue, fExact(currentValue), recvbuf[i]);
        }

        // Количество точек не всегда кратно количеству процессов,
        // поэтому некоторые процессы будут выполнять большее количество вычислений.
        // Процесс-мастер будет выполнять оставшиеся вычисления
        for (int i = recvCount; i < n; i++) {
            double currentValue =  sendbufdots[i];
            print(currentValue, fExact(currentValue), f(currentValue, eps));
        }
    } else {
        // Получение количества значений, которые процесс должен будет обработать
        MPI_Bcast(&count, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);

        // Получение eps
        MPI_Bcast(&eps, 1, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);

        sendbufdots = createDoubleArray(count);
        recbufdots = createDoubleArray(count);
        recvbuf = createDoubleArray(count);

        // Получение значений точек значения
        MPI_Scatter(sendbufdots, count, MPI_DOUBLE, recbufdots, count, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);

        // Вычисление значений
        sendbuf = calculateSeriesValues(count, recbufdots, eps);

        // Отправка значений
        MPI_Gather(sendbuf, count, MPI_DOUBLE, recvbuf, count, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

//region Function for creations of arrays
/**
 * Функция, создающая массив вещественных чисел заданного размера
 * @param size размер созданного массива
 * @return массив вещественных чисел
 */
double *createDoubleArray(int size) {
    return (double *) malloc(size * sizeof(double));
}

/**
 * Функция, возвращающая массив значений функции в заданных точках с заданной точностью
 * @param count количество элементов полученного массива
 * @param values значений точек, в которых будет вычислено значений функции
 * @param eps точность вычислений
 * @return массив значений функции в заданных точках с заданной точностью
 */
double *calculateSeriesValues(int count, double *values, double eps) {
    double  *result = createDoubleArray(count);
    for (int i = 0; i < count; i++)
        result[i] = f(values[i], eps);
    return result;
}
//endregion

//region Functions for task solution
/**
 * Функция, вычисляющая значение функции в точке с заданной точностью
 * @param x точка, в которой будет вычислено значение функции
 * @param eps точность вычислений
 * @return значение функции в точке с заданной точностью
 */
double f(double x, double eps) {
    //todo
    double
            powX = -x * x,
            pred = x,
            result = pred - x * x / 2;

    for (int pos = 3; fabs(pred - result) > eps; pos++) {
        powX *= -x;
        pred = result;
        result += powX / pos;
    }
    return result;
}

/**
 * Функция, возвращающая точное значение функции в заданной точке
 * @param x точка, в которой будет вычислено значение функции
 * @return точное значение функции в точке
 */
double fExact(double x) {
    return atan(x);
}
//endregion