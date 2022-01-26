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

#include "fspmv_ComputeSPMV.hpp"

namespace fspmv {

void spmv(const SparseMatrix& A, const Vector& x, Vector& y) {
  for (index_type i = 0; i < A.nrows; i++) {
    for (index_type jj = A.row_offsets[i]; jj < A.row_offsets[i + 1]; jj++) {
      y.values[i] = A.values[jj] * x.values[A.column_indices[jj]];
    }
  }
}

}  // namespace fspmv