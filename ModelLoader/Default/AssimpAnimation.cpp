#include "stdafx.h"
#include "AssimpAnimation.h"
#include "AssimpChannel.h"

CAssimpAnimation::CAssimpAnimation()
	: m_isLoop(true)
{
}

CAssimpAnimation::~CAssimpAnimation()
{
	for (auto& pChannel : m_Channels)
		Safe_Delete(pChannel);

	m_Channels.clear();
}

HRESULT CAssimpAnimation::Initialize(const aiAnimation * pAIAnimation, const CAssimpModel::BONES& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CAssimpChannel*		pChannel = CAssimpChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_ChannelCurrentKeyFrames.resize(m_iNumChannels);

	return S_OK;
}

void CAssimpAnimation::Invalidate_TransformationMatrix(CAssimpModel::BONES& Bones, _double TimeDelta)
{
	m_TimeAcc += m_TickPerSecond * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		if (true == m_isLoop)
		{
			m_TimeAcc = 0.f;
		}
		else
			m_isFinished = true;
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

CAssimpAnimation * CAssimpAnimation::Create(const aiAnimation * pAIAnimation, const CAssimpModel::BONES& Bones)
{
	CAssimpAnimation*	pInstance = new CAssimpAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed to Created CAssimpAnimation");
		Safe_Delete(pInstance);
	}
	return pInstance;
}

HRESULT CAssimpAnimation::Save_Data(ParsingData* data)
{
	ModelParsingData* myData = (ModelParsingData*)&data;
	AnimationParsingData AnimData;

	AnimData.ChannelDatas.resize(m_Channels.size());

	for (int index = 0; index < m_Channels.size(); index++)
	{
		m_Channels[index]->Save_Data(&AnimData.ChannelDatas[index]);
	}
	strcpy(AnimData.szName, m_szName);
	AnimData.iNumChannels = m_iNumChannels;
	AnimData.Duration = m_Duration;
	AnimData.TickPerSecond = m_TickPerSecond;

	myData->AnimationDatas.push_back(AnimData);
	return S_OK;
}

