#include "..\Public\DataParsing.h"

ParsingData* CDataParsing::Save_File(const TCHAR* pFilePath, ISerializable* data)
{
	// 颇老 俺规
	HANDLE		hFile = CreateFile
	(pFilePath,
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		//MSG_BOX("Failed to Save File");
		return nullptr;
	}

	ParsingData* result = data->Save_Data(hFile, nullptr);

	CloseHandle(hFile);

	//MSG_BOX("Success to Save File");
	return result;
}

ParsingData* CDataParsing::Load_File(const TCHAR* pFilePath, ISerializable* data)
{
	// 颇老 俺规
	HANDLE		hFile = CreateFile
	(pFilePath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		//MSG_BOX("Failed to Load File");
		return nullptr;
	}


	ParsingData* result = data->Load_Data(hFile, nullptr);


	// 颇老 家戈
	CloseHandle(hFile);

	//MSG_BOX("Succeed to Load File");
	return result;
}
