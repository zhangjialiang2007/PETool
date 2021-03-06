// XXZ_PETool.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "XXZ_PETool.h"

#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<assert.h>

void PrintHex(const void* ptr, const int count, int countperline)
{
	int index = 0;
	while (index < count)
	{
		BYTE v = *((char*)ptr + index);
		//为了保持美观，如果为0-f之间的数字，则需要补0
		if (v < 0x10)
		{
			printf("0");
		}
		printf("%x ", v);
		index++;
		if (index % countperline == 0)
		{
			printf("\n");
		}
	}
}
bool RVA2FOA(const int& rva, int& foa)
{
	if (rva > m_image_option_header->SizeOfImage)
	{
		return false;
	}

	if (rva < m_image_option_header->SizeOfHeaders)
	{
		foa = rva;
		return true;
	}
	int index = 0;
	while (index < m_section_table.size())
	{
		int point = rva - m_section_table[index]->VirtualAddress;
		if (point < 0)
		{
			break;
		}
		index++;
	}
	foa = rva - m_section_table[index - 1]->VirtualAddress + m_section_table[index - 1]->PointerToRawData;
	return true;
}
bool FOA2RVA(const int& foa, int& rva)
{
	if (foa > m_file_length)
	{
		return false;
	}

	if (foa < m_image_option_header->SizeOfHeaders)
	{
		rva = foa;
		return true;
	}
	int index = 0;
	while (index < m_section_table.size())
	{
		int point = foa - m_section_table[index]->PointerToRawData;
		if (point < 0)
		{
			break;
		}
		index++;
	}
	rva = foa - m_section_table[index - 1]->PointerToRawData + m_section_table[index - 1]->VirtualAddress;
	return true;
}

bool ReadPEFile(const char* file_name)
{
	FILE *fp = fopen(file_name, "rb");
	fseek(fp, 0, SEEK_END);
	m_file_length = ftell(fp);
	rewind(fp);
	m_file_buffer = (char*)malloc(m_file_length * sizeof(char));
	memset(m_file_buffer, 0, m_file_length);
	fread(m_file_buffer, m_file_length, 1, fp);

	ReadDosHeader();	
	ReadNTHeader();
	PrintOptionalPEHeader();
	ReadSectionTable();

	FileBuffer2ImageBuffer();

	PrintHex(m_image_buffer, m_image_option_header->SizeOfImage);
	getchar();
	return true;
}
void ReadDosHeader()
{
	m_dos_header = new _IMAGE_DOS_HEADER();
	m_dos_header->e_magic = *((WORD*)m_file_buffer);
	m_dos_header->e_cblp = *((WORD*)(m_file_buffer+2));
	m_dos_header->e_cp = *((WORD*)(m_file_buffer + 4));
	m_dos_header->e_crlc = *((WORD*)(m_file_buffer + 6));
	m_dos_header->e_cparhdr = *((WORD*)(m_file_buffer + 8));
	m_dos_header->e_minalloc = *((WORD*)(m_file_buffer + 10));
	m_dos_header->e_maxalloc = *((WORD*)(m_file_buffer + 12));
	m_dos_header->e_ss = *((WORD*)(m_file_buffer + 14));
	m_dos_header->e_sp = *((WORD*)(m_file_buffer + 16));
	m_dos_header->e_csum = *((WORD*)(m_file_buffer + 18));
	m_dos_header->e_ip = *((WORD*)(m_file_buffer + 20));
	m_dos_header->e_cs = *((WORD*)(m_file_buffer + 22));
	m_dos_header->e_lfarlc = *((WORD*)(m_file_buffer + 24));
	m_dos_header->e_ovno = *((WORD*)(m_file_buffer + 26));
	memcpy(m_dos_header->e_res, m_file_buffer + 28, 4 * 2);
	m_dos_header->e_oemid = *((WORD*)(m_file_buffer + 36));
	m_dos_header->e_oeminfo = *((WORD*)(m_file_buffer + 38));
	memcpy(m_dos_header->e_res2, m_file_buffer + 40, 10 * 2);
	m_dos_header->e_lfanew = *((DWORD*)(m_file_buffer + 60));
}
void PrintDosHeader()
{
	printf("------------------DOS Header---------------\n");
	if (m_dos_header == NULL)
	{
		return;
	}
	printf("e_magic = %x \n", m_dos_header->e_magic);
	printf("e_cblp = %x \n", m_dos_header->e_cblp);
	printf("e_cp = %x \n", m_dos_header->e_cp);
	printf("e_crlc = %x \n", m_dos_header->e_crlc);
	printf("e_cparhdr = %x \n", m_dos_header->e_cparhdr);
	printf("e_minalloc = %x \n", m_dos_header->e_minalloc);
	printf("e_maxalloc = %x \n", m_dos_header->e_maxalloc);
	printf("e_ss = %x \n", m_dos_header->e_ss);
	printf("e_sp = %x \n", m_dos_header->e_sp);
	printf("e_csum = %x \n", m_dos_header->e_csum);
	printf("e_ip = %x \n", m_dos_header->e_ip);
	printf("e_cs = %x \n", m_dos_header->e_cs);
	printf("e_lfarlc = %x \n", m_dos_header->e_lfarlc);
	printf("e_ovno = %x \n", m_dos_header->e_ovno);
	printf("e_res[0] = %x \n", m_dos_header->e_res[0]);
	printf("e_res[1] = %x \n", m_dos_header->e_res[1]);
	printf("e_res[2] = %x \n", m_dos_header->e_res[2]);
	printf("e_res[3] = %x \n", m_dos_header->e_res[3]);
	printf("e_oemid = %x \n", m_dos_header->e_oemid);
	printf("e_oeminfo = %x \n", m_dos_header->e_oeminfo);
	printf("e_res2[0] = %x \n", m_dos_header->e_res2[0]);
	printf("e_res2[1] = %x \n", m_dos_header->e_res2[1]);
	printf("e_res2[2] = %x \n", m_dos_header->e_res2[2]);
	printf("e_res2[3] = %x \n", m_dos_header->e_res2[3]);
	printf("e_res2[4] = %x \n", m_dos_header->e_res2[4]);
	printf("e_res2[5] = %x \n", m_dos_header->e_res2[5]);
	printf("e_res2[6] = %x \n", m_dos_header->e_res2[6]);
	printf("e_res2[7] = %x \n", m_dos_header->e_res2[7]);
	printf("e_res2[8] = %x \n", m_dos_header->e_res2[8]);
	printf("e_res2[9] = %x \n", m_dos_header->e_res2[9]);
	printf("e_lfanew = %x \n", m_dos_header->e_lfanew);
}
void ReadNTHeader()
{
	m_nt_header = new IMAGE_NT_HEADERS();
	m_nt_header->Signature = *(DWORD*)(m_file_buffer + m_dos_header->e_lfanew);
	ReadFilePEHeader();
	ReadOptionalPEHeader();
}
void PrintNTHeader()
{
	printf("------------------NT Header---------------\n");
	if (m_nt_header == NULL)
	{
		return;
	}
	printf("m_nt_header_flag = %x \n", m_nt_header->Signature);
	PrintFilePEHeader();
	PrintOptionalPEHeader();
}
void ReadFilePEHeader()
{
	char* temp_ptr = m_file_buffer + m_dos_header->e_lfanew + NT_FLAG_COUNT;
	m_image_file_header = new _IMAGE_FILE_HEADER();
	m_image_file_header->Machine = *((WORD*)temp_ptr);
	m_image_file_header->NumberOfSections = *((WORD*)(temp_ptr + 2));
	m_image_file_header->TimeDateStamp = *((DWORD*)(temp_ptr + 4));
	m_image_file_header->PointerToSymbolTable = *((DWORD*)(temp_ptr + 8));
	m_image_file_header->NumberOfSymbols = *((DWORD*)(temp_ptr + 12));
	m_image_file_header->SizeOfOptionalHeader = *((WORD*)(temp_ptr + 16));
	m_image_file_header->Characteristics = *((WORD*)(temp_ptr + 18));
	m_nt_header->FileHeader = *m_image_file_header;
}
void PrintFilePEHeader()
{
	printf("------------------File PE Header---------------\n");
	if (m_image_file_header == NULL)
	{
		return;
	}
	printf("Machine = %x \n", m_image_file_header->Machine);
	printf("NumberOfSections = %x \n", m_image_file_header->NumberOfSections);
	printf("TimeDateStamp = %x \n", m_image_file_header->TimeDateStamp);
	printf("PointerToSymbolTable = %x \n", m_image_file_header->PointerToSymbolTable);
	printf("NumberOfSymbols = %x \n", m_image_file_header->NumberOfSymbols);
	printf("SizeOfOptionalHeader = %x \n", m_image_file_header->SizeOfOptionalHeader);
	printf("Characteristics = %x \n", m_image_file_header->Characteristics);
}
//64位的PE文件格式有所不同
void ReadOptionalPEHeader()
{
	char* temp_ptr = m_file_buffer + m_dos_header->e_lfanew + NT_FLAG_COUNT + IMAGE_SIZEOF_FILE_HEADER;
	m_image_option_header = new _IMAGE_OPTIONAL_HEADER();
	m_image_option_header->Magic = *((WORD*)temp_ptr);
	m_image_option_header->MajorLinkerVersion = *(temp_ptr + 2);
	m_image_option_header->MinorLinkerVersion = *(temp_ptr + 3);
	m_image_option_header->SizeOfCode = *((DWORD*)(temp_ptr+4));
	m_image_option_header->SizeOfInitializedData = *((DWORD*)(temp_ptr + 8));
	m_image_option_header->SizeOfUninitializedData = *((DWORD*)(temp_ptr + 12));
	m_image_option_header->AddressOfEntryPoint = *((DWORD*)(temp_ptr + 16));
	m_image_option_header->BaseOfCode = *((DWORD*)(temp_ptr + 20));

	printf("----%x \n", m_image_file_header->Machine);
	if (m_image_file_header->Machine == 0x8664)
	{
		m_image_option_header->ImageBase = *((LONGLONG*)(temp_ptr + 24));
		printf("----ImageBase for x64 = %llx \n", m_image_option_header->ImageBase);
	}
	else if(m_image_file_header->Machine == 0x14c)
	{
		m_image_option_header->BaseOfData = *((DWORD*)(temp_ptr + 24));
		m_image_option_header->ImageBase = *((DWORD*)(temp_ptr + 28));
	}
	else
	{
		//既不是X86,也不是x64，需要关注一下
		assert(0);
	}
	m_image_option_header->SectionAlignment = *((DWORD*)(temp_ptr + 32));
	m_image_option_header->FileAlignment = *((DWORD*)(temp_ptr + 36));
	m_image_option_header->MajorOperatingSystemVersion = *((WORD*)(temp_ptr + 40));
	m_image_option_header->MinorOperatingSystemVersion = *((WORD*)(temp_ptr + 42));
	m_image_option_header->MajorImageVersion = *((WORD*)(temp_ptr + 44));
	m_image_option_header->MinorImageVersion = *((WORD*)(temp_ptr + 46));
	m_image_option_header->MajorSubsystemVersion = *((WORD*)(temp_ptr + 48));
	m_image_option_header->MinorSubsystemVersion = *((WORD*)(temp_ptr + 50));
	m_image_option_header->Win32VersionValue = *((DWORD*)(temp_ptr + 52));
	m_image_option_header->SizeOfImage = *((DWORD*)(temp_ptr +56));
	m_image_option_header->SizeOfHeaders = *((DWORD*)(temp_ptr + 60));
	m_image_option_header->CheckSum = *((DWORD*)(temp_ptr + 64));
	m_image_option_header->Subsystem = *((DWORD*)(temp_ptr + 68));
	m_image_option_header->DllCharacteristics = *((WORD*)(temp_ptr + 70));
	if (m_image_file_header->Machine == 0x8664)
	{
		m_image_option_header->SizeOfStackReserve = *((LONGLONG*)(temp_ptr + 72));
		m_image_option_header->SizeOfStackCommit = *((LONGLONG*)(temp_ptr + 80));
		m_image_option_header->SizeOfHeapReserve = *((LONGLONG*)(temp_ptr + 88));
		m_image_option_header->SizeOfHeapCommit = *((LONGLONG*)(temp_ptr + 96));
		m_image_option_header->LoaderFlags = *((DWORD*)(temp_ptr + 104));
		m_image_option_header->NumberOfRvaAndSizes = *((DWORD*)(temp_ptr + 108));
		for (int i = 0; i < 16; i++)
		{
			m_image_option_header->DataDirectory[i].VirtualAddress = *((DWORD*)(temp_ptr + 112 + i * 8));
			m_image_option_header->DataDirectory[i].Size = *((DWORD*)(temp_ptr + 116 + i * 8));
		}
	}
	else if (m_image_file_header->Machine == 0x14c)
	{
		m_image_option_header->SizeOfStackReserve = *((DWORD*)(temp_ptr + 72));
		m_image_option_header->SizeOfStackCommit = *((DWORD*)(temp_ptr + 76));
		m_image_option_header->SizeOfHeapReserve = *((DWORD*)(temp_ptr + 80));
		m_image_option_header->SizeOfHeapCommit = *((DWORD*)(temp_ptr + 84));
		m_image_option_header->LoaderFlags = *((DWORD*)(temp_ptr + 88));
		m_image_option_header->NumberOfRvaAndSizes = *((DWORD*)(temp_ptr + 92));
		for (int i = 0; i < 16; i++)
		{
			m_image_option_header->DataDirectory[i].VirtualAddress = *((DWORD*)(temp_ptr + 96 + i * 8));
			m_image_option_header->DataDirectory[i].Size = *((DWORD*)(temp_ptr + 100 + i * 8));
		}
	}
	else
	{
		//既不是X86,也不是x64，需要关注一下
		assert(0);
	}



	m_nt_header->OptionalHeader = *m_image_option_header;
}
void PrintOptionalPEHeader()
{
	printf("------------------File PE Header---------------\n");
	if (m_image_option_header == NULL)
	{
		return;
	}
	printf("Magic = %x \n", m_image_option_header->Magic);
	printf("MajorLinkerVersion = %x \n", m_image_option_header->MajorLinkerVersion);
	printf("MinorLinkerVersion = %x \n", m_image_option_header->MinorLinkerVersion);
	printf("SizeOfCode = %x \n", m_image_option_header->SizeOfCode);
	printf("SizeOfInitializedData = %x \n", m_image_option_header->SizeOfInitializedData);
	printf("SizeOfUninitializedData = %x \n", m_image_option_header->SizeOfUninitializedData);
	printf("AddressOfEntryPoint = %x \n", m_image_option_header->AddressOfEntryPoint);
	printf("BaseOfCode = %x \n", m_image_option_header->BaseOfCode);
	printf("BaseOfData = %x \n", m_image_option_header->BaseOfData);
	printf("ImageBase = %x \n", m_image_option_header->ImageBase);
	printf("SectionAlignment = %x \n", m_image_option_header->SectionAlignment);
	printf("FileAlignment = %x \n", m_image_option_header->FileAlignment);
	printf("MajorOperatingSystemVersion = %x \n", m_image_option_header->MajorOperatingSystemVersion);
	printf("MinorOperatingSystemVersion = %x \n", m_image_option_header->MinorOperatingSystemVersion);
	printf("MajorImageVersion = %x \n", m_image_option_header->MajorImageVersion);
	printf("MinorImageVersion = %x \n", m_image_option_header->MinorImageVersion);
	printf("MajorSubsystemVersion = %x \n", m_image_option_header->MajorSubsystemVersion);
	printf("MinorSubsystemVersion = %x \n", m_image_option_header->MinorSubsystemVersion);
	printf("Win32VersionValue = %x \n", m_image_option_header->Win32VersionValue);
	printf("SizeOfImage = %x \n", m_image_option_header->SizeOfImage);
	printf("SizeOfHeaders = %x \n", m_image_option_header->SizeOfHeaders);
	printf("CheckSum = %x \n", m_image_option_header->CheckSum);
	printf("Subsystem = %x \n", m_image_option_header->Subsystem);
	printf("DllCharacteristics = %x \n", m_image_option_header->DllCharacteristics);

	printf("SizeOfStackReserve = %x \n", m_image_option_header->SizeOfStackReserve);
	printf("SizeOfStackCommit = %x \n", m_image_option_header->SizeOfStackCommit);
	printf("SizeOfHeapReserve = %x \n", m_image_option_header->SizeOfHeapReserve);
	printf("SizeOfHeapCommit = %x \n", m_image_option_header->SizeOfHeapCommit);

	printf("LoaderFlags = %x \n", m_image_option_header->LoaderFlags);
	printf("NumberOfRvaAndSizes = %x \n", m_image_option_header->NumberOfRvaAndSizes);
	for (int i = 0; i < 16; i++)
	{
		printf("DataDirectory va = %x size = %x \n", m_image_option_header->DataDirectory[i].VirtualAddress, m_image_option_header->DataDirectory[i].Size);
	}
}

void ReadSectionTable()
{
	char* temp_ptr = m_file_buffer + m_dos_header->e_lfanew + NT_FLAG_COUNT + IMAGE_SIZEOF_FILE_HEADER + m_image_file_header->SizeOfOptionalHeader;
	for (int i = 0; i < m_image_file_header->NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER section_ptr = new IMAGE_SECTION_HEADER();
		m_section_table.push_back(section_ptr);		
		memcpy(section_ptr->Name, temp_ptr + i * IMAGE_SIZEOF_SECTION_HEADER, IMAGE_SIZEOF_SHORT_NAME);
		section_ptr->Misc.VirtualSize = *((DWORD*)(temp_ptr + 8 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->VirtualAddress = *((DWORD*)(temp_ptr + 12 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->SizeOfRawData = *((DWORD*)(temp_ptr + 16 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->PointerToRawData = *((DWORD*)(temp_ptr + 20 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->PointerToRelocations = *((DWORD*)(temp_ptr + 24 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->PointerToLinenumbers = *((DWORD*)(temp_ptr + 28 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->NumberOfRelocations = *((WORD*)(temp_ptr + 32 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->NumberOfLinenumbers = *((WORD*)(temp_ptr + 34 + i * IMAGE_SIZEOF_SECTION_HEADER));
		section_ptr->Characteristics = *((DWORD*)(temp_ptr + 36 + i * IMAGE_SIZEOF_SECTION_HEADER));
	}
}
void PrintSectionTable()
{
	printf("------------------Section Table---------------\n");
	for (int i = 0; i < m_section_table.size(); i++)
	{
		IMAGE_SECTION_HEADER* section_ptr = m_section_table[i];
		printf("---------------以下是第%d节的信息--------------\n", i + 1);
		printf("Name = %s \n", section_ptr->Name);
		printf("Misc = %x \n", section_ptr->Misc.VirtualSize);
		printf("VirtualAddress = %x \n", section_ptr->VirtualAddress);
		printf("SizeOfRawData = %x \n", section_ptr->SizeOfRawData);
		printf("PointerToRawData = %x \n", section_ptr->PointerToRawData);
		printf("PointerToRelocations = %x \n", section_ptr->PointerToRelocations);
		printf("PointerToLinenumbers = %x \n", section_ptr->PointerToLinenumbers);
		printf("NumberOfRelocations = %x \n", section_ptr->NumberOfRelocations);
		printf("NumberOfLinenumbers = %x \n", section_ptr->NumberOfLinenumbers);
		printf("Characteristics = %x \n", section_ptr->Characteristics);
	}
}

//-------------------------------------------------------------------
bool FileBuffer2ImageBuffer()
{
	m_image_buffer = (char*)malloc(m_image_option_header->SizeOfImage);
	memset(m_image_buffer, 0, m_image_option_header->SizeOfImage);

	//先把头和节表整体拷贝过去
	int header_length = m_image_option_header->SizeOfHeaders;
	memcpy(m_image_buffer, m_file_buffer, header_length);

	for (int i = 0; i < m_section_table.size(); i++)
	{
		int section_file_length = m_section_table[i]->SizeOfRawData;
		char* dest_ptr = m_image_buffer + m_section_table[i]->VirtualAddress;
		char* source_ptr = m_file_buffer + m_section_table[i]->PointerToRawData;
		memcpy(dest_ptr, source_ptr, section_file_length);
	}
	return true;
}

bool ImageBuffer2FileBuffer()
{
	return true;
}



