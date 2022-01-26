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

#ifndef FSPMV_COOMATRIX_HPP
#define FSPMV_COOMATRIX_HPP

#include "fspmv_Config.hpp"
#include <cstddef>
#include <vector>
#include <iostream>

namespace fspmv {

class CooMatrix {
 public:
  using index_type       = fspmv_index_type;
  using value_type       = fspmv_value_type;
  using index_array_type = std::vector<index_type, host_allocator<index_type>>;
  using value_array_type = std::vector<value_type, host_allocator<value_type>>;

  index_type nrows;
  index_type ncols;
  size_t nnnz;

  index_array_type row_indices, column_indices;
  value_array_type values;

  ~CooMatrix()                = default;
  CooMatrix(const CooMatrix&) = default;
  CooMatrix(CooMatrix&&)      = default;
  CooMatrix& operator=(const CooMatrix&) = default;
  CooMatrix& operator=(CooMatrix&&) = default;

  // Construct an empty CooMatrix
  inline CooMatrix()
      : nrows(0),
        ncols(0),
        nnnz(0),
        row_indices(0),
        column_indices(0),
        values(0) {}

  // Construct with a specific shape and number of non-zero entries
  inline CooMatrix(index_type num_rows, index_type num_cols,
                   index_type num_entries)
      : nrows(num_rows),
        ncols(num_cols),
        nnnz(num_entries),
        row_indices(num_entries),
        column_indices(num_entries),
        values(num_entries) {}

  inline void resize(index_type num_rows, index_type num_cols,
                     size_t num_entries) {
    nrows = num_rows;
    ncols = num_cols;
    nnnz  = num_entries;

    row_indices.resize(num_entries);
    column_indices.resize(num_entries);
    values.resize(num_entries);
  }

  inline void copy(CooMatrix& matrix) {
    std::copy(matrix.row_indices.begin(), matrix.row_indices.end(),
              row_indices.begin());
    std::copy(matrix.column_indices.begin(), matrix.column_indices.end(),
              column_indices.begin());
    std::copy(matrix.values.begin(), matrix.values.end(), values.begin());
  }
};

}  // namespace fspmv
#endif  // FSPMV_COOMATRIX_HPP