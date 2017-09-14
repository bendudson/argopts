
GTEST_DIR = googletest/googletest
GTEST_SOURCES = $(GTEST_DIR)/src/gtest-all.cc

CXX = g++

CXXFLAGS += -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -I.

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++11

check: tests
	./tests

tests: gtest-all.o test_argopts.cxx argopts.hxx
	$(CXX) -o $@ test_argopts.cxx gtest-all.o $(CXXFLAGS)

gtest-all.o: googletest/README.md
	$(CXX) -c $(GTEST_SOURCES) -o $@ $(CXXFLAGS)

# Download and compile the google test
googletest/README.md:
	cd googletest
	git submodule update --init --recursive
	cd -



