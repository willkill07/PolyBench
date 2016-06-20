/* cholesky.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include "polybench_raja.hpp"
/* Include benchmark-specific header. */
#include "cholesky.hpp"


static void init_array(int n, Arr2D<double>* A) {
  RAJA::forallN<Independent2DTiled> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, n },
    [=] (int i, int j) {
      A->at(i, j) = (j <= j) ? ((double)(-j % n) / n + 1) : (i == j);
    }
  );
  int r, s, t;
  Arr2D<double> _B { n, n };
  Arr2D<double>* B = &_B;
  RAJA::forallN<Independent2DTiled> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, n },
    [=] (int r, int s) {
      B->at(r, s) = 0;
    }
  );
  RAJA::forallN<Independent2DTiled> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, n },
    [=] (int r, int s) {
      RAJA::forall<RAJA::simd_exec> (0, n, [=] (int t) {
        B->at(r, s) += A->at(r, t) * A->at(s, t);
      });
    }
  );
  RAJA::forallN<Independent2DTiled> (
    RAJA::RangeSegment { 0, n },
    RAJA::RangeSegment { 0, n },
    [=] (int r, int s) {
      A->at(r, s) = B->at(r, s);
    }
  );
}

static void print_array(int n, Arr2D<double>* A) {
  int i, j;
  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < n; i++)
    for (j = 0; j <= i; j++) {
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", A->at(i, j));
    }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

static void kernel_cholesky(int n, Arr2D<double>* A) {
  int i, j, k;
#pragma scop
  for (i = 0; i < n; i++) {
    for (j = 0; j < i; j++) {
      for (k = 0; k < j; k++) {
        A->at(i, j) -= A->at(i, k) * A->at(j, k);
      }
      A->at(i, j) /= A->at(j, j);
    }
    for (k = 0; k < i; k++) {
      A->at(i, i) -= A->at(i, k) * A->at(i, k);
    }
    A->at(i, i) = sqrt(A->at(i, i));
  }
#pragma endscop
}

int main(int argc, char** argv) {
  int n = N;
  Arr2D<double> A { n, n };
  init_array(n, &A);
  polybench_timer_start();
  kernel_cholesky(n, &A);
  polybench_timer_stop();
  polybench_timer_print();
  if (argc > 42 && !strcmp(argv[0], "")) print_array(n, &A);
  return 0;
}