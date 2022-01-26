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
#include "fspmv_Print.hpp"

#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "fSpmv::main.cpp requires 1 input argument to be given "
                 "at runtime (filename). Only received "
              << argc - 1 << "!" << std::endl;
    exit(-1);
  }

  std::string filename = argv[1];

  fspmv::SparseMatrix A;
  fspmv::Vector x, y;

  fspmv::read_matrix(A, filename);

  x.resize(A.ncols);
  x.assign(2.0);

  y.resize(A.nrows);
  y.assign(0.0);

  fspmv::spmv(A, x, y);

  fspmv::print(y, std::cout);

  return 0;
}