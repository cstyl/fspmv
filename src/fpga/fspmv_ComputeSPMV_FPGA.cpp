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

#include "fspmv_ComputeSPMV_FPGA.hpp"
#include "fspmv_Config.hpp"

#if defined(FSPMV_ENABLE_XILINX)

#include "xilinx/xcl/xcl.hpp"

namespace fspmv{

void spmv_fpga(const SparseMatrix& A, const Vector& x, Vector& y){
  
  assert(x.length>=A.ncols); // Test vector lengths
  assert(y.length>=A.nrows);
  
  const int value_size = sizeof(fspmv_value_type);
  const int index_size = sizeof(fspmv_index_type);

  cl_int err;
  cl::CommandQueue q;
  std::string krnl_name = "spmv_kernel";
  cl::Kernel kernel;
  cl::Context context;

  // OPENCL HOST CODE AREA START
  // The get_xil_devices will return vector of Xilinx Devices
  auto devices = xcl::get_xil_devices();

  // read_binary_file() command will find the OpenCL binary file created using the
  // V++ compiler load into OpenCL Binary and return pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;

  for (unsigned int i = 0; i < devices.size(); i++) {//edit here to skip trying to program device zero
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, 
                  q = cl::CommandQueue(context, 
                                       device, 
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | 
                                       CL_QUEUE_PROFILING_ENABLE, 
                                       &err));

    cl::Program program(context, {device}, bins, NULL, &err);

    if (err == CL_SUCCESS) {
      // Creating Kernel object using Compute unit names
      OCL_CHECK(err, kernel = cl::Kernel(program, krnl_name.c_str(), &err));
    
      valid_device++;
      break; // we break because we found a valid device
    }
  }
   
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }
  
  // Allocate the buffers:
  OCL_CHECK(err, cl::Buffer buffer_roff(context, 
                                        CL_MEM_READ_ONLY, 
                                        A.row_offsets.size() * index_size, 
                                        NULL, &err)); // A.row_offsets.data)
  OCL_CHECK(err, cl::Buffer buffer_cind(context, 
                                        CL_MEM_READ_ONLY, 
                                        A.column_indices.size() * index_size, 
                                        NULL, &err)); // A.column_indices.data)
  OCL_CHECK(err, cl::Buffer buffer_vals(context, 
                                        CL_MEM_READ_ONLY, 
                                        A.values.size() * value_size, 
                                        NULL, &err)); // A.values.data)

  OCL_CHECK(err, cl::Buffer buffer_xval(context, 
                                        CL_MEM_READ_ONLY, 
                                        x.values.size() * value_size, 
                                        NULL, &err)); // x.values.data)
  OCL_CHECK(err, cl::Buffer buffer_yval(context, 
                                        CL_MEM_READ_WRITE, 
                                        y.values.size() * value_size, 
                                        NULL, &err)); // y.values.data)

  // Set kernel arguments:
  OCL_CHECK(err, err = kernel.setArg(0, buffer_roff));
  OCL_CHECK(err, err = kernel.setArg(1, buffer_cind));
  OCL_CHECK(err, err = kernel.setArg(2, buffer_vals));
  OCL_CHECK(err, err = kernel.setArg(3, buffer_xval));
  OCL_CHECK(err, err = kernel.setArg(4, buffer_yval));
  OCL_CHECK(err, err = kernel.setArg(5, A.nrows));
  OCL_CHECK(err, err = kernel.setArg(6, A.ncols));
  OCL_CHECK(err, err = kernel.setArg(7, A.nnnz));

  // Initialize the buffers:
  cl::Event event;

  OCL_CHECK(err, q.enqueueWriteBuffer(buffer_roff,                        // buffer on the FPGA
                                      CL_TRUE,                            // blocking call
                                      0,                                  // buffer offset in bytes
                                      A.row_offsets.size() * index_size,  // Size in bytes
                                      A.row_offsets.data(),               // Pointer to the data to copy
                                      nullptr, &event));
  OCL_CHECK(err, q.enqueueWriteBuffer(buffer_cind,                            // buffer on the FPGA
                                      CL_TRUE,                                // blocking call
                                      0,                                      // buffer offset in bytes
                                      A.column_indices.size() * index_size,   // Size in bytes
                                      A.column_indices.data(),                // Pointer to the data to copy
                                      nullptr, &event));
  OCL_CHECK(err, q.enqueueWriteBuffer(buffer_vals,                        // buffer on the FPGA
                                      CL_TRUE,                            // blocking call
                                      0,                                  // buffer offset in bytes
                                      A.values.size() * value_size,       // Size in bytes
                                      A.values.data(),                    // Pointer to the data to copy
                                      nullptr, &event));

  OCL_CHECK(err, q.enqueueWriteBuffer(buffer_xval,                        // buffer on the FPGA
                                      CL_TRUE,                            // blocking call
                                      0,                                  // buffer offset in bytes
                                      x.values.size() * value_size,       // Size in bytes
                                      A.row_offsets.data(),               // Pointer to the data to copy
                                      nullptr, &event));  
  OCL_CHECK(err, q.enqueueWriteBuffer(buffer_yval,                        // buffer on the FPGA
                                      CL_TRUE,                            // blocking call
                                      0,                                  // buffer offset in bytes
                                      y.values.size() * value_size,       // Size in bytes
                                      y.values.data(),                    // Pointer to the data to copy
                                      nullptr, &event));

  q.finish();

  // Profiling Objects
  cl_ulong start = 0;
  cl_ulong end = 0;
  cl::Event event_sp;

  // Execute the kernel:
  OCL_CHECK(err, err = q.enqueueTask(kernel, NULL, &event_sp));

  clWaitForEvents(1, (const cl_event*)&event_sp);
  event_sp.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
  event_sp.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
  std::cout << "FPGA SPMV Kernel took: " << ((end - start) / 1000000) << "ms" << std::endl;

  // Copy Result from Device Global Memory to Host Local Memory
  q.enqueueReadBuffer(buffer_yval,                  // This buffers data will be read
                      CL_TRUE,                      // blocking call
                      0,                            // offset
                      y.values.size() * value_size),
                      y.values.data(),              // Data will be stored here
                      nullptr, &event_sp);

  q.finish();
}
}

#endif
