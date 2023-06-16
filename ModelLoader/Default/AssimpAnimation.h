#pragma once

#include "AssimpModel.h"

/* Ư��(Idle, Attack, Run) �ִϸ��̼��� �����ϱ����� �ʿ��� ������ ������.  */
/* �����ϱ����� �ʿ��� ���� 
: ����ϰ��ִ� ���� ����m_iNumChannels, �� ���� �ð������� � ���¸� ���ؾ��ϴ���?!
: Ư��(Idle, Attack, Run) �ִϸ��̼��� �����ϴµ� �ɸ��� �� �ð�. Duration
: Ư��(Idle, Attack, Run) �ִϸ��̼��� ����ӵ�. TickPerSecond * TimeDelta 
: �ִϸ��̼��� �̸�. 
*/

class CAssimpAnimation : public IWriteable
{
public:
	CAssimpAnimation();
	~CAssimpAnimation();

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const CAssimpModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CAssimpModel::BONES& Bones, _double TimeDelta);

private:
	char						m_szName[MAX_PATH];
	_uint						m_iNumChannels = { 0 };
	vector<class CAssimpChannel*>		m_Channels;
	vector<_uint>				m_ChannelCurrentKeyFrames;	
	_double						m_Duration = { 0.0 };	
	_double						m_TickPerSecond = { 0.0 };
	_double						m_TimeAcc = { 0.0 };

	_bool						m_isFinished = { false };
	_bool						m_isLoop = { false };

public:
	static CAssimpAnimation* Create(const aiAnimation* pAIAnimation, const CAssimpModel::BONES& Bones);


	// IWriteable��(��) ���� ��ӵ�
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) override;

};