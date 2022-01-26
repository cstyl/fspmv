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

#ifndef FSPMV_CONFIG_HPP
#define FSPMV_CONFIG_HPP

#include <memory>
#include <vector>

typedef int index_type;
typedef float value_type;

template <typename T>
using host_allocator = std::allocator<T>;

template <typename T>
using array_type = std::vector<T, host_allocator<T>>;

#endif  // FSPMV_CONFIG_HPP