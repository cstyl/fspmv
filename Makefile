# -*- Makefile -*-
#######################################################################################
.PHONY: help
help:
	@echo "Makefile Usage:"
	@echo "  make device TARGET=<sw_emu/hw_emu/hw>"
	@echo "      Command to generate the design for specified Target and Device."
	@echo ""
	@echo "  make host "
	@echo "      Command to generate host."
	@echo ""
	@echo "  make clean "
	@echo "      Command to remove the generated files."
	@echo ""
#######################################################################################

arch = UNKNOWN
setup_file = setup/Make.$(arch)

include $(setup_file)

FSPMV_PATH = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
SRCdir = $(FSPMV_PATH)/src

WITH_XILINX ?= false

ifeq ($(WITH_XILINX),true)
XILINX_VITIS  ?= /opt/Xilinx/Vitis/2021.1
XILINX_XRT    ?= /opt/xilinx/xrt
XILINX_VIVADO ?= /opt/Xilinx/Vivado/2021.1

TARGET ?= sw_emu
PLATFORM ?= xilinx_aws-vu9p-f1_shell-v04261818_201920_2

VXX ?= $(XILINX_VITIS)/bin/v++
VXXFLAGS += --target $(TARGET)	

ifeq ($(TARGET),sw_emu)
CXXFLAGS += -DSW_EMU_TEST
endif #($(TARGET), sw_emu)

ifeq ($(TARGET),hw_emu)
CXXFLAGS += -DHW_EMU_TEST
endif #($(TARGET), hw_emu)

CXXFLAGS += -I$(SRCdir)/xilinx/xcl -I$(XILINX_XRT)/include/ -I$(XILINX_VIVADO)/include/ -I$(XILINX_HLS)/include/
LDFLAGS += -L$(XILINX_XRT)/lib -lOpenCL

DEVICE_SRCdir  = $(SRCdir)/fpga
DEVICE_OBJECTS = fspmv_ComputeSPMV_FPGA.xo
OBJECTS				 = xilinx/xcl/xcl.o
XCLBIN 			   = fspmv.$(TARGET).xclbin
endif #($(WITH_XILINX), true)

# Look for headers in here
VPATH = src

#Include Libraries
LDFLAGS += -lstdc++ 
OBJECTS  += fspmv_ComputeSPMV.o \
						fspmv_Convert.o \
	 					fspmv_MatrixMarket.o

build: $(XCLBIN) $(EXE) emconfig

# create necessary directories
.PHONY: mkrefdir
mkrefdir:
	@mkdir -p bin build 
ifeq ($(WITH_XILINX),true)
	@mkdir -p	build_$(TARGET)
endif

.PHONY: clean
clean:
	rm -rf build bin/fspmv build_$(TARGET)

.PHONY: device
.ONESHELL:
device: mkrefdir $(DEVICE_OBJECTS) $(XCLBIN)
	  cp build_$(TARGET)/$(XCLBIN) bin/.

#
# Host Rules
#
host: mkrefdir main.o $(OBJECTS)
	cd build; $(LINKER) $(LINKFLAGS) main.o $(OBJECTS) $(LIBS) -o $(FSPMV_PATH)/bin/xfspmv $(LDFLAGS)

%.o: $(SRCdir)/%.cpp
	cd build; $(CXX) -c $(CXXFLAGS) -I$(SRCdir) $< -o $@

#
# Kernel Rules
#
$(XCLBIN): $(DEVICE_OBJECTS)
	cd build_$(TARGET); $(VLINKER) $(VLINKFLAGS) --link -o $@

.PHONY: $(DEVICE_OBJECTS)
$(DEVICE_OBJECTS): %.xo : $(DEVICE_SRCdir)/%.cpp
	cd build_$(TARGET); $(VXX) $(VXXFLAGS) --compile -Isrc/fpga $< -o $@

.PHONY: emconfig
emconfig:
	mkdir -p bin
	cd bin
ifeq ($(WITH_XILINX),true)
		emconfigutil --platform $(PLATFORM)
endif
