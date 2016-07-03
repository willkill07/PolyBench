# PolyBench/C-RAJA 0.1.0

Copyright (c) 2011-2016 the Ohio State University.

Copyright (c) 2016 Lawrence Livermore National Laboratory

Copyright (c) 2016 University of Delaware

Contact:

* Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
* Tomofumi Yuki <tomofumi.yuki@inria.fr>
* William Killian <killian4@llnl.gov> <killian@udel.edu>

PolyBench is a benchmark suite of 30 numerical computations with
static control flow, extracted from operations in various application
domains (linear algebra computations, image processing, physics
simulation, dynamic programming, statistics, etc.). PolyBench features
include:
- A single file, tunable at compile-time, used for the kernel
  instrumentation. It performs extra operations such as cache flushing
  before the kernel execution, and can set real-time scheduling to
  prevent OS interference.
- Non-null data initialization, and live-out data dump.
- Syntactic constructs to prevent any dead code elimination on the kernel.
- Parametric loop bounds in the kernels, for general-purpose implementation.
- Clear kernel marking, using pragma-based delimiters.

## Available benchmarks (PolyBench/C 4.2.1)

| Benchmark | Description |
| --------- | ----------- |
| 2mm | 2 Matrix Multiplications (alpha * A * B * C + beta * D) |
| 3mm | 3 Matrix Multiplications ((A*B)*(C*D)) |
| adi | Alternating Direction Implicit solver |
| atax | Matrix Transpose and Vector Multiplication |
| bicg | BiCG Sub Kernel of BiCGStab Linear Solver |
| cholesky | Cholesky Decomposition |
| correlation | Correlation Computation |
| covariance | Covariance Computation |
| deriche | Edge detection filter |
| doitgen | Multi-resolution analysis kernel (MADNESS) |
| durbin | Toeplitz system solver |
| fdtd-2d | 2-D Finite Different Time Domain Kernel |
| gemm | Matrix-multiply C=alpha.A.B+beta.C |
| gemver | Vector Multiplication and Matrix Addition |
| gesummv | Scalar, Vector and Matrix Multiplication |
| gramschmidt | Gram-Schmidt decomposition |
| head-3d | Heat equation over 3D data domain |
| jacobi-1D | 1-D Jacobi stencil computation |
| jacobi-2D | 2-D Jacobi stencil computation |
| lu | LU decomposition |
| ludcmp | LU decomposition followed by Forward Substitution |
| mvt | Matrix Vector Product and Transpose |
| nussinov | Dynamic programming algorithm for sequence alignment |
| seidel | 2-D Seidel stencil computation |
| symm | Symmetric matrix-multiply |
| syr2k | Symmetric rank-2k update |
| syrk | Symmetric rank-k update |
| trisolv | Triangular solver |
| trmm | Triangular matrix-multiply |


### Mailing lists:

polybench-announces@lists.sourceforge.net

* Announces about releases of PolyBench.

polybench-discussion@lists.sourceforge.net

* General discussions reg. PolyBench.

### Available benchmarks:

* See utilities/benchmark_list for paths to each files.

### Sample compilation commands

* To compile a benchmark without any monitoring:

`gcc -I utilities -I linear-algebra/kernels/atax utilities/polybench.c linear-algebra/kernels/atax/atax.c -o atax_base`

* To compile a benchmark with execution time reporting:

`gcc -O3 -I utilities -I linear-algebra/kernels/atax utilities/polybench.c linear-algebra/kernels/atax/atax.c -DPOLYBENCH_TIME -o atax_time`

* To generate the reference output of a benchmark:

```
$> gcc -O0 -I utilities -I linear-algebra/kernels/atax utilities/polybench.c linear-algebra/kernels/atax/atax.c -DPOLYBENCH_DUMP_ARRAYS -o atax_ref
$> ./atax_ref 2>atax_ref.out
```

#### Some available options:

They are all passed as macro definitions during compilation time (e.g, `-Dname_of_the_option`).

#### Typical options:

* `POLYBENCH_TIME`: output execution time (gettimeofday) [default: off]

* `MINI_DATASET`, `SMALL_DATASET`, `MEDIUM_DATASET`, `LARGE_DATASET`, `EXTRALARGE_DATASET`: set the dataset size to be used [default: `STANDARD_DATASET`]

* `POLYBENCH_DUMP_ARRAYS`: dump all live-out arrays on stderr [default: off]

* `POLYBENCH_STACK_ARRAYS`: use stack allocation instead of malloc [default: off]


#### Options that may lead to better performance:

* `POLYBENCH_USE_RESTRICT`: Use restrict keyword to allow compilers to assume absence of aliasing. [default: off]

* `POLYBENCH_USE_SCALAR_LB`: Use scalar loop bounds instead of parametric ones. [default: off]

* `POLYBENCH_PADDING_FACTOR`: Pad all dimensions of all arrays by this value [default: 0]

* `POLYBENCH_INTER_ARRAY_PADDING_FACTOR`: Offset the starting address of polybench arrays allocated on the heap (default) by a multiple of this value [default: 0]

* `POLYBENCH_USE_C99_PROTO`: Use standard C99 prototype for the functions. [default: off]


#### Timing/profiling options:

* `POLYBENCH_PAPI`: turn on papi timing (see below).

* `POLYBENCH_CACHE_SIZE_KB`: cache size to flush, in kB [default: 33MB]

* `POLYBENCH_NO_FLUSH_CACHE`: don't flush the cache before calling the timer [default: flush the cache]

* `POLYBENCH_CYCLE_ACCURATE_TIMER`: Use Time Stamp Counter to monitor the execution time of the kernel [default: off]

* `POLYBENCH_LINUX_FIFO_SCHEDULER`: use FIFO real-time scheduler for the kernel execution, the program must be run as root, under linux only, and compiled with -lc [default: off]


## PAPI support

* To compile a benchmark with PAPI support:
`gcc -O3 -I utilities -I linear-algebra/kernels/atax utilities/polybench.c linear-algebra/kernels/atax/atax.c -DPOLYBENCH_PAPI -lpapi -o atax_papi`

* To specify which counter(s) to monitor:

** Edit utilities/papi_counters.list, and add 1 line per event to monitor. Each line (including the last one) must finish with a ',' and both native and standard events are supported.

** The whole kernel is run one time per counter (no multiplexing) and there is no sampling being used for the counter value.

## Accurate performance timing:

With kernels that have an execution time in the orders of a few tens of milliseconds, it is critical to validate any performance number by repeating several times the experiment. A companion script is available to perform reasonable performance measurement of a PolyBench.

```
gcc -O3 -I utilities -I linear-algebra/kernels/atax utilities/polybench.c linear-algebra/kernels/atax/atax.c -DPOLYBENCH_TIME -o atax_time
./utilities/time_benchmark.sh ./atax_time
```

This script will run the benchmark five times (that must be a PolyBench compiled with `-DPOLYBENCH_TIME`), eliminate the two extremal times, and check that the deviation of the three remaining does not exceed a given threshold, set to 5%. It is also possible to use `POLYBENCH_CYCLE_ACCURATE_TIMER` to use the Time Stamp Counter instead of `gettimeofday()` to monitor the number of elapsed cycles.

## Utility scripts:

* `create_cpped_version.pl`: Used in the above for generating macro free version.
* `makefile-gen.pl`: generates make files in each directory. Options are globally configurable through config.mk at polybench root.
* `header-gen.pl`: refers to `polybench.spec` file and generates header in each directory. Allows default problem sizes and datatype to be configured without going into each header file.
* `run-all.pl`: compiles and runs each kernel.
* `clean.pl`: runs make clean in each directory and then removes Makefile.
