#include "Camera_Manager.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	:m_pCurCamera(nullptr)
{
}

CCamera_Manager::~CCamera_Manager()
{

}

CCamera* CCamera_Manager::Find_Camera(const wstring& pCamTag)
{
	auto iter = m_CameraList.find(pCamTag);
	if (iter == m_CameraList.end()) return nullptr;

	return iter->second;
}

void CCamera_Manager::On_Camera(const wstring& pCamTag)
{
	CCamera* pCamera = Find_Camera(pCamTag);

	if (pCamera == nullptr) return;

	if (m_pCurCamera)
		m_pCurCamera->Off_Camera();

	pCamera->On_Camera();
	m_pCurCamera = pCamera;
}

void CCamera_Manager::On_Shake(CCamera::SHAKE_TYPE eType, const _float& fForce, const _float& fTime)
{
	m_pCurCamera->On_Shake(eType, fForce, fTime);
}

void CCamera_Manager::Add_Camera(const wstring& pCamTag, CCamera* pCamera)
{
	if (Find_Camera(pCamTag))
		return;

	Safe_AddRef(pCamera);
	m_CameraList.emplace(pCamTag, pCamera);
}

void CCamera_Manager::Remove_Camera(const wstring& pCamTag)
{
	CCamera* pCamera = nullptr;
	if (pCamera = Find_Camera(pCamTag))
		return;

	Safe_Release(pCamera);
	m_CameraList.erase(pCamTag);
}

void CCamera_Manager::Free(void)
{
	for (auto& item : m_CameraList)
	{
		Safe_Release(item.second);
	}

	m_CameraList.clear();
}