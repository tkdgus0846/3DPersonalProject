#pragma once
#include "ImGuiWindow.h"

BEGIN(Engine)
class CGameObject;
class CComponent;
class CTransform;
END

enum InspectorMode
{
	IM_GAMEOBJECT,
	IM_TRANSFORM,
	IM_COLLIDER,
	IM_TEXTURE,
	IM_SHADER,
	IM_VIBUFFER,
	IM_RENDERER,
	IM_MODEL,
	IM_NAVIGATION,

	IM_DEFAULT,
	IM_END
};

class CInspectorWindow : public CImGuiWindow
{

public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CInspectorWindow* Create(const WINDOWDESC& windowDesc);

private:
	virtual void	Rendering() override;


	void			Mode_GameObject();
	void			Mode_Transform();
	void			Mode_Collider();
	void			Mode_Texture();
	void			Mode_Shader();
	void			Mode_Animation();
	void			Mode_Model();
	void			Mode_Renderer();
	void			Mode_Navigation();
	void			Mode_Default();

	/*������Ʈ �ν����� â*/
	void			Show_ComponentList();
	void			Copy_Object_Function();
	void			Paste_Object_Function();
	void			Add_Component_Function();
	void			Delete_Component_Function();
	void			Delete_Button();

	/*����Ʈ �ν����� â*/
	void			Add_GameObject_Function();
	void			Delete_GameObject_Function();

	/* ���콺 ��ŷ */
	HRESULT			Place_Object(class CTransform* transformComp);

	_bool			Is_Use_Picking() override;


private:
	InspectorMode	m_eInspectorMode;
	CComponent*		m_pCurComponent = { nullptr };
	_int			selectedComponentIndex = { -1 };
	vector<pair<wstring, class CComponent*>> m_PrototypesVec;
	vector<string>			m_PrototypesStrVec;

	/* ���ڿ� �Է� ���۵�*/
	_char m_ComponentSearchBuffer[FILE_NAME_SIZE] = "";
	_char m_ObjectAddNameBuffer[FILE_NAME_SIZE] = "";

	/* Ʈ������ ���� ������*/
	_float translation[3] = { 0.f, 0.f, 0.f };
	_float rotation[3] = { 0.f, 0.f, 0.f };
	_float scale[3] = { 1.f, 1.f, 1.f };
	_bool m_bPlaceObject = { false }; // ��ŷ�ؼ� Ʈ������ �ٲٱ�.
	_bool m_bScaleLock = { false };

	/* ������Ʈ ���� ���� ������*/
	_bool m_bPastePicking = { false }; // ������Ʈ�� �����ؼ� ��ġ�ϱ�.
	_bool m_bCopyingObject = { false };
	_bool m_bCopyLock = { false };
	_char m_CopyingObjectName[FILE_NAME_SIZE] = "";

	/* ������ ���� ������*/
	vector<string>			m_RenderGroupStrVec;
	_uint					m_CurRenderGroupSelectedItem = { 0 };
};

