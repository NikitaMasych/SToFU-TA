#pragma once
#include <Windows.h>
#include <vector>
#include <string>

// Finds section in which is specified RVA
PIMAGE_SECTION_HEADER getEnclosingSectionHeader(long long rva, 
	PIMAGE_NT_HEADERS pNTHeader);

// Converts RVA to physical memory adress 
LPVOID getPtrFromRVA(long long rva, PIMAGE_NT_HEADERS pNTHeader, long long imageBase);

// Writes all import DLL's in the vector result
void DumpImportsSection(long long base, PIMAGE_NT_HEADERS pNTHeader,
	std::vector<std::string>& result);

// Verifies validity of the .PE file and calls imports section dump
void DumpExeFile(PIMAGE_DOS_HEADER dosHeader, std::vector<std::string>& result);

// Performs all necessary verifications and calls .PE file dump
std::vector<std::string> getDllList(std::string file_name);
