#pragma once

#include "Base.h"

/* 객체들의 원형을 보관한다. */
/* 게임내에 출현하는 객체들을 내가 나누고 싶은 기준에 따라 구분지어(CLayer) 보관한다. */
/* 객체들의 Tick, Late_Tick을 무한히 호출 해준다. */

BEGIN(Engine)

class CObject_Manager final : public CBase, public ISerializable
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	enum TerrainGrid
	{
		GRID_SIZE = 2,
		GRID_TOTAL_SIZE = GRID_SIZE*GRID_SIZE
	};

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

	
public:/*실제 사용할 레벨의 갯수만큼 미리 공간을 할당하낟. */
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);

	// Add 하지 않고 프로토타입을 찾아서 Clone만 해주는 역할.
	CGameObject* Clone_Object(const wstring& pPrototypeTag, wstring& pObjName, void* pArg);

	// 프로토타입을 기반으로 Add 해주는 역할.
	CGameObject* Add_GameObject(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pLayerTag, wstring& pObjName, void* pArg);

	// 프로토타입 기반X 로 Add 해주는 역할.
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& pLayerTag, wstring& pObjName, CGameObject* object);

	HRESULT Delete_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName);
	HRESULT Delete_GameObject_SameName(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName);

	CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName);

	HRESULT Clear_Layer(_uint iLevelIndex, const wstring& pLayerTag);
	void Clear_LevelResources(_uint iLevelIndex);
	void Tick(_float TimeDelta);
	void Late_Tick(_float TimeDelta);

	// 어떤 레이어에 어떤 오브젝트 인지를 찾아와서 복사.
	// 복사함과 동시에 Add_GameObject도 실행
	// 복사생성자를 어떻게 작성했냐에 따라 다르게 동작할수있음에 주의
	CGameObject* Copy_Object(_uint iLevelIndex, const wstring& layerTag, wstring& objectName);

	void Set_CurLevelIndex(_uint index) 
	{ 
		m_iCurLevelIndex = index; 
	}

	void Clear_ObjectNums() { m_ObjectsNums.clear(); }

	unordered_map<wstring, vector<ObjectParsingData*>>* Get_LoadedObjectData(_int row, _int col);

public:
	unordered_map<wstring, class CLayer*>* Get_LayersMapPtr() { return m_pLayers; }

private:	/* 키와 객체수를 바탕으로 네이밍을 하기위함.*/
	unordered_map<wstring, _uint> m_ObjectsNums;

	/* 키와 로딩에서 불러온 데이터들을 가지고 있기위함. 터레인 별로 들고 있는것.*/
	unordered_map<wstring, vector<ObjectParsingData*>>  m_LoadedObjectDatas[GRID_TOTAL_SIZE];

private: /* 원형객체들을 생성하여 보관하고 있는다. */
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;

private: /* 사본객체들을 레이어로 구분하여 보관한다. */
	_uint m_iNumLevels = { 0 };
	_uint m_iCurLevelIndex = { 0 };

	unordered_map<wstring, class CLayer*>*		m_pLayers = { nullptr };
	typedef unordered_map<wstring, class CLayer*>	LAYERS;

private:
	class CGameObject* Find_Prototype(const wstring& pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& pLayerTag);

	void Decide_Object_Name(wstring& objName);
	void Decrease_Object_Name(const wstring& objName);
	
	HRESULT Make_ObjectPath(OUT _tchar* resultPath, const wstring& levelPath, const wstring& objectName);

public:
	virtual void Free() override;

	// ISerializable을(를) 통해 상속됨
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) override;
	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) override;
	ParsingData*	Load_Tool_Objects(ParsingData* data);
};

END