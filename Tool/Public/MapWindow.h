#pragma once
#include "ImGuiWindow.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CNavigation;
END

class CMapWindow : public CImGuiWindow
{
public:
	enum BrushMode
	{
		BM_SCULPT,
		BM_EROSION,
		BM_FLATTEN,
		BM_END
	};

	struct TerrainInfo
	{

	};

public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CMapWindow* Create(const WINDOWDESC& windowDesc);

	virtual void Tick(_double timeDelta) override;

	void			Set_Render_NavMesh(_bool bRender);
	void			Set_CurTerrain(class CDummyObject* pTerrain);

	void			Save_Terrain(const wstring& name);
	void			Load_Terrain(const wstring& name);

private:
	void			Save_Navigation(const wstring& name);
	void			Load_Navigation(const wstring& name);
	void			Save_Texture();

private:
	virtual void	Rendering() override;

	/* Brush 관련 함수들*/
	void			BrushTool();
	void			ErosionBrush();
	void			SculptBrush();
	void			FlattenBrush();

	void			Brush_Function();

	/* Terrain 관련 함수들*/
	void			TerrainTool();
	void			ShowTextureComponentList();

	virtual void	Open_Function() final;
	virtual void	Close_Function() final;

	virtual void	Save_Function() final;
	virtual void	Load_Function() final;

	void			DeleteCurTerrain();
	
	/* NavMesh 관련 함수들 */
	void			NavMeshTool();
	void			ShowNavigationList();
	void			EditNavigationTool();

	


private:
	/* 텍스쳐를 위한 원본 리스트*/
	vector<pair<wstring, class CComponent*>> m_PrototypesTextures;
	vector<string> m_PrototypesTexturesStr;

	/* 네비 메쉬를 위한 원본 리스트*/
	vector<pair<wstring, class CComponent*>> m_PrototypesNavMeshes;
	vector<string> m_PrototypesNavMeshesStr;

	/* Map_Window들을 위한 포인터들*/
	class CDummyObject*		m_pCurTerrain = { nullptr };
	CVIBuffer_Terrain*		m_pCurTerrainBuffer = { nullptr };
	CNavigation*			m_pCurNavigation = { nullptr };

	/* Brush 관련 변수들*/
	BrushMode		m_eBrushMode;
	_float			m_fBrushSize;
	const _float	m_fMaxBrushSize = { 50.f };
	const _float	m_fMinBrushSize = { 7.0f };
	_float			m_fBrushPower = { 0.5f };
	const _float	m_fMaxBrushPower = { 1.f };
	const _float	m_fMinBrushPower = { 0.1f };
	_bool			m_bBrushOn = { true };

	/* Terrain 관련 변수들*/
	_int			m_TerrainSize[2];
	_float			m_TerrainInterval;
	_bool			m_bLoad = false;

	/* Terrain Texture*/
	_char			m_ComponentSearchBuffer[MAX_PATH];
	_int			selectedComponentIndex = { -1 };

	/* 네비 메쉬 관련 변수들*/
	_char			m_NavigationSearchBuffer[MAX_PATH];
	_int			selectedNavigationIndex = { -1 };
	_bool			m_bNavMeshEditMode = { false };
	vector<_float3> m_PickingVec;
	vector<_uint>	m_PickingIndexVec;

};

