#pragma once
#include "stdafx.h"
#include <vector>
using namespace std;

//打印函数
void PrintHex(const void* ptr, const int count, int countperline = 16);
bool RVA2FOA(const int& rva,int& foa);
bool FOA2RVA(const int& foa, int& rva);

char* m_file_buffer = NULL;
char* m_image_buffer= NULL;

const int NT_FLAG_COUNT = 4;
int m_file_length = 0;

//DOS头
PIMAGE_DOS_HEADER m_dos_header = NULL;
//NT头
PIMAGE_NT_HEADERS m_nt_header;
//标准PE头
PIMAGE_FILE_HEADER m_image_file_header = NULL;
//可选PE头
PIMAGE_OPTIONAL_HEADER m_image_option_header = NULL;
//节表
std::vector<PIMAGE_SECTION_HEADER> m_section_table;

//将PE文件读入内存
extern "C" _declspec(dllexport) bool ReadPEFile(const char* file_name);
void ReadDosHeader();
void PrintDosHeader();
void ReadNTHeader();
void PrintNTHeader();
void ReadFilePEHeader();
void PrintFilePEHeader();
void ReadOptionalPEHeader();
void PrintOptionalPEHeader();
void ReadSectionTable();
void PrintOptionalPEHeader();

//将FileBuffer拉伸为ImageBuffer
extern "C" _declspec(dllexport) bool FileBuffer2ImageBuffer();

//将ImageBuffer收缩为FileBuffer
extern "C" _declspec(dllexport) bool ImageBuffer2FileBuffer();

//向代码节空白区增加代码


//新增一个节


//扩大节


//合并节



