#include "GameData.h"
#include "YunLai.h"
#include "../include/ITPublic.h"
#include "MLAssist.h"
GameData& GameData::getInstance()
{
	static GameData g_pGameData;
	return g_pGameData;
}

POINT GameData::GetGamePersonCoordinate()
{
	POINT pointPerson;
	pointPerson.x = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	pointPerson.y = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	return pointPerson;
}

QString GameData::GetGameMapName()
{
	QString szMapName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "009181C0", 100)));
	return szMapName;
}

int GameData::getPersionHP()
{
	int nHP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A418");
	return nHP;
}

int GameData::getPersionMP()
{
	int nMP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A420");
	return nMP;
}

int GameData::IsBattering()
{
	return YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "008E5444");
}

void GameData::moveToPoint(int x, int y)
{
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "008E543C", x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "008E5440", y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044D000), NULL, 0, &tId);
	return;
	LPVOID CallBase = VirtualAllocEx(hTarget, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (CallBase == NULL)//如果内存申请失败
	{
		//关闭进程句柄
		CloseHandle(hTarget);
		return;
	}
	bool bRet = WriteProcessMemory(hTarget, CallBase, (LPCVOID)RemoteMoveCall, 0x1000, NULL);
	if (bRet == false)
	{
		//释放内存空间
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	//动态装载指定的DLL文件
	HANDLE hthread = CreateRemoteThread(hTarget, NULL, 0, LPTHREAD_START_ROUTINE(CallBase), NULL, 0, NULL);
	if (hthread == NULL)//创建加载DLL的线程失败
	{
		//释放内存空间
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	WaitForSingleObject(hthread, INFINITE);
	//释放资源
	VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
	CloseHandle(hthread);
	CloseHandle(hTarget);
}

void GameData::turn_about(int nDirection)
{
	//调用call方式
	POINT points = GetGamePersonCoordinate();
	switch (nDirection)
	{
	case MOVE_DIRECTION_UP:
	{
		points.x = points.x + 1;
		points.y = points.y - 1;
		break;
	}
	case MOVE_DIRECTION_DOWN:
	{
		points.x = points.x - 1;
		points.y = points.y + 1;
		break;
	}
	case MOVE_DIRECTION_LEFT:
	{
		points.x = points.x - 1;
		points.y = points.y - 1;
		break;
	}
	case MOVE_DIRECTION_RIGHT:
	{
		points.x = points.x + 1;
		points.y = points.y + 1;
	}
	case MOVE_DIRECTION_LEFTUP:	points.y = points.y - 1; break;
	case MOVE_DIRECTION_RIGHTUP:
	{
		points.x = points.x + 1;
		break;
	}
	case MOVE_DIRECTION_LEFTDOWN:
	{
		points.x = points.x - 1;
		break;
	}
	case MOVE_DIRECTION_RIGHTDOWN:
	{
		points.y = points.y + 1;
		break;
	}default:return;
	}
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B58", points.x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B5C", points.y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044CCC0), NULL, 0, &tId);
	return;

	//计算鼠标像素点 模拟按键方式
	//POINT points;
	//long resX = 320;
	//long resY = 240;// -MOVE_STEP_DIAGONAL;
	//switch (nDirection)
	//{
	//case MOVE_DIRECTION_UP:resY = 240 - MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_DOWN:resY = 240 + MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_LEFT:resX = 320 - MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_RIGHT:resX = 320 + MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_LEFTUP:
	//{
	//	resX = 320 - MOVE_STEP_DIAGONAL;
	//	resY = 240 - MOVE_STEP_DIAGONAL; break;
	//}
	//case MOVE_DIRECTION_RIGHTUP:
	//{
	//	resX = 320 + MOVE_STEP_DIAGONAL;
	//	resY = 240 - MOVE_STEP_DIAGONAL; break;
	//}
	//case MOVE_DIRECTION_LEFTDOWN:
	//{
	//	resX = 320 - MOVE_STEP_DIAGONAL;
	//	resY = 240 + MOVE_STEP_DIAGONAL; break;
	//}
	//case MOVE_DIRECTION_RIGHTDOWN:
	//{
	//	resX = 320 + MOVE_STEP_DIAGONAL;
	//	resY = 240 + MOVE_STEP_DIAGONAL; break;
	//}default:return;
	//}
	//points.x = resX;
	//points.y = resY;
	////ClientToScreen(m_gameHwnd, &points);
	//	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	//LPARAM newl = MAKELPARAM(points.x, points.y);
	//PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	//Sleep(GetDoubleClickTime());
	//PostMessage(m_gameHwnd, WM_RBUTTONDOWN, WM_RBUTTONDOWN, newl);
	//PostMessage(m_gameHwnd, WM_RBUTTONUP, WM_RBUTTONUP, newl);
	//return;
	//::ClientToScreen(m_gameHwnd, &points);
	//::SetCursorPos(points.x, points.y);
	////YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	//YunLai::SimMouseClick(2, 1);
}

void GameData::RemoteMoveCall()
{
	_asm
	{
		/*push 0x1
		push 0x3
		mov ecx, 0x00ce5758*/
		mov eax, 0x0044D000
		call eax
	}
}


bool GameData::WaitToMove(int x, int y)
{
	int nTime = 100;//100秒
	moveToPoint(x, y);
	for (int i = 0; i < nTime; ++i)
	{
		POINT curPoint = GetGamePersonCoordinate();
		if (curPoint.x == x && curPoint.y == y)
			return true;
		Sleep(1000);
	}
	return false;
}

void GameData::JianDongXi(int nDirection)
{
	POINT points = GetGamePersonCoordinate();	
	switch (nDirection)
	{
	case MOVE_DIRECTION_UP:
	{
		points.x = points.x + 1;
		points.y = points.y - 1;
		break;
	}
	case MOVE_DIRECTION_DOWN: 
	{
		points.x = points.x - 1;
		points.y = points.y + 1;
		break;
	}
	case MOVE_DIRECTION_LEFT:
	{
		points.x = points.x - 1;
		points.y = points.y - 1;
		break;
	}
	case MOVE_DIRECTION_RIGHT: 
	{
		points.x = points.x + 1;
		points.y = points.y + 1;
	}
	case MOVE_DIRECTION_LEFTUP:	points.y = points.y - 1;break;
	case MOVE_DIRECTION_RIGHTUP:
	{
		points.x = points.x + 1;
		break;
	}
	case MOVE_DIRECTION_LEFTDOWN:
	{
		points.x = points.x - 1;
		break;
	}
	case MOVE_DIRECTION_RIGHTDOWN:
	{
		points.y = points.y + 1;
		break;
	}default:return;
	}	
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B58", points.x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B5C", points.y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044CCC0), NULL, 0, &tId);
	return;
}

void GameData::readBattleInfo()
{
	memset(m_Infos, 0, sizeof(PERSONINFO) * 20);
	char* pText = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000);//0x005590E4
	if (strlen(pText) < 1)
		return;
	LPWSTR wText = ANSITOUNICODE1(pText);
	QString szBattleInfo = QString::fromStdWString(wText);// YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000));
	QStringList szBattleInfoList = szBattleInfo.split("|");//11个为一组战斗信息
	int nPersonSize = szBattleInfoList.size() / 11;
	for (int i = 0; i < nPersonSize; ++i)
	{
		QStringList szTemps = szBattleInfoList.mid(i * 11, 11);
		//	qDebug() << szTemps;
		int nPos = szTemps.at(2).toInt(nullptr, 16);
		QString szName = szTemps.at(3);
		lstrcpyW(m_Infos[nPos].name, szName.toStdWString().c_str());
		m_Infos[nPos].level = szTemps.at(5).toInt(nullptr, 16);
		m_Infos[nPos].hp = szTemps.at(6).toInt(nullptr, 16);
		m_Infos[nPos].totalhp = szTemps.at(7).toInt(nullptr, 16);
		m_Infos[nPos].mp = szTemps.at(8).toInt(nullptr, 16);
		m_Infos[nPos].totalmp = szTemps.at(9).toInt(nullptr, 16);
	}
}

void GameData::getPersonInfo(PERSONINFO* pPerson)
{
	if (pPerson == nullptr)return;
	memcpy(pPerson,m_Infos,sizeof(m_Infos));
}
