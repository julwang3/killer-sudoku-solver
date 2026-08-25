CXX      		:= g++
CXXFLAGS 		:= -std=c++17 -O2 -Wall -Wextra
INCLUDES 		:= -I. -Ithird_party

BUILD_DIR 		:= out
BIN_SUDOKU		:= $(BUILD_DIR)/test_sudoku
BIN_K_SUDOKU	:= $(BUILD_DIR)/test_killer_sudoku

SRCS			:= tests/main.cpp \
        		src/core/solver_json.cpp \
        		src/solver/sudoku_solver.cpp \
        		src/solver/killer_sudoku_solver.cpp

sudoku: $(BIN_SUDOKU)

killer-sudoku: $(BIN_K_SUDOKU)

$(BIN_SUDOKU): $(SRCS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(BIN_SUDOKU)

$(BIN_K_SUDOKU): $(SRCS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(BIN_K_SUDOKU)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
