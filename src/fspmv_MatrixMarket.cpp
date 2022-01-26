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

#include "fspmv_MatrixMarket.hpp"
#include "fspmv_Exceptions.hpp"
#include "fspmv_CooMatrix.hpp"
#include "fspmv_Convert.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace fspmv {
namespace Impl {

inline void tokenize(std::vector<std::string>& tokens, const std::string& str,
                     const std::string& delimiters = "\n\r\t ") {
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

struct matrix_market_banner {
  std::string storage;   // "array" or "coordinate"
  std::string symmetry;  // "general", "symmetric"
                         // "hermitian", or "skew-symmetric"
  std::string type;      // "complex", "real", "integer", or "pattern"
};

template <typename Stream>
void read_matrix_market_banner(matrix_market_banner& banner, Stream& input) {
  std::string line;
  std::vector<std::string> tokens;

  // read first line
  std::getline(input, line);
  Impl::tokenize(tokens, line);

  if (tokens.size() != 5 || tokens[0] != "%%MatrixMarket" ||
      tokens[1] != "matrix")
    throw fspmv::IOException("invalid MatrixMarket banner");

  banner.storage  = tokens[2];
  banner.type     = tokens[3];
  banner.symmetry = tokens[4];

  if (banner.storage != "array" && banner.storage != "coordinate")
    throw fspmv::IOException("invalid MatrixMarket storage format [" +
                             banner.storage + "]");

  if (banner.type != "complex" && banner.type != "real" &&
      banner.type != "integer" && banner.type != "pattern")
    throw fspmv::IOException("invalid MatrixMarket data type [" + banner.type +
                             "]");

  if (banner.symmetry != "general" && banner.symmetry != "symmetric" &&
      banner.symmetry != "hermitian" && banner.symmetry != "skew-symmetric")
    throw fspmv::IOException("invalid MatrixMarket symmetry [" +
                             banner.symmetry + "]");
}

template <typename Stream>
void read_coordinate_stream(CooMatrix& mat, Stream& input,
                            const matrix_market_banner& banner) {
  using index_type = typename CooMatrix::index_type;
  using value_type = typename CooMatrix::value_type;
  // read file contents line by line
  std::string line;

  // skip over banner and comments
  do {
    std::getline(input, line);
  } while (line[0] == '%');

  // line contains [num_rows num_columns num_entries]
  std::vector<std::string> tokens;
  Impl::tokenize(tokens, line);

  if (tokens.size() != 3)
    throw fspmv::IOException("invalid MatrixMarket coordinate format");

  index_type num_rows, num_cols, num_entries;

  std::istringstream(tokens[0]) >> num_rows;
  std::istringstream(tokens[1]) >> num_cols;
  std::istringstream(tokens[2]) >> num_entries;

  mat.resize(num_rows, num_cols, num_entries);

  index_type num_entries_read = 0;

  // read file contents
  if (banner.type == "pattern") {
    while (num_entries_read < num_entries && !input.eof()) {
      input >> mat.row_indices[num_entries_read];
      input >> mat.column_indices[num_entries_read];
      num_entries_read++;
    }

    std::fill(mat.values.data(), mat.values.data() + mat.values.size(),
              value_type(1));
  } else if (banner.type == "real" || banner.type == "integer") {
    while (num_entries_read < num_entries && !input.eof()) {
      value_type real;

      input >> mat.row_indices[num_entries_read];
      input >> mat.column_indices[num_entries_read];
      input >> real;

      mat.values[num_entries_read] = real;
      num_entries_read++;
    }
  } else if (banner.type == "complex") {
    throw fspmv::NotImplementedException(
        "fspmv does not currently support complex "
        "matrices");

  } else {
    throw fspmv::IOException("invalid MatrixMarket data type");
  }

  if (num_entries_read != num_entries)
    throw fspmv::IOException(
        "unexpected EOF while reading MatrixMarket entries");

  // check validity of row and column index data
  if (num_entries > 0) {
    index_type min_row_index =
        *std::min_element(mat.row_indices.data(),
                          mat.row_indices.data() + mat.row_indices.size());
    index_type max_row_index =
        *std::max_element(mat.row_indices.data(),
                          mat.row_indices.data() + mat.row_indices.size());

    index_type min_col_index = *std::min_element(
        mat.column_indices.data(),
        mat.column_indices.data() + mat.column_indices.size());
    index_type max_col_index = *std::max_element(
        mat.column_indices.data(),
        mat.column_indices.data() + mat.column_indices.size());

    if (min_row_index < 1)
      throw fspmv::IOException("found invalid row index (index < 1)");
    if (min_col_index < 1)
      throw fspmv::IOException("found invalid column index (index < 1)");
    if (max_row_index > num_rows)
      throw fspmv::IOException("found invalid row index (index > num_rows)");
    if (max_col_index > num_cols)
      throw fspmv::IOException(
          "found invalid column index (index > num_columns)");
  }

  // convert base-1 indices to base-0
  for (index_type n = 0; n < num_entries; n++) {
    mat.row_indices[n] -= 1;
    mat.column_indices[n] -= 1;
  }

  // expand symmetric formats to "general" format
  if (banner.symmetry != "general") {
    index_type off_diagonals = 0;

    for (index_type n = 0; n < num_entries; n++)
      if (mat.row_indices[n] != mat.column_indices[n]) off_diagonals++;

    index_type general_num_entries = num_entries + off_diagonals;
    CooMatrix general(num_rows, num_cols, general_num_entries);

    if (banner.symmetry == "symmetric") {
      index_type nnz = 0;

      for (index_type n = 0; n < num_entries; n++) {
        // copy entry over
        general.row_indices[nnz]    = mat.row_indices[n];
        general.column_indices[nnz] = mat.column_indices[n];
        general.values[nnz]         = mat.values[n];
        nnz++;

        // duplicate off-diagonals
        if (mat.row_indices[n] != mat.column_indices[n]) {
          general.row_indices[nnz]    = mat.row_indices[n];
          general.column_indices[nnz] = mat.column_indices[n];
          general.values[nnz]         = mat.values[n];
          nnz++;
        }
      }
    } else if (banner.symmetry == "hermitian") {
      throw fspmv::NotImplementedException(
          "MatrixMarket I/O does not currently support hermitian matrices");
      // TODO
    } else if (banner.symmetry == "skew-symmetric") {
      // TODO
      throw fspmv::NotImplementedException(
          "MatrixMarket I/O does not currently support skew-symmetric "
          "matrices");
    }

    // store new full matrix
    num_entries = general_num_entries;
    mat         = general;
  }  // if (banner.symmetry != "general")
  // TODO: sort indices by (row,column)
}

template <typename Stream>
void read_matrix_market_stream(SparseMatrix& mtx, Stream& input) {
  using index_type = typename SparseMatrix::index_type;
  using value_type = typename SparseMatrix::value_type;

  // read banner
  matrix_market_banner banner;
  read_matrix_market_banner(banner, input);

  if (banner.storage == "coordinate") {
    CooMatrix temp;
    read_coordinate_stream(temp, input, banner);
    // TODO
    fspmv::convert(temp, mtx);

  } else  // banner.storage == "array"
  {
    throw fspmv::NotImplementedException(
        "MatrixMarket I/O does not currently support array stream");
  }
}
}  // namespace Impl

void read_matrix(SparseMatrix& mtx, const std::string& filename) {
  std::ifstream file(filename.c_str());

  if (!file)
    throw fspmv::IOException(std::string("unable to open file \"") + filename +
                             std::string("\" for reading"));

#ifdef __APPLE__
  // WAR OSX-specific issue using rdbuf
  std::stringstream file_string(std::stringstream::in | std::stringstream::out);
  std::vector<char> buffer(
      file.rdbuf()->pubseekoff(0, std::ios::end, std::ios::in));
  file.rdbuf()->pubseekpos(0, std::ios::in);
  file.rdbuf()->sgetn(&buffer[0], buffer.size());
  file_string.write(&buffer[0], buffer.size());

  Impl::read_matrix_market_stream(mtx, file_string);
#else
  Impl::read_matrix_market_stream(mtx, file);
#endif
}
}  // namespace fspmv