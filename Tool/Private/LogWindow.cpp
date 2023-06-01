#include "stdafx.h"
#include "..\Public\LogWindow.h"

HRESULT CLogWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	no_titlebar = false;
	m_bAlwaysShow = true;

	return result;
}

CLogWindow* CLogWindow::Create(const WINDOWDESC& windowDesc)
{
	CLogWindow* newWindow = new CLogWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("CLogWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

void CLogWindow::Rendering()
{
	
}
