#pragma once

#include "Base.h"


BEGIN(Engine)

class ENGINE_DLL CBlackBoard : public CBase
{
protected:
	CBlackBoard();
	virtual ~CBlackBoard() = default;

public:
	void			ClearData();
	void			AddData(const string& name, any Data);
	void			ChangeData(const string& name, any Data);

	any FindData(const string& name)
	{
		//// 그냥 int 로 해도 알아서 포인터로 된다.
		return m_DataMap[name];

	}
	static CBlackBoard* Create();
	virtual void Free() override;
	
protected:
	unordered_map<string, any> m_DataMap;

	
};

END