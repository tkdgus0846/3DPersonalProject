#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"

/* 0. Ŭ���̾�Ʈ ������ ���� ��Ȱ�Ѵ�. */
/* 0.0 �����ν��Ͻ���� ��ü�� ���ؼ� �����Ǵپ��ѱ���� Ȱ��ۤ� �� �ְ� �Ѵ�. */
/* 0.1 ��¥ �ʿ��� �Լ��� �����Ͽ� �������ش�. */
/* 0.2 �Ѳ����� �ʱ�ȭ �� ����ó�� ����.  */
/* 0.3 ������Ʈ ���������� �����ϴ�.  */



BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_double TimeDelta);
	void Clear_LevelResources(_uint iLevelIndex);

	void ResizeBuffers(_uint & g_ResizeWidth, _uint & g_ResizeHeight);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte ubyKeyID);
	_byte Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID);
	_long Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

	bool	Key_Pressing(_ubyte ubyKey);
	bool	Key_Down(_ubyte ubyKey);
	bool	Key_Up(_ubyte ubyKey);

	bool	Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID);
	bool	Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID);
	bool	Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID);

public: /* For.Timer_Manager */
	_double		Get_Timer(const _tchar* pTimerTag);
	void		Set_Timer(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clear_CurLevel_Layer(const wstring & pLayerTag);
	void	Clear_ObjectNums();

	// Add ���� �ʰ� ������Ÿ���� ã�Ƽ� Clone�� ���ִ� ����.
	CGameObject* Clone_Object(const wstring & pPrototypeTag, wstring & pObjName, void* pArg = nullptr);

	// ������Ÿ���� ������� Add ���ִ� ����.
	CGameObject* Add_GameObject(_uint iLevelIndex, const wstring & pPrototypeTag, const wstring& pLayerTag, wstring & pObjName, void* pArg = nullptr);
	// ������Ÿ�� ���X �� Add ���ִ� ����.
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring & pLayerTag, wstring & pObjName, CGameObject * object);
	CGameObject* Copy_GameObject(const wstring & layerTag, wstring & objectName);
	HRESULT Delete_GameObject(const wstring & pLayerTag, const wstring & pObjName);
	HRESULT Delete_GameObject_SameName(const wstring & pLayerTag, const wstring & pObjName);
	CGameObject* Find_GameObject(const wstring & pLayerTag, const wstring & pObjName);

	HRESULT Save_CurLevel(const _tchar* curLevelName);
	HRESULT Load_CurLevel(const _tchar* curLevelName, _bool bIsTool = false);

	list<class CGameObject*> Get_All_Objects();

public: /* For.Component_Manager*/ 
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & pPrototypeTag, void* pArg = nullptr);
	vector<pair<wstring, class CComponent*>> Get_Prototypes_ByVector();

public: /* For.PipeLine */
	_float4x4 Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4 Get_CamPosition() const;

public: /* For. Light Manager */
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);
	HRESULT Add_Lights(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CLight::LIGHTDESC & LightDesc);

public: /* For. Collision Manager */
#ifdef _DEBUG
	void	Render_Collider();
	_bool	Get_ColliderRender();
	void	Set_ColliderRender(_bool bRender);
	_bool	Toggle_ColliderRender();
#endif
	void	Check_Collision(COLGROUP ID1, COLGROUP ID2);
	void	Change_ColGroup(CCollider * collider, COLGROUP changeID);
	void	Add_ColGroup(COLGROUP eID, CCollider * pCollider);
	void	Reset_ColGroup();
	HRESULT	Remove_Collider(CCollider * collider, COLGROUP colID);
	

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CCollisionManager*		m_pCollisionManager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END