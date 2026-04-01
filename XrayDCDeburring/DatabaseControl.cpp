#include "pch.h"
#include "DatabaseControl.h"


//历史数据表，参数除外的列数

#ifdef LANGUAGE_ENGLISH
#define TABLENAME_DETECTHEADER "VirtualDetectHeader"
#define TABLENAME_PROGRAMS "DetectPrograms"
#define TABLENAME_COMM "CommSet"
#define TABLENAME_COMM_DETECTHEADERS "CommSet_DetectHeader"
#else

#define TABLENAME_PRODUCTINFO_TABLE       _T("产品表")
#define TABLENAME_PROGRAMITEM_TABLE         _T("程序项目表")
#define TABLENAME_FIRERECORD_TABLE         _T("打火记录表")
#endif

//脚本中可能存在需要转换的字符，未处理由数据表读出后的字符回转问题。
static CString sqliteEscape(CString keyWord)
{
	//keyWord.Replace('/', '//');
	keyWord.Replace('\'', '"');
	//keyWord.Replace('[', '/[');
	//keyWord.Replace(']', '/]');
	//keyWord.Replace('%', '/%');
	//keyWord.Replace('&', '/&');
	//keyWord.Replace('_', '/_');
	//keyWord.Replace('(', '/(');
	//keyWord.Replace(')', '/)');
	return keyWord;
}





CDatabaseControl::CDatabaseControl()
{
	m_pdb = NULL;
}


CDatabaseControl::~CDatabaseControl()
{
	if (m_pdb != NULL)
	{
		sqlite3_close(m_pdb);
	}
}


sqlite3 * CDatabaseControl::GetInstance(CString strDbFilepath)
{
	if (m_pdb == NULL)
	{
		DWORD nLongth = strDbFilepath.GetLength()*2+1;
		char * pDbPath = new char[(DWORD)nLongth];

		memset(pDbPath, 0, nLongth);
		WCharToUTF8(strDbFilepath, pDbPath, nLongth);

		int rc;
		rc = sqlite3_open(pDbPath, &m_pdb);
		if (pDbPath != NULL)
		{
			delete[] pDbPath;
			pDbPath = NULL;
		}
		
		if (rc)
		{
			sqlite3_close(m_pdb);
			return NULL;
		}		
		char* errmsg = NULL;
		//配置执行动作属性
		int rt = sqlite3_exec(m_pdb, "PRAGMA synchronous = OFF; ", 0, 0, &errmsg);//NORMAL,FULL 
		//压缩数据库，释放无效区域给操作系统，会耗时较长（实际为一项功能，建议备份前执行一次） PRAGMA auto_vacuum = 1;
		//建议：根据原则选择部分数据以及数据表复制到新创建的数据库中，作为后续使用对象，然后对当前数据库执行压缩，然后备份保存。
		//设置合理的 page_size 和 cache_size---文档：pragma_page_size
		//PRAGMA schema.page_size = bytes;
		//PRAGMA schema.cache_size = pages;

		if (rt != SQLITE_OK)
		{
			AfxMessageBox(L"sqlite init failed.");//初始化失败。
			sqlite3_free(errmsg);
			return m_pdb;
		}
		
	}
	return m_pdb;
}



void CDatabaseControl::WriteDataBase(int nCommandType, void *pData, void * pReserved)
{
	if (m_pdb == NULL || pData == NULL)
	{
		return;
	}
    if (nCommandType == COMMAND_TYPE_PRODUCTINFO_SAVE)
	{
		SaveProductInfo(pData, pReserved);
	}
	else if (nCommandType == COMMAND_TYPE_FIRERECORD_SAVE)
	{
		SaveFireRecord(pData, pReserved);
	}

	else if (nCommandType == COMMAND_TYPE_OPENCLOSETUBE_SAVE)
	{
		SaveOpenCloseFlowInfo(pData, pReserved);
	}

}


void CDatabaseControl::ReadDataBase(int nCommandType, void * pInputData, void * pOutputData, void * pReserved)
{
	if (m_pdb == NULL || pOutputData==NULL)
	{
		return;
	}
    if (nCommandType == COMMAND_TYPE_PRODUCTINFO_LOAD)
	{
		LoadProductInfo(pInputData, pOutputData, pReserved);
	}
	else if (nCommandType == COMMAND_TYPE_FIRERECORD_LOAD)
	{
		LoadFireRecord(pInputData, pOutputData, pReserved);
	} 
	else if (nCommandType == COMMAND_TYPE_OPENCLOSETUBE_LOAD)
	{
		LoadOpenCloseFlowInfo(pInputData, pOutputData, pReserved);
	}
}

BOOL CDatabaseControl::VacuumDataBase(CString strDbPath)
{
	if (m_pdb == NULL)
	{
		return FALSE;
	}
	else if (strDbPath.GetLength() <= 0)
	{
		return FALSE;
	}
	else if (-1 == _taccess(strDbPath,0))
	{
		return FALSE;
	}

	int rc;
	char *errmsg = NULL;

	CString strTemp = _T("VACUUM");
	DWORD nLongth = strTemp.GetLength()*2+1;
	char * sSQL = new char[(DWORD)nLongth];
	memset(sSQL, 0, nLongth);
	WCharToUTF8(strTemp, sSQL, nLongth);
	rc = sqlite3_exec(m_pdb, sSQL, 0, 0, &errmsg);
	if (sSQL != NULL)
	{
		delete[] sSQL;
		sSQL = NULL;
	}
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errmsg);
		TRACE(errmsg);
		sqlite3_free(errmsg);
		return FALSE;
	}

	return TRUE;
}

BOOL CDatabaseControl::TableIsExist(const TCHAR * strTableName)
{
	CString s(strTableName);
	CString strTemp = _T("SELECT * FROM ")+s;
	DWORD nLongth = strTemp.GetLength() * 2 + 1;
	char *sql = new char[(DWORD)nLongth];
	memset(sql, 0, nLongth);
	WCharToUTF8(strTemp, sql, nLongth);

	sqlite3_stmt *stmt = NULL;
	int nColumnNum = 0;

	if (sqlite3_prepare_v2(m_pdb, sql, -1, &stmt, NULL) == SQLITE_OK) {

		if (stmt != NULL)
		{
			nColumnNum = sqlite3_column_count(stmt);
		}
		sqlite3_finalize(stmt);
	}
	if (sql != NULL)
	{
		delete[] sql;
	}

	if (nColumnNum <= 0)
	{

	}
	return nColumnNum>0? TRUE:FALSE;
}

BOOL CDatabaseControl::TableHasDataRecords(const TCHAR * strTableName)
{
	CString s(strTableName);
	CString strTemp = _T("SELECT COUNT(*) FROM ") + s;
	char *sql = new char[strTemp.GetLength() * 2 + 1];
	memset(sql, 0, strTemp.GetLength() * 2 + 1);
	WCharToUTF8(strTemp, sql, strTemp.GetLength() * 2);

	sqlite3_stmt *stmt = NULL;
	int nDataRecordNum = 0;

	if (sqlite3_prepare_v2(m_pdb, sql, -1, &stmt, NULL) == SQLITE_OK) {

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			nDataRecordNum = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}
	if (sql != NULL)
	{
		delete[] sql;
	}

	return nDataRecordNum > 0 ? TRUE : FALSE;
}

void  CDatabaseControl::SaveProductInfo(void * pData, void * pReserved)
{
	int rc;
	char *errmsg = NULL;
	if (TableIsExist(TABLENAME_PRODUCTINFO_TABLE))
	{
		//TRACE("此表存在\n");
		CString strTemp = _T("delete from ");
		strTemp += TABLENAME_PRODUCTINFO_TABLE;
		char * sSQL_del = new char[strTemp.GetLength() * 2 + 1];
		memset(sSQL_del, 0, strTemp.GetLength() * 2 + 1);
		WCharToUTF8(strTemp, sSQL_del, strTemp.GetLength() * 2 + 1);
		rc = sqlite3_exec(m_pdb, sSQL_del, 0, 0, &errmsg);
		if (sSQL_del != NULL)
		{
			delete[] sSQL_del;
			sSQL_del = NULL;
		}
		if (rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", errmsg);
			TRACE(errmsg);
			sqlite3_free(errmsg);
			return;
		}
		strTemp = _T("delete from ");
		strTemp += TABLENAME_PROGRAMITEM_TABLE;
	    sSQL_del = new char[strTemp.GetLength() * 2 + 1];
		memset(sSQL_del, 0, strTemp.GetLength() * 2 + 1);
		WCharToUTF8(strTemp, sSQL_del, strTemp.GetLength() * 2 + 1);
		rc = sqlite3_exec(m_pdb, sSQL_del, 0, 0, &errmsg);
		if (sSQL_del != NULL)
		{
			delete[] sSQL_del;
			sSQL_del = NULL;
		}
		if (rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", errmsg);
			TRACE(errmsg);
			sqlite3_free(errmsg);
			return;
		}


		VECT_PROGRAM *pVectDetect = (VECT_PROGRAM*)pData;
		for (int i = 0; i < pVectDetect->size(); i++)
		{
			CString strSqlExec = _T("");
			strSqlExec.Format(_T("insert into %s values(NULL,'%d','%s','%.*f','%d','%d','%d','%s','%s');"),
				TABLENAME_PRODUCTINFO_TABLE,
				pVectDetect->at(i).nProgramID,
				pVectDetect->at(i).strProgramName,
				DOT_BITNUMBER,
				pVectDetect->at(i).fHightVoltage_Limit,
				pVectDetect->at(i).nLowValtageLimit,
				pVectDetect->at(i).nLowVoltage,
				pVectDetect->at(i).nLowCurrent,
				pVectDetect->at(i).strMemo,
				pVectDetect->at(i).strDataSavePath);
			char * cSqlExec = new char[strSqlExec.GetLength() * 3 + 1];
			if (cSqlExec == NULL)
			{
				return;
			}
			memset(cSqlExec, 0, strSqlExec.GetLength() * 3 + 1);
			WCharToUTF8(strSqlExec, cSqlExec, strSqlExec.GetLength() * 3 + 1);

			if (SQLITE_OK != sqlite3_exec(m_pdb, cSqlExec, 0, 0, &errmsg))
			{
				fprintf(stderr, "SQL error: %s\n", errmsg);
				TRACE(errmsg);
				sqlite3_free(errmsg);
			}
			delete[] cSqlExec;

			for (int j = 0; j < pVectDetect->at(i).VectProgItem.size(); j++)
			{
				CString strSqlExec = _T("");
				strSqlExec.Format(_T("insert into %s values(NULL,'%d','%d','%.*f','%d','%s');"),
					TABLENAME_PROGRAMITEM_TABLE,
					pVectDetect->at(i).nProgramID,
					pVectDetect->at(i).VectProgItem.at(j).nStep,		
					DOT_BITNUMBER,
					pVectDetect->at(i).VectProgItem.at(j).nHightVotage,
					pVectDetect->at(i).VectProgItem.at(j).nKeepTime,
					pVectDetect->at(i).VectProgItem.at(j).strMemo);
				char* cSqlExec = new char[strSqlExec.GetLength() * 3 + 1];
				if (cSqlExec == NULL)
				{
					return;
				}
				memset(cSqlExec, 0, strSqlExec.GetLength() * 3 + 1);
				WCharToUTF8(strSqlExec, cSqlExec, strSqlExec.GetLength() * 3 + 1);

				if (SQLITE_OK != sqlite3_exec(m_pdb, cSqlExec, 0, 0, &errmsg))
				{
					fprintf(stderr, "SQL error: %s\n", errmsg);
					TRACE(errmsg);
					sqlite3_free(errmsg);
				}
				delete[] cSqlExec;
			}

		}
	}
}

VECT_PROGRAMITEM CDatabaseControl::GetProgramItem(int nProgramID)
{
	VECT_PROGRAMITEM Vect_ProgramItem;
	Vect_ProgramItem.clear();
	if (TableIsExist(TABLENAME_PROGRAMITEM_TABLE))
	{
		//TRACE("此表存在\n");
		// 查询数据表
		CString strtemp = _T("");
		strtemp.Format(L"select * from %s where 程序ID = %d order by 步号 Asc", TABLENAME_PROGRAMITEM_TABLE, nProgramID);

		char* sSQL3 = new char[strtemp.GetLength() * 2 + 1];
		memset(sSQL3, 0, strtemp.GetLength() * 2 + 1);
		WCharToUTF8(strtemp, sSQL3, strtemp.GetLength() * 2);
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(m_pdb, sSQL3, -1, &stmt, NULL) == SQLITE_OK)
		{

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				//获得数据l
				STRUCT_PROGRAMITEM structtemp;
				TCHAR pWdata[1001];
				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				structtemp.nStep = sqlite3_column_int(stmt, 2);
				structtemp.nHightVotage = sqlite3_column_double(stmt, 3);
				structtemp.nKeepTime = sqlite3_column_int(stmt, 4);
				char* pData = (char*)sqlite3_column_text(stmt, 5);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strMemo.Format(_T("%s"), pWdata);





				Vect_ProgramItem.push_back(structtemp);
			}
			sqlite3_finalize(stmt);
		}
		if (sSQL3 != NULL)
		{
			delete[] sSQL3;
		}
	}
	return Vect_ProgramItem;
}
void  CDatabaseControl::LoadProductInfo(void * pInputData, void * pOutputData, void * pReserved)
{
	if (TableIsExist(TABLENAME_PRODUCTINFO_TABLE))
	{
		//TRACE("此表存在\n");
		VECT_PROGRAM*pVectDetect = (VECT_PROGRAM*)pOutputData;
		// 查询数据表
		CString strtemp = _T("select * from ");
		strtemp += TABLENAME_PRODUCTINFO_TABLE;
		char * sSQL3 = new char[strtemp.GetLength() * 2 + 1];
		memset(sSQL3, 0, strtemp.GetLength() * 2 + 1);
		WCharToUTF8(strtemp, sSQL3, strtemp.GetLength() * 2);
		sqlite3_stmt *stmt;
		if (sqlite3_prepare_v2(m_pdb, sSQL3, -1, &stmt, NULL) == SQLITE_OK)
		{
			while (sqlite3_step(stmt) == SQLITE_ROW) 
			{
				//获得数据l
				STRUCT_PROGRAM structtemp;
				TCHAR pWdata[1001];
				memset(pWdata, 0, sizeof(TCHAR)* 1001);
				structtemp.nProgramID = sqlite3_column_int(stmt, 1);

				char* pData = (char*)sqlite3_column_text(stmt, 2);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strProgramName.Format(_T("%s"), pWdata);

				structtemp.fHightVoltage_Limit = sqlite3_column_double(stmt, 3);
				structtemp.nLowValtageLimit = sqlite3_column_int(stmt, 4);
				structtemp.nLowVoltage = sqlite3_column_int(stmt, 5);
				structtemp.nLowCurrent = sqlite3_column_int(stmt, 6);
				pData = (char *)sqlite3_column_text(stmt, 7);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strMemo.Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 8);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strDataSavePath.Format(_T("%s"), pWdata);

				structtemp.VectProgItem = GetProgramItem(structtemp.nProgramID);
				pVectDetect->push_back(structtemp);
			}
			sqlite3_finalize(stmt);
		}
		if (sSQL3 != NULL)
		{
			delete[] sSQL3;
		}
	}
}

void CDatabaseControl::SaveFireRecord(void* pData, void* pReserved)
{
	int rc;
	char* errmsg = NULL;
	if (TableIsExist(TABLENAME_FIRERECORD_TABLE))
	{
		STRUCT_FIRERECORD* pfireRecord = (STRUCT_FIRERECORD*)pData;
		CString strSqlExec = _T("");
		strSqlExec.Format(_T("insert into %s values(NULL,'%s','%s','%s','%d','%s','%s','%s','%s','%s','%s','%s','%s');"),
			TABLENAME_FIRERECORD_TABLE,
			pfireRecord->strTubeType,
			pfireRecord->strTubeSn,
			pfireRecord->strTime,
			pfireRecord->nStep,
			pfireRecord->strVoltage,
			pfireRecord->strVoltageFire,
			pfireRecord->strFileMoment,
			pfireRecord->strFireCurrentCH[0],
			pfireRecord->strFireCurrentCH[1],
			pfireRecord->strFireCurrentCH[2],
			pfireRecord->strFireCurrentCH[3],
			pfireRecord->strReserved);
		char* cSqlExec = new char[strSqlExec.GetLength() * 3 + 1];
		if (cSqlExec == NULL)
		{
			return;
		}
		memset(cSqlExec, 0, strSqlExec.GetLength() * 3 + 1);
		WCharToUTF8(strSqlExec, cSqlExec, strSqlExec.GetLength() * 3 + 1);

		if (SQLITE_OK != sqlite3_exec(m_pdb, cSqlExec, 0, 0, &errmsg))
		{
			fprintf(stderr, "SQL error: %s\n", errmsg);
			TRACE(errmsg);
			sqlite3_free(errmsg);
		}
		delete[] cSqlExec;

	}
	
}
void CDatabaseControl::LoadFireRecord(void* pInputData, void* pOutputData, void* pReserved)
{
	if (TableIsExist(TABLENAME_FIRERECORD_TABLE))
	{
		//TRACE("此表存在\n");
		VECT_FIRERECORD* pVectDetect = (VECT_FIRERECORD*)pOutputData;
		// 查询数据表
		CString strtemp = _T("");
		if (pInputData == NULL)
		{
			strtemp.Format(L"select * from %s order by id desc limit 100", TABLENAME_FIRERECORD_TABLE);
		}
		else
		{
			strtemp = *((CString*)pInputData);
		}
		char* sSQL3 = new char[strtemp.GetLength() * 3 + 1];
		memset(sSQL3, 0, strtemp.GetLength() * 3 + 1);
		WCharToUTF8(strtemp, sSQL3, strtemp.GetLength() * 3);
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(m_pdb, sSQL3, -1, &stmt, NULL) == SQLITE_OK)
		{
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				//获得数据
				STRUCT_FIRERECORD structtemp;
				TCHAR pWdata[1001];
				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				char* pData = (char*)sqlite3_column_text(stmt, 1);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strTubeType.Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 2);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strTubeSn.Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 3);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strTime.Format(_T("%s"), pWdata);

				structtemp.nStep = sqlite3_column_int(stmt, 4);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 5);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strVoltage.Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 6);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strVoltageFire.Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 7);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strFileMoment.Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 8);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strFireCurrentCH[0].Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 9);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strFireCurrentCH[1].Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 10);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strFireCurrentCH[2].Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 11);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strFireCurrentCH[3].Format(_T("%s"), pWdata);

				memset(pWdata, 0, sizeof(TCHAR) * 1001);
				pData = (char*)sqlite3_column_text(stmt, 12);
				UTF8ToWChar(pData, pWdata, 1000);
				structtemp.strReserved.Format(_T("%s"), pWdata);

				pVectDetect->push_back(structtemp);
			}
			sqlite3_finalize(stmt);
		}
		if (sSQL3 != NULL)
		{
			delete[] sSQL3;
		}
	}
}

void CDatabaseControl::SaveOpenCloseFlowInfo(void* pData, void* pReserved)
{
	//int rc;
	//char* errmsg = NULL;
	//if (TableIsExist(TABLENAME_OPENCLOSEFLOW_TABLE))
	//{
	//	//TRACE("此表存在\n");
	//	CString strTemp = _T("delete from ");
	//	strTemp += TABLENAME_OPENCLOSEFLOW_TABLE;
	//	char* sSQL_del = new char[strTemp.GetLength() * 2 + 1];
	//	memset(sSQL_del, 0, strTemp.GetLength() * 2 + 1);
	//	WCharToUTF8(strTemp, sSQL_del, strTemp.GetLength() * 2 + 1);
	//	rc = sqlite3_exec(m_pdb, sSQL_del, 0, 0, &errmsg);
	//	if (sSQL_del != NULL)
	//	{
	//		delete[] sSQL_del;
	//		sSQL_del = NULL;
	//	}
	//	if (rc != SQLITE_OK)
	//	{
	//		fprintf(stderr, "SQL error: %s\n", errmsg);
	//		TRACE(errmsg);
	//		sqlite3_free(errmsg);
	//		return;
	//	}

	//	VECT_OPENCLOSETUBE* pVectDetect = (VECT_OPENCLOSETUBE*)pData;
	//	for (int i = 0; i < pVectDetect->size(); i++)
	//	{
	//		CString strSqlExec = _T("");
	//		strSqlExec.Format(_T("insert into %s values(NULL,'%s',%d,%d,%d,%d,%d);"),
	//			TABLENAME_OPENCLOSEFLOW_TABLE,
	//			pVectDetect->at(i).strProductName,
	//			pVectDetect->at(i).nVoltage,
	//			pVectDetect->at(i).nCurrent,
	//			pVectDetect->at(i).nKeepOpenMinutes,
	//			pVectDetect->at(i).nKeepCloseMinutes,
	//			pVectDetect->at(i).nLoopTimes);
	//		char* cSqlExec = new char[strSqlExec.GetLength() * 2 + 1];
	//		if (cSqlExec == NULL)
	//		{
	//			return;
	//		}
	//		memset(cSqlExec, 0, strSqlExec.GetLength() * 2 + 1);
	//		WCharToUTF8(strSqlExec, cSqlExec, strSqlExec.GetLength() * 2 + 1);

	//		if (SQLITE_OK != sqlite3_exec(m_pdb, cSqlExec, 0, 0, &errmsg))
	//		{
	//			fprintf(stderr, "SQL error: %s\n", errmsg);
	//			TRACE(errmsg);
	//			sqlite3_free(errmsg);
	//		}
	//		delete[] cSqlExec;

	//	}
	//}
}
void CDatabaseControl::LoadOpenCloseFlowInfo(void* pInputData, void* pOutputData, void* pReserved)
{
	//if (TableIsExist(TABLENAME_OPENCLOSEFLOW_TABLE))
	//{
	//	//TRACE("此表存在\n");
	//	VECT_OPENCLOSETUBE* pVectDetect = (VECT_OPENCLOSETUBE*)pOutputData;
	//	// 查询数据表
	//	CString strtemp = _T("");
	//	strtemp.Format(L"select * from %s order by 产品名 Asc", TABLENAME_OPENCLOSEFLOW_TABLE);
	//	char* sSQL3 = new char[strtemp.GetLength() * 2 + 1];
	//	memset(sSQL3, 0, strtemp.GetLength() * 2 + 1);
	//	WCharToUTF8(strtemp, sSQL3, strtemp.GetLength() * 2);
	//	sqlite3_stmt* stmt;
	//	if (sqlite3_prepare_v2(m_pdb, sSQL3, -1, &stmt, NULL) == SQLITE_OK)
	//	{
	//		while (sqlite3_step(stmt) == SQLITE_ROW)
	//		{
	//			//获得数据
	//			STRUCT_OPENCLOSETUBE structtemp;
	//			TCHAR pWdata[1001];
	//			memset(pWdata, 0, sizeof(TCHAR) * 1001);
	//			char* pData = (char*)sqlite3_column_text(stmt, 1);
	//			UTF8ToWChar(pData, pWdata, 1000);
	//			structtemp.strProductName.Format(_T("%s"), pWdata);
	//			structtemp.nVoltage = sqlite3_column_int(stmt, 2);
	//			structtemp.nCurrent = sqlite3_column_int(stmt, 3);
	//			structtemp.nKeepOpenMinutes= sqlite3_column_int(stmt, 4);
	//			structtemp.nKeepCloseMinutes = sqlite3_column_int(stmt, 5);
	//			structtemp.nLoopTimes = sqlite3_column_int(stmt, 6);

	//			pVectDetect->push_back(structtemp);
	//		}
	//		sqlite3_finalize(stmt);
	//	}
	//	if (sSQL3 != NULL)
	//	{
	//		delete[] sSQL3;
	//	}
	//}
}