#pragma once

#include "Base.h"

/* ��ü���� ������ �����Ѵ�. */
/* ���ӳ��� �����ϴ� ��ü���� ���� ������ ���� ���ؿ� ���� ��������(CLayer) �����Ѵ�. */
/* ��ü���� Tick, Late_Tick�� ������ ȣ�� ���ش�. */

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

	
public:/*���� ����� ������ ������ŭ �̸� ������ �Ҵ��ϳ�. */
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);

	// Add ���� �ʰ� ������Ÿ���� ã�Ƽ� Clone�� ���ִ� ����.
	CGameObject* Clone_Object(const wstring& pPrototypeTag, wstring& pObjName, void* pArg);

	// ������Ÿ���� ������� Add ���ִ� ����.
	CGameObject* Add_GameObject(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pLayerTag, wstring& pObjName, void* pArg);

	// ������Ÿ�� ���X �� Add ���ִ� ����.
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& pLayerTag, wstring& pObjName, CGameObject* object);

	HRESULT Delete_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName);
	HRESULT Delete_GameObject_SameName(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName);

	CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjName);

	HRESULT Clear_Layer(_uint iLevelIndex, const wstring& pLayerTag);
	void Clear_LevelResources(_uint iLevelIndex);
	void Tick(_float TimeDelta);
	void Late_Tick(_float TimeDelta);

	// � ���̾ � ������Ʈ ������ ã�ƿͼ� ����.
	// �����԰� ���ÿ� Add_GameObject�� ����
	// ��������ڸ� ��� �ۼ��߳Ŀ� ���� �ٸ��� �����Ҽ������� ����
	CGameObject* Copy_Object(_uint iLevelIndex, const wstring& layerTag, wstring& objectName);

	void Set_CurLevelIndex(_uint index) 
	{ 
		m_iCurLevelIndex = index; 
	}

	void Clear_ObjectNums() { m_ObjectsNums.clear(); }

	unordered_map<wstring, vector<ObjectParsingData*>>* Get_LoadedObjectData(_int row, _int col);

public:
	unordered_map<wstring, class CLayer*>* Get_LayersMapPtr() { return m_pLayers; }

private:	/* Ű�� ��ü���� �������� ���̹��� �ϱ�����.*/
	unordered_map<wstring, _uint> m_ObjectsNums;

	/* Ű�� �ε����� �ҷ��� �����͵��� ������ �ֱ�����. �ͷ��� ���� ��� �ִ°�.*/
	unordered_map<wstring, vector<ObjectParsingData*>>  m_LoadedObjectDatas[GRID_TOTAL_SIZE];

private: /* ������ü���� �����Ͽ� �����ϰ� �ִ´�. */
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;

private: /* �纻��ü���� ���̾�� �����Ͽ� �����Ѵ�. */
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

	// ISerializable��(��) ���� ��ӵ�
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) override;
	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) override;
	ParsingData*	Load_Tool_Objects(ParsingData* data);
};

END