#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include "../OpenBLAS/cblas.h"

using namespace std;

template <typename T>
void my_gemm(int M, int N, int K, T alpha, const T* A, const T* B, T beta, T* C) {
    #pragma omp parallel for
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i * N + j] *= beta;
        }
        for (int k = 0; k < K; ++k) {
            T temp_A = alpha * A[i * K + k];
            for (int j = 0; j < N; ++j) {
                C[i * N + j] += temp_A * B[k * N + j];
            }
        }
    }
}

double get_geometric_mean(const vector<double>& times) {
    double sum_log = 0.0;
    for (double t : times) sum_log += log(t);
    return exp(sum_log / times.size());
}

void call_blas_gemm(int M, int N, int K, float alpha, const float* A, const float* B, float beta, float* C) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha, A, K, B, N, beta, C, N);
}

void call_blas_gemm(int M, int N, int K, double alpha, const double* A, const double* B, double beta, double* C) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha, A, K, B, N, beta, C, N);
}

template <typename T>
void run_benchmark(int threads, int size, string type_name) {

    omp_set_num_threads(threads);
    openblas_set_num_threads(threads); 

    int M = size, N = size, K = size;
    T alpha = 1.0, beta = 0.0;
    
    vector<T> A(M * K, 1.1), B(K * N, 2.2), C_my(M * N, 0.0), C_blas(M * N, 0.0);
    vector<double> my_times, blas_times;

    cout << "--- Тест [" << type_name << "]: " << threads << " поток(ов), размер " << size << "x" << size << " ---" << endl;

    for (int i = 0; i < 10; ++i) {
        auto s1 = chrono::high_resolution_clock::now();
        my_gemm(M, N, K, alpha, A.data(), B.data(), beta, C_my.data());
        auto e1 = chrono::high_resolution_clock::now();
        my_times.push_back(chrono::duration<double>(e1 - s1).count());

        auto s2 = chrono::high_resolution_clock::now();
        call_blas_gemm(M, N, K, alpha, A.data(), B.data(), beta, C_blas.data());
        auto e2 = chrono::high_resolution_clock::now();
        blas_times.push_back(chrono::duration<double>(e2 - s2).count());
    }

    double my_mean = get_geometric_mean(my_times);
    double blas_mean = get_geometric_mean(blas_times);
    double efficiency = (blas_mean / my_mean) * 100.0;

    cout << fixed << setprecision(4);
    cout << "Мой GEMM (среднее): " << my_mean << " сек" << endl;
    cout << "OpenBLAS (среднее):  " << blas_mean << " сек" << endl;
    cout << "Эффективность:       " << efficiency << "% от OpenBLAS" << endl << endl;
}

int main() {
    
    int test_size = 2000; 
    int thread_counts[] = {1, 2, 4, 8, 16};

    for (int t : thread_counts) {
        run_benchmark<float>(t, test_size, "float");
        run_benchmark<double>(t, test_size, "double");
    }

    return 0;
}