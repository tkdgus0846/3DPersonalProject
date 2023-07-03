#include "..\Public\BlackBoard.h"

CBlackBoard::CBlackBoard()
{
	
}

void CBlackBoard::AddData(const string& name, any Data)
{
	if (m_DataMap.find(name) == m_DataMap.end())
		m_DataMap[name] = Data;
}

void CBlackBoard::ChangeData(const string& name, any Data)
{
	if (m_DataMap.find(name) != m_DataMap.end())
		m_DataMap[name] = Data;	
}

CBlackBoard* CBlackBoard::Create()
{
	return new CBlackBoard;
}

void CBlackBoard::Free()
{
}

