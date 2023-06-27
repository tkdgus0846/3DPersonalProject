#pragma once

#include "Model.h"

/* 특정(Idle, Attack, Run) 애니메이션을 구동하기위해 필요한 정보를 가지낟.  */
/* 구동하기위해 필요한 정보 
: 사용하고있는 뼈의 갯수m_iNumChannels, 각 뼈가 시간에따라 어떤 상태를 취해야하는지?!
: 특정(Idle, Attack, Run) 애니메이션을 구동하는데 걸리는 총 시간. Duration
: 특정(Idle, Attack, Run) 애니메이션의 재생속도. TickPerSecond * TimeDelta 
: 애니메이션의 이름. 
*/

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase, public IReadable
{
public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ParsingData* pData);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);
	_int Lerp_NextAnimation(CAnimation* pNextAnimation, CModel::BONES & Bones, _double Duration, _double LerpTimeAcc);
	void Reset_Channels()
	{
		for (int i = 0; i < m_ChannelCurrentKeyFrames.size(); i++)
			m_ChannelCurrentKeyFrames[i] = 0;

		m_TimeAcc = 0.f;
	}

	_char*	Get_Name() { return m_szName; }
	_bool	Get_Loop() const { return m_isLoop; }
	void	Set_Loop(_bool state) { m_isLoop = state; }
	_bool	Get_Finished() { return m_isFinished; }

private:
	char						m_szName[MAX_PATH];
	_uint						m_iNumChannels = { 0 };
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_ChannelCurrentKeyFrames;	
	_double						m_Duration = { 0.0 };	
	_double						m_TickPerSecond = { 0.0 };
	_double						m_TimeAcc = { 0.0 };

	_bool						m_isFinished = { false };
	_bool						m_isLoop = { false };

public:
	static CAnimation* Create(ParsingData* pData);
	CAnimation* Clone();
	virtual void Free();

	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final;
};

END