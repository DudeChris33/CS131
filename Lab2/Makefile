# FILES := ImplementationA1.cpp # Remember that you need to submit a file for each of the 2 implementations! Change the name in between!
FILES := ImplementationA2.cpp
OUTPUT_FILE := output.o
ARGS := -Wall -std=c++11 -fopenmp
# INPUT_FILE := tc1.pgm # Name of the image being taken as input
# INPUT_FILE := tc2.pgm
# INPUT_FILE := tc3.pgm
# INPUT_FILE := tc4.pgm
INPUT_FILE := tc5.pgm
# You can set the input file name manually, e.g. "make run INPUT_FILE=tc2.pgm"

# Make sure to set these accordingly to what you need!
NUM_THREADS := 8
CPU_AFFINITY := 0-15

build: $(FILES)
	g++ $(ARGS) $(FILES) -o $(OUTPUT_FILE)

run: build $(INPUT_FILE)
	OMP_NUM_THREADS=$(NUM_THREADS) GOMP_CPU_AFFINITY="$(CPU_AFFINITY)" ./$(OUTPUT_FILE) $(INPUT_FILE)

build_datarace: $(FILES)
	g++ $(ARGS) $(FILES) -o $(OUTPUT_FILE) -fsanitize=thread -fPIE -pie -g

test_datarace: build_datarace $(INPUT_FILE)
	OMP_NUM_THREADS=$(NUM_THREADS) GOMP_CPU_AFFINITY=$(CPU_AFFINITY) ./$(OUTPUT_FILE) $(INPUT_FILE)

clean: $(OUTPUT_FILE)
	rm -f $(OUTPUT_FILE)