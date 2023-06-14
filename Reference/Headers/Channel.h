#pragma once

#include "Model.h"

/* 뼈.(특정 애니메이션에서 사용되는 뼈) */
/* 애니메이션의 단계별 재생 시간에 따른 상태!!(행렬 == 키프레임) */
/* */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeAcc, _uint* pCurrentKeyFrameIndex);

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;	
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END