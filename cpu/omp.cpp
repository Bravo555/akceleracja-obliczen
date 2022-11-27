#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int longFunc(int i)
{
    int output = i + omp_get_thread_num();
    printf("output = %d + %d = %d;\n", i, omp_get_thread_num(), output);
    sleep(2);
    return output;
}

int main()
{
    int result = 0;
#pragma omp parallel
    {
#pragma omp single
        {
            printf("max thread number = %d;\n", omp_get_max_threads());
            printf("threads number = %d;\n", omp_get_num_threads());
            printf("procs number = %d;\n", omp_get_num_procs());
        }
#pragma omp for
        for (int i = 0; i < 100; i++)
        {
            int temp = longFunc(i);
#pragma omp atomic
            result += temp;
        }
#pragma omp single
        {
            printf("threads number = %d;\n", omp_get_num_threads());
        }
    }
    printf("result = %d;\n", result);
    return 0;
}