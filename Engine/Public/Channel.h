#pragma once

#include "Model.h"

/* ��.(Ư�� �ִϸ��̼ǿ��� ���Ǵ� ��) */
/* �ִϸ��̼��� �ܰ躰 ��� �ð��� ���� ����!!(��� == Ű������) */
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