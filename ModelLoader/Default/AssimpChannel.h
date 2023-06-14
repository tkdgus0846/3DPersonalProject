#pragma once

#include "AssimpModel.h"

/* 뼈.(특정 애니메이션에서 사용되는 뼈) */
/* 애니메이션의 단계별 재생 시간에 따른 상태!!(행렬 == 키프레임) */
/* */

class CAssimpChannel : public IWriteable
{
public:
	CAssimpChannel();
	virtual ~CAssimpChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const CAssimpModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CAssimpModel::BONES& Bones, _double TimeAcc, _uint* pCurrentKeyFrameIndex);

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;	
	_uint				m_iBoneIndex = { 0 };

public:
	static CAssimpChannel* Create(const aiNodeAnim* pAIChannel, const CAssimpModel::BONES& Bones);

	// IWriteable을(를) 통해 상속됨
	virtual HRESULT Save_Data(ParsingData* data) override;
};
