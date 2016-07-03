/* 3mm.c: this file is part of PolyBench/C */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/* Include polybench common header. */
#include "PolyBenchRAJA.hpp"
/* Include benchmark-specific header. */
#include "3mm.hpp"

static void init_array(int ni,
                       int nj,
                       int nk,
                       int nl,
                       int nm,
                       Arr2D<double>* A,
                       Arr2D<double>* B,
                       Arr2D<double>* C,
                       Arr2D<double>* D) {
  RAJA::forallN<Independent2D>(RAJA::RangeSegment{0, ni},
                               RAJA::RangeSegment{0, nk},
                               [=](int i, int k) {
                                 A->at(i, k) =
                                     (double)((i * k + 1) % ni) / (5 * ni);
                               });
  RAJA::forallN<Independent2D>(RAJA::RangeSegment{0, nk},
                               RAJA::RangeSegment{0, nj},
                               [=](int k, int j) {
                                 B->at(k, j) = (double)((k * (j + 1) + 2) % nj)
                                               / (5 * nj);
                               });
  RAJA::forallN<Independent2D>(RAJA::RangeSegment{0, nj},
                               RAJA::RangeSegment{0, nm},
                               [=](int j, int m) {
                                 C->at(j, m) =
                                     (double)(j * (m + 3) % nl) / (5 * nl);
                               });
  RAJA::forallN<Independent2D>(RAJA::RangeSegment{0, nm},
                               RAJA::RangeSegment{0, nl},
                               [=](int m, int l) {
                                 A->at(m, l) = (double)((m * (l + 2) + 2) % nk)
                                               / (5 * nk);
                               });
}

static void print_array(int ni, int nl, const Arr2D<double>* G) {
  int i, j;
  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "G");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
      if ((i * ni + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", G->at(i, j));
    }
  fprintf(stderr, "\nend   dump: %s\n", "G");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

static void kernel_3mm(int ni,
                       int nj,
                       int nk,
                       int nl,
                       int nm,
                       Arr2D<double>* E,
                       const Arr2D<double>* A,
                       const Arr2D<double>* B,
                       Arr2D<double>* F,
                       const Arr2D<double>* C,
                       const Arr2D<double>* D,
                       Arr2D<double>* G) {
#pragma scop
  using ExecPolicy = RAJA::
      NestedPolicy<RAJA::ExecList<RAJA::omp_collapse_nowait_exec,
                                  RAJA::omp_collapse_nowait_exec,
                                  RAJA::simd_exec>,
                   RAJA::
                       Tile<RAJA::TileList<RAJA::tile_fixed<16>,
                                           RAJA::tile_fixed<16>,
                                           RAJA::tile_none>,
                            RAJA::
                                OMP_Parallel<RAJA::
                                                 Permute<RAJA::PERM_IJK> > > >;
  RAJA::forallN<ExecPolicy>(RAJA::RangeSegment{0, ni},
                            RAJA::RangeSegment{0, nj},
                            RAJA::RangeSegment{0, nk},
                            [=](int i, int j, int k) {
                              E->at(i, j) += A->at(i, k) * B->at(k, j);
                            });
  RAJA::forallN<ExecPolicy>(RAJA::RangeSegment{0, nj},
                            RAJA::RangeSegment{0, nl},
                            RAJA::RangeSegment{0, nk},
                            [=](int j, int l, int k) {
                              F->at(j, l) += C->at(j, k) * D->at(k, l);
                            });
  RAJA::forallN<ExecPolicy>(RAJA::RangeSegment{0, ni},
                            RAJA::RangeSegment{0, nl},
                            RAJA::RangeSegment{0, nj},
                            [=](int i, int l, int j) {
                              G->at(i, l) += E->at(i, j) * F->at(j, l);
                            });
#pragma endscop
}

int main(int argc, char** argv) {
  int ni = NI;
  int nj = NJ;
  int nk = NK;
  int nl = NL;
  int nm = NM;
  Arr2D<double> E{ni, nj};
  Arr2D<double> A{ni, nk};
  Arr2D<double> B{nk, nj};
  Arr2D<double> F{nj, nl};
  Arr2D<double> C{nj, nm};
  Arr2D<double> D{nm, nl};
  Arr2D<double> G{ni, nl};

  init_array(ni, nj, nk, nl, nm, &A, &B, &C, &D);
  {
    util::block_timer t{"3MM"};
    kernel_3mm(ni, nj, nk, nl, nm, &E, &A, &B, &F, &C, &D, &G);
  }
  if (argc > 42) print_array(ni, nl, &G);
  return 0;
}
