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

#ifndef FSPMV_COMPUTESPMV_HPP
#define FSPMV_COMPUTESPMV_HPP

#include "fspmv_SparseMatrix.hpp"
#include "fspmv_Vector.hpp"

namespace fspmv {

void spmv(const SparseMatrix& A, const Vector& x, Vector& y);

}

#endif  // FSPMV_COMPUTESPMV_HPP