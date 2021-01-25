#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>

#define SAMPLES 10000000
#define THREADS 8

using namespace std;
using namespace std::chrono;

double square(double a){
    return a*a;
}

void trap(vector<double> &arr, double a, double b, vector<double> &area){
    double h, x, local_a, local_b, local_n, res;
    int t_rank = omp_get_thread_num();
    int t_count = omp_get_num_threads();
    h = (b-a)/SAMPLES;
    local_n = SAMPLES/t_count;
    local_a = t_rank*local_n;
    local_b = local_a + local_n;

    res = (arr[local_a] + arr[local_b])/2.0;
    for(int i=1; i<=local_n-1; i++) {
        x = local_a + i;
        res += arr[x];
    }
    area[t_rank] = res*h;
}

void computeArea(vector<double> &arr, double a, double b, double * area, double *time){
    double h = (b-a)/SAMPLES;
    auto start = high_resolution_clock::now();
    for(int i=0;i<SAMPLES-1;i++){
        *area += 0.5*h*(arr[i]+arr[i+1]);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    *time = duration.count();
}

void computeAreaParallel(vector<double> &arr, double a, double b, double * area, double *time){
    vector<double> accumulator(THREADS);
    auto start = high_resolution_clock::now();
    #pragma omp parallel num_threads(THREADS)
    trap(arr, a, b, accumulator);
    for(int i=0;i<THREADS;i++){
        *area+=accumulator[i];
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    *time = duration.count();
}

int main(){
    double a, b, h;
    cout<<"End points for computing area?"<<endl;
    cin>>a>>b;  
    h = (b-a)/SAMPLES;

    vector<int> x(SAMPLES, 0);
    for(int i=0;i<SAMPLES;i++){
        x[i] = i;   // generate x axis
    }

    vector<double> f(SAMPLES, 0);
    for(int i=0;i<SAMPLES;i++){
        f[i] = square(a+x[i]*h);    // generate parabola f(x)=x^2 between a and b
    }

    double area=0, area_p=0, time_s=0, time_p=0;
    computeArea(f, a, b, &area, &time_s);
    computeAreaParallel(f, a, b, &area_p, &time_p);

    cout<<"Area from computeArea():"<<area<<endl;
    cout<<"Area from computeAreaParallel():"<<area_p<<endl;
    cout<<"Time taken for serial execution:"<<time_s<<endl;
    cout<<"Time taken for parallel execution:"<<time_p<<endl;
    
    return 0;
}