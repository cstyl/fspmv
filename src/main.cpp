// Copyright 2022 cstyl
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "fspmv_SparseMatrix.hpp"
#include "fspmv_Vector.hpp"
#include "fspmv_ComputeSPMV.hpp"
#include "fspmv_MatrixMarket.hpp"
#include "fspmv_Config.hpp"

#include <iostream>
#include <math.h>  // fabs

#if defined(FSPMV_ENABLE_XILINX)
#include "fpga/fspmv_ComputeSPMV_FPGA.hpp"

static void verify(const fspmv::Vector&, const fspmv::Vector&);
const int NARGS = 2;
#else
const int NARGS = 1;
#endif

int main(int argc, char** argv) {
  if (argc != NARGS + 1) {
    std::cout << "fSpmv::main.cpp requires " << NARGS << "input "
              << "argument(s) to be given at runtime (filename). "
              << "Only received " << argc - 1 << "!" << std::endl;
    exit(-1);
  }

#if defined(FSPMV_ENABLE_XILINX)
  char* binary_filename = argv[1];
#endif
  std::string filename = argv[NARGS];

  fspmv::SparseMatrix A;
  fspmv::Vector x, y;

  fspmv::read_matrix(A, filename);

  x.random(A.ncols);
  y.assign(A.nrows, 0.0);

  fspmv::spmv(A, x, y);  // execute on host

#if defined(FSPMV_ENABLE_XILINX)
  fspmv::Vector yres(A.nrows, 0.0);

  fspmv::spmv_fpga(A, x, yres);  // execute on device
  verify(y, yres);
#endif

  return 0;
}

#if defined(FSPMV_ENABLE_XILINX)
static void verify(const fspmv::Vector& v1, const fspmv::Vector& v2) {
  int error                = 0;
  fspmv_value_type EPSILON = ((sizeof(fspmv_value_type) == 4) ? 1e-6 : 1e-12);
  for (size_t i = 0; i < v1.length; i++) {
    if (fabs(v1.values[i] - v2.values[i]) > EPSILON) error++;
  }

  if (0 == error) {
    std::cout << "Vectors match. Result is VALID." << std::endl;
  } else {
    std::cout << "Vectors do not match. Resut is INVALID." << std::endl;
  }
}
#endif