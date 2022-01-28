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

#ifndef FSPMV_VECTOR_HPP
#define FSPMV_VECTOR_HPP

#include "fspmv_Config.hpp"
#include <vector>

namespace fspmv {
class Vector {
 public:
  using value_type       = fspmv_value_type;
  using index_type       = std::size_t;
  using value_array_type = std::vector<value_type, host_allocator<value_type>>;

  index_type length;
  value_array_type values;

  // Construct empty Vector
  inline Vector() : length(0), values(0) {}

  // Construct with a specific length
  inline Vector(const index_type len, const value_type value)
      : length(len), values(len, value) {}

  inline void resize(const index_type len) {
    values.resize(len);
    length = len;
  }

  inline void assign(const index_type len, const value_type value) {
    resize(len);
    values.assign(len, value); 
  }

  inline void random(const index_type len){
    resize(len);

    for(index_type i = 0; i < length; i++){
      values[i] = -1 +std::rand() % 2;
    }
  }
};

}  // namespace fspmv

#endif  // FSPMV_VECTOR_HPP
