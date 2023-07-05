#pragma once

#include "Component.h"

BEGIN(Engine)

class CBlackBoard;
class CDecorator;

class ENGINE_DLL CBehavior : public CBase
{
	friend class CBehaviorTree;
public:
	enum NODERET
	{
		SUCCESS, FAIL, RUNNING
	};
	struct RESULT
	{
		RESULT(NODERET _eRet) : eRet(_eRet), LastRunningList{} {}
		RESULT() {}
		NODERET			eRet;
		vector<CBehavior*> LastRunningList;
	};
protected:
	CBehavior() = default;
	virtual ~CBehavior() = default;

public:
	virtual RESULT	Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList = nullptr) PURE;
	void			AddNode(CBehavior* pNode);
	void			Set_BlackBoard(CBlackBoard* pBlackBoard);
	CBehavior*		Get_Parent() { return m_pParent; }
	void			Add_Decorator(CDecorator* deco);
	_bool			Pass_Decorator();
	
protected:
	vector<CBehavior*>					m_ChildList;
	vector<CDecorator*>					m_DecoratorList;

	CBlackBoard*					m_pBlackBoard = { nullptr };
	CBehavior*						m_pParent = { nullptr };
	

protected:
	virtual void Free() override;
};

END

