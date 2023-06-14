#include "..\Public\DataParsing.h"

HRESULT CDataParsing::Save_File(const TCHAR* pFilePath, vector<ISerializable*> dataVector)
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
		MSG_BOX("Failed to Save File");
		return E_FAIL;
	}

	for (auto data : dataVector)
	{
		HRESULT result = data->Save_Data(nullptr);
	}

	CloseHandle(hFile);

	MSG_BOX("Success to Save File");
	return S_OK;
}

HRESULT CDataParsing::Save_File(const TCHAR* pFilePath, ISerializable* data)
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
		MSG_BOX("Failed to Save File");
		return E_FAIL;
	}

	HRESULT result = data->Save_Data(nullptr);

	CloseHandle(hFile);

	MSG_BOX("Success to Save File");
	return S_OK;
}

HRESULT CDataParsing::Load_File(const TCHAR* pFilePath, vector<ISerializable*> dataVector)
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
		MSG_BOX("Failed to Load File");
		return E_FAIL;
	}

	for (auto data : dataVector)
	{
		HRESULT result = data->Load_Data(nullptr);
	}

	// 颇老 家戈
	CloseHandle(hFile);

	MSG_BOX("Succeed to Load File");
	return S_OK;
}

HRESULT CDataParsing::Load_File(const TCHAR* pFilePath, ISerializable* data)
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
		MSG_BOX("Failed to Load File");
		return E_FAIL;
	}


	HRESULT result = data->Load_Data(nullptr);


	// 颇老 家戈
	CloseHandle(hFile);

	MSG_BOX("Succeed to Load File");
	return S_OK;
}
