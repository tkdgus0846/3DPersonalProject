#include "stdafx.h"
#include "..\Public\MapWindow.h"

HRESULT CMapWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	m_eBrushMode = BM_SCULPT;
	m_fBrushSize = (m_fMaxBrushSize + m_fMinBrushSize) * 0.5f;

	m_TerrainSize[0] = 100;
	m_TerrainSize[1] = 100;

	m_TerrainInterval = 1.f;

	return result;
}

CMapWindow* CMapWindow::Create(const WINDOWDESC& windowDesc)
{
	CMapWindow* newWindow = new CMapWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("MapWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

void CMapWindow::Rendering()
{
	Spacing();
	BrushTool();
	Spacing();
	TerrainTool();
	
}

void CMapWindow::BrushTool()
{
	if (ImGui::CollapsingHeader("BrushTool"))
	{
		Spacing();
		if (RadioButton("Sculpt", m_eBrushMode == BM_SCULPT))
		{
			m_eBrushMode = BM_SCULPT;
		}
		SameLine();
		if (RadioButton("Erosion", m_eBrushMode == BM_EROSION))
		{
			m_eBrushMode = BM_EROSION;
		}

		float scroll = ImGui::GetIO().MouseWheel;
		if (scroll != 0.0f)
		{
			m_fBrushSize += scroll; // ���� �����ӿ� ���� ���� ���� ����

			if (m_fBrushSize > m_fMaxBrushSize) 
				m_fBrushSize = m_fMaxBrushSize;
			if (m_fBrushSize < m_fMinBrushSize)
				m_fBrushSize = m_fMinBrushSize;
		}

		/*Spacing(); Spacing(); Spacing(); Spacing();*/
		NewLine();
		SeparatorText("Brush Default Settings");
		SliderFloat("BrushSize", &m_fBrushSize, m_fMinBrushSize, m_fMaxBrushSize, "%.2f");

		switch (m_eBrushMode)
		{
		case CMapWindow::BM_SCULPT:
			SculptBrush();
			break;
		case CMapWindow::BM_EROSION:
			ErosionBrush();
			break;
		case CMapWindow::BM_END:
			break;
		default:
			break;
		}
	}
}

void CMapWindow::ErosionBrush()
{
	NewLine();
	SeparatorText("Erosion");
}

void CMapWindow::SculptBrush()
{
	NewLine();
	SeparatorText("Sculpt");
}

void CMapWindow::TerrainTool()
{
	if (ImGui::CollapsingHeader("TerrainTool"))
	{
		Spacing();
		/*Text("X: ");
		SameLine();*/
		SeparatorText("TerrainSize");
		InputInt(" => X", m_TerrainSize);
		InputInt(" => Z", m_TerrainSize+1);
		InputFloat(" => Interval", &m_TerrainInterval);
		
		NewLine();
		SeparatorText("Texture");

		// �ؽ��� �ҷ����� (����Ʈ�ڽ�)
		
		// ���̸� (�Ƹ��� ���� �׸� ���̸��� �ҷ��ðŰ���)
		
		

		SetCursorPosX(150.f);
		if (Button("Apply"))
		{
			// ���� �ͷ����� ����� ���ο� �ͷ����� �����.
		}
		SameLine();
		if (Button("Undo"))
		{
			// ���� �ͷ��� ������ �ǵ������Ѵ�.
		}
		SameLine();
		if (Button("Redo"))
		{
			// �ǵ����⸦ �������.
		}

	}
}
