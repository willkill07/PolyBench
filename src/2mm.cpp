
/* 2mm.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include "polybench_raja.hpp"
/* Include benchmark-specific header. */
#include "2mm.hpp"

static void init_array(int ni,
                       int nj,
                       int nk,
                       int nl,
                       double *alpha,
                       double *beta,
                       double A[NI][NK],
                       double B[NK][NJ],
                       double C[NJ][NL],
                       double D[NI][NL]) {
  *alpha = 1.5;
  *beta = 1.2;

  RAJA::forallN <Independent2DTiled> (
    RAJA::RangeSegment { 0, ni },
    RAJA::RangeSegment { 0, nk },
    [=] (int i, int j) {
      A[i][j] = (double)((i * j + 1) % ni) / ni;
    }
  );
  RAJA::forallN <Independent2DTiled> (
    RAJA::RangeSegment { 0, nk },
    RAJA::RangeSegment { 0, nj },
    [=] (int i, int j) {
      B[i][j] = (double)(i * (j + 1) % nj) / nj;
    }
  );
  RAJA::forallN <Independent2DTiled> (
    RAJA::RangeSegment { 0, nj },
    RAJA::RangeSegment { 0, nl },
    [=] (int i, int j) {
      C[i][j] = (double)((i * (j + 3) + 1) % nl) / nl;
    }
  );
  RAJA::forallN <Independent2DTiled> (
    RAJA::RangeSegment { 0, ni },
    RAJA::RangeSegment { 0, nl },
    [=] (int i, int j) {
      D[i][j] = (double)(i * (j + 2) % nk) / nk;
    }
  );
}

static void print_array(int ni, int nl, double D[NI][NL]) {
  int i, j;
  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "D");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
      if ((i * ni + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", D[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "D");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

static void kernel_2mm(int ni,
                       int nj,
                       int nk,
                       int nl,
                       double alpha,
                       double beta,
                       double tmp[NI][NJ],
                       double A[NI][NK],
                       double B[NK][NJ],
                       double C[NJ][NL],
                       double D[NI][NL]) {
#pragma scop
  RAJA::forallN <Independent2DTiled> (
    RAJA::RangeSegment { 0, ni },
    RAJA::RangeSegment { 0, nj },
    [=] (int i, int j) {
      double v { 0.0 };
      RAJA::forall <RAJA::simd_exec> (0, nk, [=] (int k) mutable {
        v += alpha * A[i][k] * B[k][j];
      });
      tmp[i][j] = v;
    }
  );
  RAJA::forallN <Independent2DTiled> (
    RAJA::RangeSegment { 0, ni },
    RAJA::RangeSegment { 0, nl },
    [=] (int i, int j) {
      double v { 0.0 };
      RAJA::forall <RAJA::simd_exec> (0, nk, [=] (int k) mutable {
        v += tmp[i][k] * C[k][j];
      });
      D[i][j] = v;
    }
  );
#pragma endscop
}

int main(int argc, char **argv) {
  int ni = NI;
  int nj = NJ;
  int nk = NK;
  int nl = NL;
  double alpha;
  double beta;
  double(*tmp)[NI][NJ];
  tmp = (double(*)[NI][NJ])polybench_alloc_data((NI) * (NJ), sizeof(double));
  double(*A)[NI][NK];
  A = (double(*)[NI][NK])polybench_alloc_data((NI) * (NK), sizeof(double));
  double(*B)[NK][NJ];
  B = (double(*)[NK][NJ])polybench_alloc_data((NK) * (NJ), sizeof(double));
  double(*C)[NJ][NL];
  C = (double(*)[NJ][NL])polybench_alloc_data((NJ) * (NL), sizeof(double));
  double(*D)[NI][NL];
  D = (double(*)[NI][NL])polybench_alloc_data((NI) * (NL), sizeof(double));
  init_array(ni, nj, nk, nl, &alpha, &beta, *A, *B, *C, *D);
  polybench_timer_start();
  kernel_2mm(ni, nj, nk, nl, alpha, beta, *tmp, *A, *B, *C, *D);
  polybench_timer_stop();
  polybench_timer_print();
  if (argc > 42 && !strcmp(argv[0], "")) print_array(ni, nl, *D);
  free((void *)tmp);
  free((void *)A);
  free((void *)B);
  free((void *)C);
  free((void *)D);
  return 0;
}
