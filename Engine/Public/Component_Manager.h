#pragma once

/* 컴포넌트들의 원형을 레벨별로 보관한다. */
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"


BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	void Clear_LevelResources(_uint iLevelIndex);
	vector<pair<const _tchar*, class CComponent*>> Get_Prototypes_ByVector();

private:
	_uint	m_iNumLevels = { 0 } ;
private:
	unordered_map<const _tchar*, class CComponent*>*		m_pPrototypes = { nullptr };
	typedef unordered_map<const _tchar*, class CComponent*>	PROTOTYPES;

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END