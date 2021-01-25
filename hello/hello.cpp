#include <iostream>
#include <omp.h>

int main(){
    #pragma omp parallel num_threads(4)
    {
        int tid = omp_get_thread_num();
        printf("Hello World from thread %d\n", tid);
    }
    return 0;
}