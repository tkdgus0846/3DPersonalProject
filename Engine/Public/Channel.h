#pragma once

#include "Model.h"

/* 뼈.(특정 애니메이션에서 사용되는 뼈) */
/* 애니메이션의 단계별 재생 시간에 따른 상태!!(행렬 == 키프레임) */
/* */

BEGIN(Engine)

class CChannel final : public CBase, public IReadable
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(ParsingData* pData);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeAcc, _uint* pCurrentKeyFrameIndex);
	_int Lerp_TransformaitionMatrix(CModel::BONES& Bones, _double duration, _double TimeAcc, _uint srcKeyFrame, KEYFRAME& dstKeyFrame);

	_uint Get_BoneIndex() { return m_iBoneIndex; }
	KEYFRAME& Get_FirstKeyFrame() { return m_KeyFrames.front(); }
	_double Erase_LastFrame();
	_double Erase_Frames_LessTime(_double time);

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;	
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(ParsingData* pData);
	virtual void Free() override;

	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final;
};

END