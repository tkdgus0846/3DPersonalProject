#pragma once
#include "ImGuiWindow.h"



class CLogWindow : public CImGuiWindow
{
public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CLogWindow* Create(const WINDOWDESC& windowDesc);

	virtual void SaveLoadButton() {}
private:
	virtual void	Rendering() override;
};

