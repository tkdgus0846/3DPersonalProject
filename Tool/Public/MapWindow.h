#pragma once
#include "ImGuiWindow.h"



class CMapWindow : public CImGuiWindow
{
public:
	enum BrushMode
	{
		BM_SCULPT,
		BM_EROSION,
		BM_END
	};

	struct TerrainInfo
	{

	};

public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CMapWindow* Create(const WINDOWDESC& windowDesc);

private:
	virtual void	Rendering() override;

	/* Brush ���� �Լ���*/
	void			BrushTool();
	void			ErosionBrush();
	void			SculptBrush();

	/* Terrain ���� �Լ���*/
	void			TerrainTool();

private:
	/* Brush ���� ������*/
	BrushMode		m_eBrushMode;
	_float			m_fBrushSize;
	const _float	m_fMaxBrushSize = { 50.f };
	const _float	m_fMinBrushSize = { 0.1f };

	/* Terrain ���� ������*/
	_int			m_TerrainSize[2];
	_float			m_TerrainInterval;
	

};

