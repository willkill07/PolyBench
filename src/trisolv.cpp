#include <array>
#include <iostream>
#include <tuple>

#include "PolyBench.hpp"
#include "PolyBench/C++/Base/trisolv.hpp"
#include "PolyBench/C++/OpenMP/trisolv.hpp"
#include "PolyBench/RAJA/Base/trisolv.hpp"
#include "PolyBench/RAJA/OpenMP/trisolv.hpp"

using DataType = Base::trisolv<dummy_t>::default_datatype;
using Kernels = std::tuple<CPlusPlus::Base::trisolv<DataType>,
                           CPlusPlus::OpenMP::trisolv<DataType>,
                           RAJA::Base::trisolv<DataType>,
                           RAJA::OpenMP::trisolv<DataType>>;

using Args = GetArgs<Kernels>;
constexpr int ARGC = CountArgs<Kernels>::value;

int main(int argc, char **argv)
{
  if (argc != ARGC + 1) {
    std::cerr << "Invalid number of parameters (expected " << ARGC << ")"
              << std::endl;
    return EXIT_FAILURE;
  }
  auto args = parseArgs<Args>(argv + 1);
  KernelPacker versions;
  versions.addAll<Kernels>(args);
  versions.run();
  return versions.check();
}
