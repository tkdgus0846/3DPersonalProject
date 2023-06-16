#include "stdafx.h"
#include "AssimpChannel.h"
#include "AssimpBone.h"

CAssimpChannel::CAssimpChannel()
{
}

HRESULT CAssimpChannel::Initialize(const aiNodeAnim * pAIChannel, const CAssimpModel::BONES& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	// ���� ��� �ִ� ���� �̸��� ���� ���� ã����. 
	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CAssimpBone* pValue)
	{
		if (0 != strcmp(m_szName, pValue->Get_Name()))
		{
			++m_iBoneIndex;
			return false;
		}
		else
			return true;
	});

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;

	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME				Keyframe;

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			Keyframe.Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;			
			Keyframe.Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			Keyframe.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		Keyframe.vScale = vScale;
		Keyframe.vRotation = vRotation;
		Keyframe.vTranslation = vTranslation;

		m_KeyFrames.push_back(Keyframe);
	}

	return S_OK;
}

void CAssimpChannel::Invalidate_TransformationMatrix(CAssimpModel::BONES& Bones, _double TimeAcc, _uint* pCurrentKeyFrameIndex)
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
	else /* ���� �����ϴ� Ű�������� ���¸� �¿� Ű������������ �̿��Ͽ� ���������Ѵ�. */
	{
		while (TimeAcc >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].Time)
			++(*pCurrentKeyFrameIndex);

		_double		Ratio = (TimeAcc - m_KeyFrames[(*pCurrentKeyFrameIndex)].Time) /
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

	/* ����� �ð��� �´� ���� ����� ������. */
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat3(&vTranslation));

	/* ���� �̸��� ���� ���� ��� �ִ� ������ �������ش�. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CAssimpChannel * CAssimpChannel::Create(const aiNodeAnim * pAIChannel, const CAssimpModel::BONES& Bones)
{
	CAssimpChannel*	pInstance = new CAssimpChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed to Created CAssimpChannel");
		Safe_Delete(pInstance);
	}
	return pInstance;
}

ParsingData* CAssimpChannel::Save_Data(HANDLE handle, ParsingData* data)
{
	ChannelParsingData* myData = (ChannelParsingData*)data;

	strcpy(myData->szName, m_szName);
	myData->iNumKeyFrames = m_iNumKeyFrames;
	myData->KeyFrames = m_KeyFrames;
	myData->iBoneIndex = m_iBoneIndex;

	return nullptr;
}
