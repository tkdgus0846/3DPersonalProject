#pragma once

#include "Base.h"
#include "Camera.h"

BEGIN(Engine)

class CCamera;

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager();

private:
	CCamera* Find_Camera(const wstring& pCamTag);
	void Add_Camera(const wstring& pCamTag, CCamera* pCamera);
	void Remove_Camera(const wstring& pCamTag);

public:
	CCamera* Get_CurCamera() { return m_pCurCamera; }
	void On_Camera(const wstring& pCamTag);
	void On_Shake(CCamera::SHAKE_TYPE eType, const _float& fForce = 1.5f, const _float& fTime = 5.f);

private:
	CCamera* m_pCurCamera;
	unordered_map<wstring, CCamera*>	m_CameraList;

private:
	virtual void Free(void) override;
};

END
