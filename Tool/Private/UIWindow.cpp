#include "stdafx.h"
#include "..\Public\UIWindow.h"

HRESULT CUIWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);


	return result;
}

CUIWindow* CUIWindow::Create(const WINDOWDESC& windowDesc)
{
	CUIWindow* newWindow = new CUIWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("CUIWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

void CUIWindow::Rendering()
{

	
}
