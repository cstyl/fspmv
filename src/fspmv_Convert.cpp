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

#include "fspmv_Convert.hpp"

namespace fspmv {

void convert(const CooMatrix& src, SparseMatrix& dst) {
  // Complexity: Linear.  Specifically O(nnz(coo) + max(n_row,n_col))
  using index_type = typename CooMatrix::index_type;

  dst.resize(src.nrows, src.ncols, src.nnnz);

  // compute number of non-zero entries per row of coo src
  for (index_type n = 0; n < src.nnnz; n++) {
    dst.row_offsets[src.row_indices[n]]++;
  }

  // cumsum the nnz per row to get csr row_offsets
  for (index_type i = 0, cumsum = 0; i < src.nrows; i++) {
    index_type temp    = dst.row_offsets[i];
    dst.row_offsets[i] = cumsum;
    cumsum += temp;
  }
  dst.row_offsets[src.nrows] = src.nnnz;

  // write coo column indices and values into csr
  for (index_type n = 0; n < src.nnnz; n++) {
    index_type row  = src.row_indices[n];
    index_type dest = dst.row_offsets[row];

    dst.column_indices[dest] = src.column_indices[n];
    dst.values[dest]         = src.values[n];

    dst.row_offsets[row]++;
  }

  for (index_type i = 0, last = 0; i <= src.nrows; i++) {
    index_type temp    = dst.row_offsets[i];
    dst.row_offsets[i] = last;
    last               = temp;
  }

  // TODO: remove duplicates, if any?
}

}  // namespace fspmv