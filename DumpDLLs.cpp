#include "DumpDLLs.h"

PIMAGE_SECTION_HEADER getEnclosingSectionHeader(long long rva,
	PIMAGE_NT_HEADERS pNTHeader) {

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);

	for (size_t i = 0; i != pNTHeader->FileHeader.NumberOfSections; ++i, ++section) {
		// Is the RVA within this section?
		if ((rva >= section->VirtualAddress) &&
			(rva < (section->VirtualAddress + section->Misc.VirtualSize)))
			return section;
	}

	return 0;
}

LPVOID getPtrFromRVA(long long rva, PIMAGE_NT_HEADERS pNTHeader, long long imageBase) {
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;

	pSectionHdr = getEnclosingSectionHeader(rva, pNTHeader);
	if (!pSectionHdr) return 0;

	delta = static_cast<INT>(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
	return reinterpret_cast<PVOID>(imageBase + rva - delta);
}

void DumpImportsSection(long long base, PIMAGE_NT_HEADERS pNTHeader,
	std::vector<std::string>& result) {
	PIMAGE_IMPORT_DESCRIPTOR importDesc;
	long long importsStartRVA;

	// Get RVA of the import section
	importsStartRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	if (!importsStartRVA) return;

	// Convert RVA to physical memory pointer
	importDesc = static_cast<PIMAGE_IMPORT_DESCRIPTOR>(getPtrFromRVA(importsStartRVA, pNTHeader, base));
	if (!importDesc) return;

	// Write DLL's name's until section is empty
	for (; !((importDesc->TimeDateStamp == 0) && (importDesc->Name == 0)); ++importDesc)
		result.emplace_back(std::string(static_cast<const char*>(getPtrFromRVA(importDesc->Name, pNTHeader, base))));
}

void DumpExeFile(PIMAGE_DOS_HEADER dosHeader, std::vector<std::string>& result) {
	// Verifies validity of the .PE file and calls imports section dump

	PIMAGE_NT_HEADERS pNTHeader;
	long long base = reinterpret_cast<long long>(dosHeader);

	// Safe pointer arifmetics
	pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<long long>(dosHeader)
		+ static_cast<long long>(dosHeader->e_lfanew));

	__try {
		if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
			throw std::exception("Not a Portable Executable .EXE!\n");

	}
	__except (TRUE) {  // If Access Violation error occurs
		throw std::exception("Corrupted .EXE!\n");
	}
	DumpImportsSection(base, pNTHeader, result);
}

std::vector<std::string> getDllList(std::string file_name) {
	// Performs all necessary verifications and calls .PE file dump 

	std::vector<std::string> result;
	HANDLE hFile;
	HANDLE hFileMapping;
	LPVOID lpFileBase;
	PIMAGE_DOS_HEADER dosHeader;

	hFile = CreateFileA(file_name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		throw std::exception("Couldn't open file with CreateFileA()!\n");

	hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);

	if (hFileMapping == 0)
		throw std::exception("Couldn't open file mapping with CreateFileMapping()!\n");

	lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMapping);

	if (lpFileBase == 0)
		throw std::exception("Couldn't map view of file with MapViewOfFile()!\n");

	dosHeader = static_cast<PIMAGE_DOS_HEADER>(lpFileBase);

	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) DumpExeFile(dosHeader, result);
	else throw std::exception("Invalid DOS Header!");

	UnmapViewOfFile(lpFileBase);
	return result;
}
