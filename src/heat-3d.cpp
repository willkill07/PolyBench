#include <array>
#include <iostream>
#include <tuple>

#include "PolyBench.hpp"
#include "PolyBench/C++/Base/heat-3d.hpp"
#include "PolyBench/C++/OpenMP/heat-3d.hpp"
#include "PolyBench/RAJA/Base/heat-3d.hpp"
#include "PolyBench/RAJA/OpenMP/heat-3d.hpp"

using DataType = Base::heat_3d<dummy_t>::default_datatype;
using Kernels = std::tuple<CPlusPlus::Base::heat_3d<DataType>,
                           CPlusPlus::OpenMP::heat_3d<DataType>,
                           RAJA::Base::heat_3d<DataType>,
                           RAJA::OpenMP::heat_3d<DataType>>;

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
