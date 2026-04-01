


#pragma once
#include "include\sqlite3.h"
class CDatabaseControl
{
	//保存临时字符串
	typedef std::vector<CString> VECT_STRING;
public:
	CDatabaseControl();
	~CDatabaseControl();
	sqlite3 * GetInstance(CString strDbFilepath);
protected:
	sqlite3 *m_pdb;
public:
	void WriteDataBase(int nCommandType, void * pData, void * pReserved);
	void ReadDataBase(int nCommandType, void * pInputData, void * pOutputData , void * pReserved);
	//数据库整理
	BOOL VacuumDataBase(CString strDbPath);

protected:
	void SaveProductInfo(void * pData, void * pReserved);
	void LoadProductInfo(void * pInputData, void * pOutputData, void * pReserved);

	void SaveFireRecord(void* pData, void* pReserved);
	void LoadFireRecord(void* pInputData, void* pOutputData, void* pReserved);

	void SaveOpenCloseFlowInfo(void* pData, void* pReserved);
	void LoadOpenCloseFlowInfo(void* pInputData, void* pOutputData, void* pReserved);

public:
	BOOL TableIsExist(const TCHAR * strTableName);
	BOOL TableHasDataRecords(const TCHAR * strTableName);
	VECT_PROGRAMITEM GetProgramItem(int nProgramID);
};

