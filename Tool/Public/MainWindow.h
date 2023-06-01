#pragma once
#include "ImGuiWindow.h"

class CMainWindow : public CImGuiWindow
{
public:
	static CMainWindow* Create(const WINDOWDESC& windowDesc);

private:
	virtual HRESULT	Initialize(const WINDOWDESC& desc) override;
	virtual void	Rendering() override;
	virtual void	SaveLoadButton() final;
	virtual void	ExitButton() final;

	virtual void	Save_Function() final;
	virtual void	Load_Function() final;

};

