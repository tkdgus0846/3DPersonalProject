#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
	: m_isLoop(true)
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_ChannelCurrentKeyFrames(rhs.m_ChannelCurrentKeyFrames)
	, m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_TimeAcc(rhs.m_TimeAcc)
	, m_isFinished(rhs.m_isFinished)
	, m_isLoop(rhs.m_isLoop)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(ParsingData* pData)
{
	AnimationParsingData* data = (AnimationParsingData*)pData;
	strcpy(m_szName, data->szName);

	m_Duration = data->Duration;
	m_TickPerSecond = data->TickPerSecond;

	m_iNumChannels = data->iNumChannels;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*		pChannel = CChannel::Create(&data->ChannelDatas[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_ChannelCurrentKeyFrames.resize(m_iNumChannels);

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta)
{
	if (m_isFinished == true) return;

	m_TimeAcc += m_TickPerSecond * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		if (true == m_isLoop)
		{
			m_TimeAcc = 0.f;
		}
		else
		{
			m_isFinished = true;
			m_TimeAcc = 0.f;
		}
			
	}



	/* 현재 재생된 시간에 맞도록 모든 뼈의 상태를 키프레임정보를 기반으로하여 갱신한다. */
	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
	}


}

_int CAnimation::Lerp_NextAnimation(CAnimation* pNextAnimation, CModel::BONES& Bones, _double Duration, _double LerpTimeAcc)
{
	_int iChannelIndex = 0;
	_int iResult = 1;

	for (auto& pPrevChannel : m_Channels)
	{
		if (nullptr == pPrevChannel) 
			return -1;

		for (auto& pNextChannel : pNextAnimation->m_Channels)
		{
			if (nullptr == pNextChannel) 
				return -1;

			if (pPrevChannel->Get_BoneIndex() == pNextChannel->Get_BoneIndex())
			{
				cout << pPrevChannel->Get_BoneIndex() << endl;
				_int result;
				
				result = pPrevChannel->Lerp_TransformaitionMatrix(Bones, Duration, LerpTimeAcc, m_ChannelCurrentKeyFrames[iChannelIndex], pNextChannel->Get_FirstKeyFrame());

				if (result == 0)
					iResult = 0;
			}
				
		}
		iChannelIndex++;
	}

	return iResult;
}

CAnimation * CAnimation::Create(ParsingData* pData)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}

ParsingData* CAnimation::Load_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = 0;
	char str[MAX_PATH];
	ModelParsingData* myData = (ModelParsingData*)data;
	AnimationParsingData AnimData;

	ReadFile(handle, &AnimData.iNumChannels, sizeof(AnimData.iNumChannels), &dwByte, nullptr);

	for (int i = 0; i < AnimData.iNumChannels; i++)
	{	
		ChannelParsingData ChannelData;
		ReadFile(handle, &str, sizeof(str), &dwByte, nullptr);
		strcpy(ChannelData.szName, str);

		ReadFile(handle, &ChannelData.iNumKeyFrames, sizeof(ChannelData.iNumKeyFrames), &dwByte, nullptr);

		for (int i = 0; i < ChannelData.iNumKeyFrames; i++)
		{
			KEYFRAME keyFrame;
			ReadFile(handle, &keyFrame, sizeof(keyFrame), &dwByte, nullptr);
			ChannelData.KeyFrames.push_back(keyFrame);
		}
		
		ReadFile(handle, &ChannelData.iBoneIndex, sizeof(ChannelData.iBoneIndex), &dwByte, nullptr);

		AnimData.ChannelDatas.push_back(ChannelData);
	}

	ReadFile(handle, &str, sizeof(str), &dwByte, nullptr);
	strcpy(AnimData.szName, str);

	ReadFile(handle, &AnimData.Duration, sizeof(AnimData.Duration), &dwByte, nullptr);
	ReadFile(handle, &AnimData.TickPerSecond, sizeof(AnimData.TickPerSecond), &dwByte, nullptr);

	myData->AnimationDatas.push_back(AnimData);
	return S_OK;
}

