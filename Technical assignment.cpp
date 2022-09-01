#include <iostream>
#include <fstream>
#include <future>
#include "DumpDLLs.h"
#include "ChangeIcon.h"

void getFile(std::ifstream& file, std::string& path_to_file, const std::string& extension) {
	/* Asks for the path to the file and opens it in binary mode*/
	do {
		std::cout << "Enter the " << extension << " file location (without quotes) : ";
		std::getline(std::cin, path_to_file);
		file.open(path_to_file, std::ios::binary | std::ios::in);
	} while (!file.is_open() && std::cerr << "Invalid file location!\n");
}

std::mutex mtx;

void readPart(size_t pos, size_t bytes_per_section, std::string path_to_file,
	std::vector<size_t>& byte_counter) {

	std::ifstream file(path_to_file, std::ios::binary | std::ios::in);
	file.seekg(pos);
	char byte;
	for (; bytes_per_section != 0; --bytes_per_section) {
		file.read(&byte, 1);
		std::lock_guard<std::mutex> lock(mtx);
		byte_counter[static_cast<uint_fast8_t>(byte)] ++;
	}
	file.close();
}

double calculateEntropy(std::string path_to_file, size_t& file_size) {
	/* Calculates Shannon's entropy of a binary file */
	double entropy = 0;

	std::vector<size_t> byte_counter(256, 0);

	std::ifstream file(path_to_file, std::ios::binary | std::ios::in);
	file.seekg(0, std::ios_base::end);
	file_size = file.tellg(); 
	file.seekg(0, std::ios_base::beg);

	const auto processor_count = std::thread::hardware_concurrency();

	size_t pos = 0;
	size_t bytes_per_section = file_size / processor_count;

	std::vector<std::thread> thread_pool;
	thread_pool.reserve(processor_count);

	for (size_t i = 0; i != processor_count; ++i, pos += bytes_per_section) {
		thread_pool.emplace_back(readPart, pos, bytes_per_section, path_to_file, ref(byte_counter));
	}

	for (auto& thread : thread_pool)
		if (thread.joinable()) thread.join();

	// read remaining bytes 
	file.seekg(pos);
	char byte;
	while (!file.eof()) {
		file.read(&byte, 1);
		byte_counter[static_cast<uint_fast8_t>(byte)] ++;
	}

	for (auto bc : byte_counter) {
		if (bc == 0) continue;
		double p = (1.0 * bc) / file_size;
		entropy -= p * log2(p);
	}

	file.close();
	return entropy / 8.0; // because we must have been taking the log with 256 base
}

size_t calculateWinApiW(const std::vector<std::string>& dlls) {
	size_t counter = 0;
	for (const auto& name : dlls) {
		// with respect to the register*
		if (name.find('W') != std::string::npos &&
			LoadLibraryA(name.c_str())) counter++;
	}
	return counter;
}

int main() {

	std::ifstream file_exe; std::string path_to_exe; getFile(file_exe, path_to_exe, ".EXE");
	std::ifstream file_ico; std::string path_to_ico; getFile(file_ico, path_to_ico, ".ICO");

	size_t exe_size, ico_size;
	double exe_entropy = calculateEntropy(path_to_exe, exe_size); file_exe.close();
	double ico_entropy = calculateEntropy(path_to_ico, ico_size); file_ico.close();

	std::cout << "\n.Exe file entropy: " << exe_entropy << '\n';
	std::cout << ".Ico file entropy: " << ico_entropy << '\n';
	
	try {
		std::vector<std::string> dlls = getDllList(path_to_exe);
		std::cout << "\nIncluded DLL's:" << '\n' << '\n';
		for (const auto& dll : dlls) {
			std::cout << '\t' << dll << "\n";
		}
		std::cout << "\nNumber of WinApi's with W letter in name: ";
		std::cout << calculateWinApiW(dlls) << '\n';
	}
	catch (const std::exception& e) {
		std::cerr << e.what();
	}

	changeIco(path_to_exe, path_to_ico, ico_size);
	
	return 0;
}
