#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CDataParsing
{
public:
	static ParsingData* Save_File(const TCHAR* pFilePath, class ISerializable* data, ParsingData* pData = nullptr);
	static ParsingData* Load_File(const TCHAR* pFilePath, class ISerializable* data, ParsingData* pData = nullptr);
};

END