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

#ifndef FSPMV_PRINT_HPP
#define FSPMV_PRINT_HPP

#include "fspmv_SparseMatrix.hpp"
#include "fspmv_CooMatrix.hpp"
#include "fspmv_Vector.hpp"

#include <iostream>
#include <iomanip>  // setw, setprecision

namespace fspmv {
namespace Impl {
template <typename Printable, typename Stream>
void print_matrix_header(const Printable& p, Stream& s) {
  s << "Matrix <" << p.nrows << ", " << p.ncols << "> with " << p.nnnz
    << " entries\n";
}
}  // namespace Impl

template <typename Stream>
void print(const CooMatrix& p, Stream& s) {
  using IndexType = typename CooMatrix::index_type;
  Impl::print_matrix_header(p, s);

  for (IndexType n = 0; n < p.nnnz; n++) {
    s << " " << std::setw(14) << p.row_indices[n];
    s << " " << std::setw(14) << p.column_indices[n];
    s << " " << std::setprecision(4) << std::setw(8) << "(" << p.values[n]
      << ")\n";
  }
}

template <typename Stream>
void print(const SparseMatrix& p, Stream& s) {
  using IndexType = typename SparseMatrix::index_type;
  Impl::print_matrix_header(p, s);

  for (IndexType i = 0; i < p.nrows; i++) {
    for (IndexType jj = p.row_offsets[i]; jj < p.row_offsets[i + 1]; jj++) {
      s << " " << std::setw(14) << i;
      s << " " << std::setw(14) << p.column_indices[jj];
      s << " " << std::setprecision(4) << std::setw(8) << "(" << p.values[jj]
        << ")\n";
    }
  }
}

template <typename Stream>
void print(const Vector& p, Stream& s) {
  using IndexType = typename Vector::index_type;

  s << "Vector <" << p.length << ">\n";

  for (IndexType i = 0; i < p.length; i++) {
    s << " " << std::setw(14) << i;
    s << " " << std::setprecision(4) << std::setw(8) << "(" << p.values[i]
      << ")\n";
  }
}

}  // namespace fspmv
#endif  // FSPMV_PRINT_HPP