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

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _float TimeDelta)
{
	// 끝났으면 무조건 리턴하게 하면 안된다. 끝나고 포즈를 유지해야하는 경우가 있기 때문이다.
	if (m_isFinished == true) return;
	
	if (m_isControlManual == false)
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
			return;
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

void CAnimation::Invalidate_TransformationMatrix_Upper(CModel::BONES& Bones, _float TimeDelta, const unordered_set<_int>& UpperSet)
{
	// 끝났으면 무조건 리턴하게 하면 안된다. 끝나고 포즈를 유지해야하는 경우가 있기 때문이다.
	if (m_isFinished == true) return;

	if (m_isControlManual == false)
		m_TimeAcc += m_TickPerSecond * TimeDelta;

	//cout << m_TimeAcc << endl;

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

		_uint boneIndex = pChannel->Get_BoneIndex();

		// 상체 인덱스에 대해서만 Invalidate 함수 부른다.
		if (UpperSet.find(boneIndex) != UpperSet.end())
		{
			pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
		}
	}
}

void CAnimation::Invalidate_TransformationMatrix_Lower(CModel::BONES& Bones, _float TimeDelta, const unordered_set<_int>& LowerSet)
{
	// 끝났으면 무조건 리턴하게 하면 안된다. 끝나고 포즈를 유지해야하는 경우가 있기 때문이다.
	if (m_isFinished == true) return;

	if (m_isControlManual == false)
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

		_uint boneIndex = pChannel->Get_BoneIndex();

		// 하체 인덱스에 대해서만 Invalidate 함수 부른다.
		if (LowerSet.find(boneIndex) != LowerSet.end())
		{
			pChannel->Invalidate_TransformationMatrix(Bones, m_TimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
		}
	}
}

_int CAnimation::Lerp_NextAnimation(CAnimation* pNextAnimation, CModel::BONES& Bones, _float Duration, _float LerpTimeAcc, unordered_set<_int>* BoneIndex)
{
	_int iChannelIndex = 0;
	_int iResult = 1;

	// 디버그 용이니까 지워라.
	_int debugSrcFrame = 0;
	

	for (auto& pPrevChannel : m_Channels)
	{
		if (nullptr == pPrevChannel) 
			return -1;

		for (auto& pNextChannel : pNextAnimation->m_Channels)
		{
			if (nullptr == pNextChannel) 
				return -1;

			if ((pPrevChannel->Get_BoneIndex() == pNextChannel->Get_BoneIndex()) && (BoneIndex == nullptr || (BoneIndex->find(pPrevChannel->Get_BoneIndex()) != BoneIndex->end() && BoneIndex->find(pNextChannel->Get_BoneIndex()) != BoneIndex->end())))
			{
				// 디버그용
				/*if (BoneIndex != nullptr)
					cout << "디버그 진입!" << endl;*/
				if (debugSrcFrame < m_ChannelCurrentKeyFrames[iChannelIndex])
					debugSrcFrame = m_ChannelCurrentKeyFrames[iChannelIndex];

				_int result = pPrevChannel->Lerp_TransformaitionMatrix(Bones, Duration, LerpTimeAcc, m_ChannelCurrentKeyFrames[iChannelIndex], pNextChannel->Get_FirstKeyFrame());

				//cout << "Hit " << result << " BoneIndex:" << BoneIndex << " LerpTime" << LerpTimeAcc<< endl;

				if (result == 0)
					iResult = 0;
			}
				
		}
		iChannelIndex++;
	}

	// 가장 컸던 소스 키프레임이 0 이라는 말은 첫번째와 보간하는게 맞음.
	//cout << "가장 컸던 소스 키프레임 " << debugSrcFrame << endl;
	if (debugSrcFrame == 0)
		int i = 2;

	return iResult;
}

void CAnimation::Erase_LastFrame_Animation()
{
	_float lastTime = -1.f;
	for (auto& channel : m_Channels)
	{
		_float tmpTime = channel->Erase_LastFrame();
		if (lastTime < tmpTime)
			lastTime = tmpTime;
	}
	m_Duration = lastTime;
}

void CAnimation::Erase_Frames_LessTime(_float time)
{
	_float lastTime = -1.f;
	for (auto& channel : m_Channels)
	{
		_float tmpTime = channel->Erase_Frames_LessTime(time);
		if (lastTime < tmpTime)
			lastTime = tmpTime;
	}
	m_Duration = lastTime;

	//// 내가 지웠던 가장 큰 프레임의 시간 그 다음으로 가장 작은 프레임을 찾아서 첫번째 프레임으로 만들어준다. 
	//_float lastTime = 99999.f;
	//for (auto& channel : m_Channels)
	//{
	//	_float tmpTime = channel->Erase_Frames_LessTime(time);
	//	if (lastTime > tmpTime)
	//		lastTime = tmpTime;
	//}

	//if (time > lastTime)
	//	m_Duration -= lastTime;

	//for (auto& channel : m_Channels)
	//{
	//	channel->Change_Start_Frame(lastTime);
	//}

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

