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
	void			Render_Mode();

	virtual void	Save_Function() final;
	virtual void	Load_Function() final;

private:
	_bool			m_bNavMeshRender = { true };
	_bool			m_bWireFrameRender = { false };


};

