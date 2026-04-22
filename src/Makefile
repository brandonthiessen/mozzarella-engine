CXX = g++

# Common flags
CXXFLAGS_COMMON = -Wall -Wextra -Werror

# Build configurations
CXXFLAGS_DEBUG = -g -O0
CXXFLAGS_RELEASE = -O3 -march=native -DNDEBUG

BUILD ?= release

ifeq ($(BUILD),debug)
	CXXFLAGS = $(CXXFLAGS_COMMON) $(CXXFLAGS_DEBUG)
else
	CXXFLAGS = $(CXXFLAGS_COMMON) $(CXXFLAGS_RELEASE)
endif


# Core objects
CORE_OBJS = evaluation.o move_generation.o move.o perft.o piece.o position.o search.o timer.o tt.o uci.o utils.o

# Phony targets
.PHONY: all debug release clean bench

# Default build
all: mozzarella bench


# Main executable
mozzarella: mozzarella.o utils.o ${CORE_OBJS}
	$(CXX) ${CXXFLAGS} $^ -o $@


# Benchmarking targets
bench: ../tests/movegen_bench ../tests/search_bench ../tests/perft

../tests/%: ../tests/%.o ${CORE_OBJS}
	$(CXX) ${CXXFLAGS} $^ -o $@

../tests/%.o: ../tests/%.cpp
	$(CXX) ${CXXFLAGS} -I.. -c $< -o $@


# Object rule
%.o: %.cpp castling_rights.h evaluation.h move_generation.h move.h perft.h piece.h player.h position.h search.h timer.h tt.h uci.h utils.h
	$(CXX) ${CXXFLAGS} -c $<


# Build modes
debug:
	$(MAKE) BUILD=debug all

release:
	$(MAKE) BUILD=release all


# Cleanup
clean:
	rm -f *.o mozzarella ../tests/movegen_bench ../tests/search_bench ../tests/perft
