# compiler variables and flags
CXX=g++
# -02 for opt
CXXFLAGS=-Wall -g -std=c++11
# command variabels
RM=rm
RMFLAGS=-rf

# directories
BUILD_DIR=build
INCL_DIR=include

MAIN_BUILD_DIR=$(BUILD_DIR)/main
OBJ_DIR=$(MAIN_BUILD_DIR)/obj
BIN_DIR=$(MAIN_BUILD_DIR)/bin
SRC_DIR=src

# files
SOURCES=$(SRC_DIR)/main.cpp
SRC_OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

INCLUDES=$(INCL_DIR)/qtest.hpp

# targets
MAIN_TARGET=$(BIN_DIR)/main

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(SRC_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCL_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCL_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCL_FLAGS) -o $@ -c $<

clean:
	$(RM) $(RMFLAGS) ./$(BUILD_DIR)

# @ suppresses the command output (but not the program's output)
run:
	@./$(MAIN_TARGET)

leaks:
	@lldb ./$(MAIN_TARGET)

macros: $(SRC_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -E $(INCL_FLAGS) -o $@ $^

# create the .o and main executable directories as well as the
#  test binary directory if they don't exist.
$(BUILD_DIR) $(TEST_BUILD_DIR) $(MAIN_BUILD_DIR) $(OBJ_DIR) $(BIN_DIR) $(TEST_OBJ_DIR) $(TEST_BIN_DIR) $(INCL_DIR):
	@mkdir -p $@

.PHONY: clean all run leaks