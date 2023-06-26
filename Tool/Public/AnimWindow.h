#pragma once
#include "ImGuiWindow.h"

class CAnimWindow : public CImGuiWindow
{
public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CAnimWindow* Create(const WINDOWDESC& windowDesc);

	virtual void SaveLoadButton() {}

private:
	virtual void	Rendering() override;

	void Show_ModelPrototypes();
	void Show_Animations();

	virtual void	Open_Function() final;
	virtual void	Close_Function() final;

private:
	vector<pair<wstring, class CComponent*>> m_ModelPrototypesVec;
	vector<string>			m_ModelPrototypesStrVec;
	_char			m_ModelSearchBuffer[MAX_PATH];
	_int			selectedModelIndex = { -1 };

	class CDummyObject* m_DummyObject = { nullptr };
	_int			m_CurAnimationIndex = { -1 };
	

};

