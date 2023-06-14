#pragma once

#include "AssimpModel.h"

/* ��.(Ư�� �ִϸ��̼ǿ��� ���Ǵ� ��) */
/* �ִϸ��̼��� �ܰ躰 ��� �ð��� ���� ����!!(��� == Ű������) */
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

	// IWriteable��(��) ���� ��ӵ�
	virtual HRESULT Save_Data(ParsingData* data) override;
};
