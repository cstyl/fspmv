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

#ifndef FSPMV_EXCEPTIONS_HPP
#define FSPMV_EXCEPTIONS_HPP

#include <string>
#include <stdexcept>

namespace fspmv {

class Exception : public std::exception {
 public:
  Exception(const Exception& exception_) : message(exception_.message) {}
  Exception(const std::string& message_) : message(message_) {}
  ~Exception() throw() {}
  const char* what() const throw() { return message.c_str(); }

 protected:
  std::string message;
};

class NotImplementedException : public Exception {
 public:
  template <typename MessageType>
  NotImplementedException(const MessageType& fn_name)
      : Exception(std::string("NotImplemented: ") + fn_name +
                  std::string(" not yet implemented.")) {}
};

class IOException : public Exception {
 public:
  template <typename MessageType>
  IOException(const MessageType& msg) : Exception(std::string("IO: ") + msg) {}
};

class RuntimeException : public Exception {
 public:
  template <typename MessageType>
  RuntimeException(const MessageType& msg) : Exception(msg) {}
};

class InvalidInputException : public Exception {
 public:
  template <typename MessageType>
  InvalidInputException(const MessageType& msg) : Exception(msg) {}
};

class FormatConversionException : public Exception {
 public:
  template <typename MessageType>
  FormatConversionException(const MessageType& msg) : Exception(msg) {}
};

}  // namespace fspmv

#endif  // FSPMV_EXCEPTIONS_HPP