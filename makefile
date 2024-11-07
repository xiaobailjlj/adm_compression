# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Target executable
TARGET = compression_program

# Source files
SRCS = compression.cpp technique_bin.cpp technique_for.cpp technique_dif.cpp compress_dic.cpp technique_rle.cpp
# SRCS = compression.cpp technique_bin.cpp technique_rle.cpp technique_dic.cpp technique_for.cpp technique_dif.cpp

# Object files (replace .cpp with .o for each source file)
OBJS = $(SRCS:.cpp=.o)

# Default target to build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule for compiling .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
