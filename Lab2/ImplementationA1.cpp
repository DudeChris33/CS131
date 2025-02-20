#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <sstream>
#include <omp.h>
#include <chrono>

#define MAX_INTENSITY 256  // Grayscale intensity levels
#define CHUNK_SIZE 70

// Function to read a PGM file (P2 format - ASCII PGM)
std::vector<unsigned char> readPGM(const std::string &filename, int &width, int &height) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "ERROR: Could not open file " << filename << std::endl;
		return {};
	}

	std::string format;
	file >> format;
	if (format != "P2") {
		std::cerr << "ERROR: Input image is not a valid ASCII PGM (P2) image.\n";
		return {};
	}

	// Skip comments
	std::string line;
	while (file.peek() == '#') std::getline(file, line);

	// Read width and height
	file >> width >> height;

	// Read max intensity (not used, but must be read)
	int maxShades;
	file >> maxShades;

	// Read pixel data
	std::vector<unsigned char> image(width * height);
	for (int i = 0; i < width * height; i++) {
		int pixelVal;
		file >> pixelVal;
		image[i] = static_cast<unsigned char>(pixelVal);
	}

	return image;
}

// Function to compute the histogram sequentially
void computeHistogramSequential(const std::vector<unsigned char> &image, int width, int height, std::array<int, MAX_INTENSITY> &histogram) {
	histogram.fill(0);

	for (int i = 0; i < width * height; i++) {
		histogram[image[i]]++;
	}
}

// Function to compute the histogram in parallel using OpenMP
void computeHistogramParallel(const std::vector<unsigned char> &image, int width, int height, std::array<int, MAX_INTENSITY> &histogram) {
	histogram.fill(0);

	// ADD YOUR CODE HERE
	std::array<int, MAX_INTENSITY> local_histograms[omp_get_max_threads()];
	std::vector<std::pair<int, int>> chunk_info;

	for (int i = 0; i < omp_get_max_threads(); i++) {
		local_histograms[i].fill(0);
	}
	
	auto start_time = std::chrono::high_resolution_clock::now();

	#pragma omp parallel for schedule(static, CHUNK_SIZE)
	for (int i = 0; i < width*height; i++) {
		int thread_id = omp_get_thread_num();
		// if (i % (width*height / omp_get_num_threads()) == 0) { // chunk start
		// 	auto chunk_start_time = std::chrono::high_resolution_clock::now();
		// 	#pragma omp critical
		// 	{
		// 		chunk_info.emplace_back(thread_id, i);
				// std::cout << "Thread " << thread_id << " -> Processing Chunk starting at Row " 
				// 		  << i / width << " -> time: " 
				// 		  << std::chrono::duration<double>(chunk_start_time - start_time).count() 
				// 		  << " seconds\n";
			// }
		// }
		local_histograms[thread_id][image[i]]++;
	}

	for (int i = 0; i < MAX_INTENSITY; i++) {
		for (int j = 0; j < omp_get_max_threads(); j++) {
			histogram[i] += local_histograms[j][i];
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Static scheduling time: " << elapsed.count() << " seconds\n";

	// std::cout << "\nThread Execution Summary:\n";
	// for (const auto& entry : chunk_info) {
	// 	std::cout << "Thread " << entry.first << " started processing at pixel index " 
	// 			  << entry.second << "\n";
	// }
}

// Function to print the histogram
void printHistogram(const std::array<int, MAX_INTENSITY> &histogram) {
	for (int i = 0; i < MAX_INTENSITY; i++) {
		std::cout << "Intensity " << i << ": " << histogram[i] << std::endl;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <input_pgm_file>" << std::endl;
		return -1;
	}

	int width, height;
	std::vector<unsigned char> image = readPGM(argv[1], width, height);
	if (image.empty()) {
		std::cerr << "Error reading the image.\n";
		return -1;
	}

	std::array<int, MAX_INTENSITY> histogram;

	std::cout << "Computing histogram sequentially...\n";
	computeHistogramSequential(image, width, height, histogram);
	// printHistogram(histogram);

	std::cout << "\nComputing histogram in parallel...\n";
	computeHistogramParallel(image, width, height, histogram);
	// printHistogram(histogram);

	return 0;
}
