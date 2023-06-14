#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CDataParsing
{
public:
	static HRESULT Save_File(const TCHAR* pFilePath, vector<ISerializable*> dataVector);
	static HRESULT Save_File(const TCHAR* pFilePath, ISerializable* data);
	static HRESULT Load_File(const TCHAR* pFilePath, vector<ISerializable*> dataVector);
	static HRESULT Load_File(const TCHAR* pFilePath, ISerializable* data);
};

END