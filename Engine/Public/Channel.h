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