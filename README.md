# fpga-spmv

### Build and run Serial on Linux

```sh
make arch=Linux
bin/xfspmv /Users/cstyl/Desktop/code/fpga-spmv/data/bcsstk13/bcsstk13.mtx
```

### TODO
- [x] Source Host
  - [x] Data Structures (Mat, Vec)
  - [x] Read mtx (matrix market format)
    - [x] convert coo->csr
  - [x] SpMV 
- [x] Run main
- [x] Build with Make
- [ ] Setup Code for FPGA
  - [ ] Write OCL Setup Code
    - [ ] Update Make to build with OCL
  - [ ] Write FPGA Host SpMV
  - [ ] Write SpMV Kernel
    - [ ] Update Make to build with Xilinx
  - [ ] Write data movements
  - [ ] Check results against Serial
- [ ] Add timers

