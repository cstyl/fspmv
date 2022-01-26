# -*- Makefile -*-

arch = UNKNOWN
setup_file = setup/Make.$(arch)

include $(setup_file)

# Look for headers in here
VPATH = src

#Include Libraries
LDFLAGS += -lstdc++ 

fspmv_DEPS = 	src/fspmv_ComputeSPMV.o \
				src/fspmv_Convert.o \
				src/fspmv_MatrixMarket.o

all: dir bin/xfspmv

# create necessary directories
dir:
	@mkdir -p bin

bin/xfspmv: src/main.o $(fspmv_DEPS)
	$(LINKER) $(LINKFLAGS) src/main.o $(fspmv_DEPS) $(fspmvLIBS) -o bin/xfspmv $(LDFLAGS)

clean:
	rm -f src/*.o bin/xfspmv

.PHONY: all clean dir

src/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -Isrc $< -o $@


