#include "ChangeIcon.h"
#include <Windows.h>
#include <fstream>

#pragma pack(push, 2)

typedef struct    
{
    WORD _id_reserved;
    WORD _id_type;
    WORD _id_count;
    BYTE _width;
    BYTE _height;
    BYTE _color_count;
    BYTE _reserved;
    WORD _planes;
    WORD _bit_count;
    DWORD _bytes_in_resource;
    DWORD _image_offset;
} IconEntry;

#pragma pack(pop)

void changeIco(const std::string& path_to_exe, 
               const std::string& path_to_ico, const size_t& ico_size) {

    HANDLE hUpdate = BeginUpdateResourceA(path_to_exe.c_str(), FALSE);
    
    if (!hUpdate) return;

    char* buffer = new char[ico_size];
    std::ifstream icon(path_to_ico, std::ios::binary | std::ios::in);
    icon.read(buffer, ico_size);
    icon.close();
    
    UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(1), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), buffer + 22, ico_size - 22);
    IconEntry icon_entry;
    
    icon_entry._id_reserved = 0;
    icon_entry._id_type = 1;
    icon_entry._id_count = 1;
    icon_entry._width = 32;
    icon_entry._height = 32;
    icon_entry._color_count = 0;
    icon_entry._reserved = 0;
    icon_entry._bit_count = 32;
    icon_entry._bytes_in_resource = ico_size - 22;
    icon_entry._image_offset = 1;

    UpdateResource(hUpdate, RT_GROUP_ICON, L"MAINICON", MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), &icon_entry, sizeof(icon_entry));

    EndUpdateResource(hUpdate, FALSE);

    delete[] buffer;
}
