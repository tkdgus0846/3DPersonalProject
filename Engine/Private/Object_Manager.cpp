#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "DataParsing.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{

}

HRESULT CObject_Manager::Reserve_Containers(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CGameObject* CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pLayerTag, wstring& pObjName, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	Decide_Object_Name(pObjName);

	pGameObject->SetName(pObjName);

	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObjects(pGameObject);

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}

	else
		pLayer->Add_GameObjects(pGameObject);

	return pGameObject;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring& pLayerTag, wstring& pObjName, CGameObject* object)
{
	// 그냥 게임오브젝트를 추가하는거지 복사하는 역할은 하지 않는다.
	if (nullptr == object)
		return E_FAIL;
	
	CGameObject* pGameObject = object;
	Decide_Object_Name(pObjName);

	pGameObject->SetName(pObjName);

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObjects(pGameObject);

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}

	else
		pLayer->Add_GameObjects(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Delete_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName)
{
	CGameObject* resultObject = nullptr;
	HRESULT hr = E_FAIL;

	auto iter = m_pLayers->find(pLayerTag);

	if (iter != m_pLayers->end())
	{
		hr = iter->second->Delete_GameObject(pObjName);
		//Decrease_Object_Name(pObjName);
		
	}
	return hr;
}

CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName)
{
	CGameObject* resultObject = nullptr;

	auto iter = m_pLayers->find(pLayerTag);

	if (iter != m_pLayers->end())
	{
		resultObject = iter->second->Find_Object(pObjName);
	}
	return resultObject;
}

void CObject_Manager::Clear_LevelResources(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(TimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(TimeDelta);
		}
	}
}

CGameObject* CObject_Manager::Copy_Object(_uint iLevelIndex, const wstring& layerTag, wstring& objectName)
{
	CGameObject* resultObject = nullptr;

	auto iter = m_pLayers->find(layerTag);

	if (iter != m_pLayers->end())
	{
		CGameObject* object = iter->second->Find_Object(objectName);

		if (object != nullptr)
		{
			resultObject = object->Clone(nullptr);

			Add_GameObject(iLevelIndex, layerTag, objectName, resultObject);
		}
	}

	return resultObject;
}

CGameObject * CObject_Manager::Find_Prototype(const wstring& pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Decide_Object_Name(wstring& objName)
{	
	string findStr = CConversion::WstringToString(objName);
	vector<string> vec = CConversion::SplitStringA(findStr, "_");
	wstring OriginName = CConversion::StringToWstring(vec.front());

	wstring newObjName = OriginName;

	auto iter = m_ObjectsNums.find(newObjName);

	if (iter == m_ObjectsNums.end())
	{
		m_ObjectsNums.emplace(objName, 1);
	}
	else
	{
		newObjName = newObjName + L"_" + to_wstring(++iter->second);
	}
	
	objName = newObjName;
}

void CObject_Manager::Decrease_Object_Name(const wstring& objName)
{
	string findStr = CConversion::WstringToString(objName);
	vector<string> vec = CConversion::SplitStringA(findStr, "_");
	wstring OriginName = CConversion::StringToWstring(vec.front());

	wstring newObjName = OriginName;

	auto iter = m_ObjectsNums.find(newObjName);

	if (iter == m_ObjectsNums.end())
	{
		return;
	}
	else
	{
		iter->second--;
		if (iter->second < 0) iter->second = 0;
	}

}

HRESULT CObject_Manager::Make_ObjectPath(OUT _tchar* resultPath, const wstring& levelPath, const wstring& objectName)
{
	wstring fullPath = levelPath + objectName + L".dat";

	lstrcpy(resultPath, fullPath.c_str());
	return S_OK;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
	{
		Safe_Release(Pair.second);
	}

	for (auto& Pair : m_LoadedObjectDatas)
	{
		for (auto data : Pair.second)
		{
			Safe_Delete(data);
		}
	}

	m_Prototypes.clear();

}

ParsingData* CObject_Manager::Save_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = { 0 };
	_tchar objectPath[MAX_PATH];

	LevelParsingData* levelData = (LevelParsingData*)data;

	wstring levelName = levelData->LevelName;
	wstring fullPath = L"../../Levels/" + levelName;

	// 폴더 생성
	if (!CreateDirectory(fullPath.c_str(), NULL))
	{
		// 폴더 생성 실패
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			MSG_BOX("Failed to Create Folder");
			return nullptr;
		}
	}

	fullPath = fullPath + L"/";
	DeleteFilesInDirectory(CConversion::WstringToString(fullPath));

	for (auto& layer : m_pLayers[m_iCurLevelIndex])
	{
		for (auto& object : *layer.second->Get_GameObjectsList())
		{
			// 툴에서 사용하는 카메라는 저장하지 않기 위함이다.
			if (object->GetName().compare(L"ToolCamera") == 0)
				continue;

			Make_ObjectPath(objectPath, fullPath, object->GetName());
			CDataParsing::Save_File(objectPath, object, nullptr);
		}
	}
	return nullptr;
}

ParsingData* CObject_Manager::Load_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = { 0 };

	_tchar compName[MAX_PATH];

	LevelParsingData* levelData = (LevelParsingData*)data;

	wstring levelName = levelData->LevelName;
	wstring fullPath = L"../../Levels/" + levelName;

	fullPath = fullPath + L"/";

	for (const auto& entry : filesystem::directory_iterator(fullPath))
	{
		if (entry.is_regular_file())
		{
			HANDLE		hFile = CreateFile
			(entry.path().c_str(),
				GENERIC_READ,
				NULL,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				return nullptr;
			}

			DWORD dwByte = { 0 };

			ObjectParsingData* myData = new ObjectParsingData;
			TransformParsingData transformData;

			/* 트랜스폼 읽어오는 과정*/
			ReadFile(handle, compName, sizeof(compName), &dwByte, nullptr);
			ReadFile(handle, &transformData.WorldMatrix, sizeof(transformData.WorldMatrix), &dwByte, nullptr);
			myData->TransformData = transformData;

			// 파일 소멸
			CloseHandle(hFile);

			wstring fileName = entry.path().filename().stem();
			vector<wstring> wstringVec = CConversion::SplitStringW(fileName, "_");
			wstring key = wstringVec.front();

			auto iter = m_LoadedObjectDatas.find(key);

			if (iter == m_LoadedObjectDatas.end())
			{
				vector<ObjectParsingData*> vec;
				vec.push_back(myData);
				m_LoadedObjectDatas.emplace(key, vec);
			}
			else
			{
				iter->second.push_back(myData);
			}

		}
	}
	return nullptr;
}

ParsingData* CObject_Manager::Load_Tool_Objects(ParsingData* data)
{
	DWORD dwByte = { 0 };

	_tchar compName[MAX_PATH];

	LevelParsingData* levelData = (LevelParsingData*)data;

	wstring levelName = levelData->LevelName;
	wstring fullPath = L"../../Levels/" + levelName;

	fullPath = fullPath + L"/";

	for (const auto& entry : filesystem::directory_iterator(fullPath))
	{
		if (entry.is_regular_file())
		{

			wstring fileName = entry.path().filename().stem();

			CGameObject* object = Add_GameObject(0, L"Prototype_GameObject_DummyObject", L"Layer_Object", fileName, nullptr);
			CDataParsing::Load_File(entry.path().c_str(), object, nullptr);
		}
	}

	return nullptr;
}




