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
	friend class CAnimInstance;
public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ParsingData* pData);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _float TimeDelta);
	void Invalidate_TransformationMatrix_Upper(CModel::BONES & Bones, _float TimeDelta, const unordered_set<_int>& UpperSet);
	void Invalidate_TransformationMatrix_Lower(CModel::BONES & Bones, _float TimeDelta, const unordered_set<_int>& LowerSet);

	_int Lerp_NextAnimation(CAnimation* pNextAnimation, CModel::BONES & Bones, _float Duration, _float LerpTimeAcc, unordered_set<_int>*BoneIndex);
	void Reset_Channels()
	{
		string str = m_szName;
		if (str.compare("VampireMale_Axes_Combo01_MoveCast_Exec") == 0)
			int sibal = 2;

		// 디버그 용으로 넣어놨다.
		for (int i = 0; i < m_ChannelCurrentKeyFrames.size(); i++)
			m_ChannelCurrentKeyFrames[i] = 0;

		m_TimeAcc = 0.f;
		m_isFinished = false;
	}
	_bool Is_Reseted()
	{
		_bool bResult = false;

		for (int i = 0; i < m_ChannelCurrentKeyFrames.size(); i++)
			bResult = (m_ChannelCurrentKeyFrames[i] == 0);

		return bResult;
	}

	_char*	Get_Name() { return m_szName; }
	_bool	Get_Loop() const { return m_isLoop; }
	void	Set_Loop(_bool state) { m_isLoop = state; }
	_bool	Get_Finished() { return m_isFinished; }
	void	Erase_LastFrame_Animation();
	void	Erase_Frames_LessTime(_float time);
	void	Set_ControlManual(_bool state) { m_isControlManual = state; }
	void	Set_TimeAcc(double time) { m_TimeAcc = time; }
	double Get_TimeAcc() const { return m_TimeAcc; }
	double Get_Duration() const { return m_Duration; }
	vector<_uint>* Get_CurKeyframes() { return &m_ChannelCurrentKeyFrames; }

private:
	char						m_szName[MAX_PATH];
	_uint						m_iNumChannels = { 0 };
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_ChannelCurrentKeyFrames;	
	double						m_Duration = { 0.0 };	
	double						m_TickPerSecond = { 0.0 };
	double						m_TimeAcc = { 0.0 };

	_bool						m_isFinished = { false };
	_bool						m_isLoop = { false };
	_bool						m_isControlManual = { false };

public:
	static CAnimation* Create(ParsingData* pData);
	CAnimation* Clone();
	virtual void Free();

	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final;
};

END