CXX      	:= g++
CXXFLAGS 	:= -std=c++17 -O2 -Wall -Wextra
INCLUDES 	:= -I. -Ithird_party

BUILD_DIR 	:= out
BIN_SUDOKU	:= $(BUILD_DIR)/test_sudoku

SRCS_SUDOKU := tests/main.cpp \
        	src/core/solver_json.cpp \
        	src/solver/sudoku_solver.cpp

build: $(BIN_SUDOKU)

$(BIN_SUDOKU): $(SRCS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS_SUDOKU) -o $(BIN_SUDOKU)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
