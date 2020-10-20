# 使用默认
#CXX = clang++
#CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow

INC_PATH = -I../basic/spdlog/include/ \
           -I.

LIB_PATH = 


#LDFLAGS = -lGL -lglut -lpng -lz -lm
CXX_FLAGS = -std=c++11 -pthread -Wall -g
LDFLAGS = -lpthread -liconv

# Final binary
BIN = main
LIB = libunicode_str.a
# Put all auto generated stuff to this build dir.
BUILD_DIR = ./build

# List of all .cpp source files.
CPP = $(wildcard *.cpp)

# All .o files go to build dir.
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)

# Actual target of the binary - depends on all .o files.
$(LIB) : $(OBJ)
	# Create build directories - same structure as sources.
	mkdir -p $(@D)
	ar -r $@ $^

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
	# The -MMD flags additionaly creates a .d file with
	# the same name as the .o file.
	$(CXX) $(CXX_FLAGS) $(INC_PATH) -MMD -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BUILD_DIR) $(LIB)
