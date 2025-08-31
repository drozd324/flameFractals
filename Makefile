KOKKOS_PATH := $(shell spack location -i kokkos)

CXX = g++
CXXFLAGS = -O3 -std=c++20 #-fopenmp -I$(KOKKOS_PATH)/include
LDFLAGS = -lSDL2 -lSDL2_image # -L$(KOKKOS_PATH)/lib -lkokkoscore 
DEBUG = #-g -fsanitize=address,undefined -Wall -Wextra
MAIN=main

OBJDIR := obj
BINDIR := bin
SRCDIR := src
DATADIR := data

EXEC_NAMES := main
EXECS := $(addprefix $(BINDIR)/, $(EXEC_NAMES))

SOURCES := main.cpp ran.cpp
ALL_OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SOURCES)))

all: $(EXECS) $(OBJDIR) $(BINDIR) $(DATADIR)

$(BINDIR)/main: $(ALL_OBJS)   | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(DEBUG) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@ $(LDFLAGS)

$(OBJDIR)/ran.o: $(SRCDIR)/ran.cpp

$(OBJDIR) $(BINDIR) $(DATADIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(EXECS) $(DATDIR)/*.csv $(DATDIR)/*.txt

