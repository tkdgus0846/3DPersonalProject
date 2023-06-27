#pragma once

#include "Model.h"

/* Ư��(Idle, Attack, Run) �ִϸ��̼��� �����ϱ����� �ʿ��� ������ ������.  */
/* �����ϱ����� �ʿ��� ���� 
: ����ϰ��ִ� ���� ����m_iNumChannels, �� ���� �ð������� � ���¸� ���ؾ��ϴ���?!
: Ư��(Idle, Attack, Run) �ִϸ��̼��� �����ϴµ� �ɸ��� �� �ð�. Duration
: Ư��(Idle, Attack, Run) �ִϸ��̼��� ����ӵ�. TickPerSecond * TimeDelta 
: �ִϸ��̼��� �̸�. 
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