#pragma once
#include "ImGuiWindow.h"

class CUIWindow : public CImGuiWindow
{
public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CUIWindow* Create(const WINDOWDESC& windowDesc);

private:
	virtual void	Rendering() override;
	
};

