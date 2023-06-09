#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(ParsingData* pData)
{
	ChannelParsingData* data = (ChannelParsingData*)pData;

	strcpy(m_szName, data->szName);

	m_iBoneIndex = data->iBoneIndex;

	m_iNumKeyFrames = data->iNumKeyFrames;

	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		m_KeyFrames.push_back(data->KeyFrames[i]);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(CModel::BONES& Bones, _float TimeAcc, _uint* pCurrentKeyFrameIndex)
{
	if (0.0 == TimeAcc)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;

	if (TimeAcc >= LastKeyFrame.Time)
	{
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;
	}
	else /* 현재 존재하는 키프레임의 상태를 좌우 키프레임정보를 이용하여 선형보간한다. */
	{
		if ((*pCurrentKeyFrameIndex) + 1 >= m_KeyFrames.size()) return;

		while (TimeAcc >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].Time)
			++(*pCurrentKeyFrameIndex);

		_float		Ratio = (TimeAcc - m_KeyFrames[(*pCurrentKeyFrameIndex)].Time) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].Time - m_KeyFrames[(*pCurrentKeyFrameIndex)].Time);

		_float3		vSourScale = m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale;
		_float4		vSourRotation = m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
		_float3		vSourTranslation = m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

		_float3		vDestScale = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
		_float4		vDestRotation = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
		_float3		vDestTranslation = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), (_float)Ratio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), (_float)Ratio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), (_float)Ratio));
	}

	/* 진행된 시간에 맞는 뼈의 행렬을 만들어낸다. */
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat3(&vTranslation));

	/* 같은 이름을 가진 모델이 들고 있는 뼈에게 전달해준다. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

_int CChannel::Lerp_TransformaitionMatrix(CModel::BONES& Bones, _float duration, _float TimeAcc, _uint srcKeyFrame, KEYFRAME& dstKeyFrame)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;

	_float		Ratio = TimeAcc/duration;

	

	if (Ratio >= 1.f) 
		return 1;

	if (srcKeyFrame >= m_KeyFrames.size()) 
		srcKeyFrame = m_KeyFrames.size() - 1;

	_float3		vSourScale = m_KeyFrames[srcKeyFrame].vScale;
	_float4		vSourRotation = m_KeyFrames[srcKeyFrame].vRotation;
	_float3		vSourTranslation = m_KeyFrames[srcKeyFrame].vTranslation;

	_float3		vDestScale = dstKeyFrame.vScale;
	_float4		vDestRotation = dstKeyFrame.vRotation;
	_float3		vDestTranslation = dstKeyFrame.vTranslation;

	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), (_float)Ratio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), (_float)Ratio));
	XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), (_float)Ratio));

	/* 진행된 시간에 맞는 뼈의 행렬을 만들어낸다. */
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat3(&vTranslation));

	/* 같은 이름을 가진 모델이 들고 있는 뼈에게 전달해준다. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	
	return 0;
}

_float CChannel::Erase_LastFrame()
{
	if (m_KeyFrames.size() < 1) return 0.0;

	m_KeyFrames.erase(m_KeyFrames.end() - 1);
	m_iNumKeyFrames -= 1;

	if (m_KeyFrames.empty())
	{
		m_iNumKeyFrames = 0;
		return 0.0;
	}
	return m_KeyFrames.back().Time;
}

_float CChannel::Erase_Frames_LessTime(_float time)
{
	
	for (auto it = m_KeyFrames.rbegin(); it != m_KeyFrames.rend();)
	{
		if (time < it->Time)
		{
			it = reverse_iterator(m_KeyFrames.erase(std::next(it).base()));
			m_iNumKeyFrames -= 1;
		}
		else
			break;
		
	}
	return m_KeyFrames.back().Time;

	/*_float fResultTime = -1.f;
	for (auto it = m_KeyFrames.begin(); it != m_KeyFrames.end();)
	{
		if (time > it->Time)
		{
			fResultTime = it->Time;
			it = m_KeyFrames.erase(it);
			m_iNumKeyFrames -= 1;
		}
		else
			break;

	}
	return m_KeyFrames.front().Time;*/
}

void CChannel::Change_Start_Frame(_float time)
{
	for (auto& item : m_KeyFrames)
	{
		item.Time -= time;
	}
}

CChannel * CChannel::Create(ParsingData* pData)
{
	CChannel*	pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Created CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}

ParsingData* CChannel::Load_Data(HANDLE handle, ParsingData* data)
{
	return nullptr;
}
