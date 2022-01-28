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

#ifndef FSPMV_SPARSEMATRIX_HPP
#define FSPMV_SPARSEMATRIX_HPP

#include "fspmv_Config.hpp"
#include <cstddef>
#include <vector>

namespace fspmv {

class SparseMatrix {
 public:
  using index_type       = fspmv_index_type;
  using value_type       = fspmv_value_type;
  using index_array_type = std::vector<index_type, host_allocator<index_type>>;
  using value_array_type = std::vector<value_type, host_allocator<value_type>>;

  index_type nrows;
  index_type ncols;
  size_t nnnz;

  index_array_type row_offsets;
  index_array_type column_indices;
  value_array_type values;

  ~SparseMatrix()                    = default;
  SparseMatrix(const SparseMatrix &) = default;
  SparseMatrix(SparseMatrix &&)      = default;
  SparseMatrix &operator=(const SparseMatrix &) = default;
  SparseMatrix &operator=(SparseMatrix &&) = default;

  // Construct an empty SparseMatrix
  inline SparseMatrix()
      : nrows(0),
        ncols(0),
        nnnz(0),
        row_offsets(1),
        column_indices(0),
        values(0) {}

  // Construct with a specific shape and number of non-zero entries
  inline SparseMatrix(index_type num_rows, index_type num_cols,
                      index_type num_entries)
      : nrows(num_rows),
        ncols(num_cols),
        nnnz(num_entries),
        row_offsets(num_rows + 1),
        column_indices(num_entries),
        values(num_entries) {}

  inline void resize(index_type num_rows, index_type num_cols,
                     index_type num_entries) {
    nrows = num_rows;
    ncols = num_cols;
    nnnz  = num_entries;

    row_offsets.resize(num_rows + 1);
    column_indices.resize(num_entries);
    values.resize(num_entries);
  }
};

}  // namespace fspmv

#endif  // FSPMV_SPARSEMATRIX_HPP

