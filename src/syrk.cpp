/* syrk.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include "polybench_raja.hpp"
/* Include benchmark-specific header. */
#include "syrk.hpp"


static void init_array(int n,
                       int m,
                       double *alpha,
                       double *beta,
                       double C[N][N],
                       double A[N][M]) {
  *alpha = 1.5;
  *beta = 1.2;
  RAJA::forallN<Independent2DTiled> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, m },
    [=] (int i, int j) {
      A[i][j] = (double)((i * j + 1) % n) / n;
    }
  );
  RAJA::forallN<Independent2DTiled> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, n },
    [=] (int i, int j) {
      C[i][j] = (double)((i * j + 2) % m) / m;
    }
  );
}

static void print_array(int n, double C[N][N]) {
  int i, j;
  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "C");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", C[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "C");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

static void kernel_syrk(int n,
                        int m,
                        double alpha,
                        double beta,
                        double C[N][N],
                        double A[N][M]) {
#pragma scop
  RAJA::forallN<Independent2D> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, n },
    [=] (int i, int j) {
      if (j <= i) {
        C[i][j] *= beta;
        RAJA::forall<RAJA::simd_exec> (0, m, [=] (int k) {
          C[i][j] += alpha * A[i][k] * A[j][k];
        });
      }
    }
  );
#pragma endscop
}

int main(int argc, char **argv) {
  int n = N;
  int m = M;
  double alpha;
  double beta;
  double(*C)[N][N];
  C = (double(*)[N][N])polybench_alloc_data((N) * (N), sizeof(double));
  double(*A)[N][M];
  A = (double(*)[N][M])polybench_alloc_data((N) * (M), sizeof(double));
  init_array(n, m, &alpha, &beta, *C, *A);
  polybench_timer_start();
  kernel_syrk(n, m, alpha, beta, *C, *A);
  polybench_timer_stop();
  polybench_timer_print();
  if (argc > 42 && !strcmp(argv[0], "")) print_array(n, *C);
  free((void *)C);
  free((void *)A);
  return 0;
}
