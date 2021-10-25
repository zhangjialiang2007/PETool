#pragma once
#include "stdafx.h"
#include <vector>
using namespace std;

//��ӡ����
void PrintHex(const void* ptr, const int count, int countperline = 16);
bool RVA2FOA(const int& rva,int& foa);
bool FOA2RVA(const int& foa, int& rva);

char* m_file_buffer = NULL;
char* m_image_buffer= NULL;

const int NT_FLAG_COUNT = 4;
int m_file_length = 0;

//DOSͷ
PIMAGE_DOS_HEADER m_dos_header = NULL;
//NTͷ
PIMAGE_NT_HEADERS m_nt_header;
//��׼PEͷ
PIMAGE_FILE_HEADER m_image_file_header = NULL;
//��ѡPEͷ
PIMAGE_OPTIONAL_HEADER m_image_option_header = NULL;
//�ڱ�
std::vector<PIMAGE_SECTION_HEADER> m_section_table;

//��PE�ļ������ڴ�
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

//��FileBuffer����ΪImageBuffer
extern "C" _declspec(dllexport) bool FileBuffer2ImageBuffer();

//��ImageBuffer����ΪFileBuffer
extern "C" _declspec(dllexport) bool ImageBuffer2FileBuffer();

//�����ڿհ������Ӵ���


//����һ����


//�����


//�ϲ���



