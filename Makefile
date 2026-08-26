CXX      	:= g++
CXXFLAGS 	:= -std=c++17 -O2 -Wall -Wextra
INCLUDES 	:= -I. -Ithird_party

BUILD_DIR 	:= out
BIN       	:= $(BUILD_DIR)/test

SRCS		:= tests/main.cpp \
       		src/core/solver_json.cpp \
        	src/solver/sudoku_solver.cpp \
        	src/solver/killer_sudoku_solver.cpp

build: $(BIN)

$(BIN): $(SRCS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
