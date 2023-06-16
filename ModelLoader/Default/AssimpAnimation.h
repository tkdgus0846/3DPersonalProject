#pragma once

#include "AssimpModel.h"

/* 특정(Idle, Attack, Run) 애니메이션을 구동하기위해 필요한 정보를 가지낟.  */
/* 구동하기위해 필요한 정보 
: 사용하고있는 뼈의 갯수m_iNumChannels, 각 뼈가 시간에따라 어떤 상태를 취해야하는지?!
: 특정(Idle, Attack, Run) 애니메이션을 구동하는데 걸리는 총 시간. Duration
: 특정(Idle, Attack, Run) 애니메이션의 재생속도. TickPerSecond * TimeDelta 
: 애니메이션의 이름. 
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


	// IWriteable을(를) 통해 상속됨
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) override;

};