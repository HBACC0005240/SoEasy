#include "GameData.h"
#include "YunLai.h"
#include "../include/ITPublic.h"
#include "MLAssist.h"
#include <QMessageBox>
#include "GlobalDefine.h"
#include "../include/XASM.h"
//#include "../include/XEDParse.h"
//#include "./asmjit/asmjit.h"
//using namespace asmjit;
//using namespace asmjit::x86;
//#ifdef _WIN64
//#pragma comment (lib,"../lib/XEDParse/x64/XEDParse_x64.lib")
//#else
//#pragma comment (lib,"../lib/XEDParse/x32/XEDParse_x86.lib")
//#endif // _WIN64
GameData& GameData::getInstance()
{
	static GameData g_pGameData;
	return g_pGameData;
}

void GameData::initData()
{
	m_characterForVal.clear();
	m_characterForVal.insert("��",CHARACTER_Troop);
	m_characterForVal.insert("ս", CHARACTER_Battle);
	m_characterForVal.insert("��", CHARACTER_Chat);
	m_characterForVal.insert("��", CHARACTER_Card);
	m_characterForVal.insert("��", CHARACTER_Exchange);
	m_characterForVal.insert("��", CHARACTER_Home);
	m_szPickSkillList.clear();
	m_indexForSkillName.clear();
	m_szPickSkillList << "����" << "�ڿ�" << "��ľ" << "����" << "��������" << "�������" << "�ڿ�����" << "��ľ����" << "��������";
	m_skillCodeForMyCode.insert(225, 104);
	m_skillCodeForMyCode.insert(226, 103);
	m_skillCodeForMyCode.insert(227, 102);
	//��ȡ������Ϣ
	for (int i = 0;i < 15;++i)//��10��  15��������
	{
		DWORD pAddress = 0x00D84FEC;
		DWORD offset = i * 0x49FC;
		pAddress += offset;
		QString skillName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
		DWORD pSkillCodeAddress =pAddress+ 0x30;//����id
		int nSkillCode = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pSkillCodeAddress);
		DWORD pShowOrderAddress =pAddress+ 0x38;//��ʾ˳��
		int nShowIndex = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pShowOrderAddress);
		/*if (m_szPickSkillList.contains(skillName))
		{
			m_skillCodeForIndex.insert();
		}*/
		if (m_skillCodeForMyCode.contains(nSkillCode))
			m_skillCodeForIndex.insert(m_skillCodeForMyCode.value(nSkillCode), i);
		m_indexForSkillName.insert(i, skillName);
	}	
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
	if (CallBase == NULL)//����ڴ�����ʧ��
	{
		//�رս��̾��
		CloseHandle(hTarget);
		return;
	}
	bool bRet = WriteProcessMemory(hTarget, CallBase, (LPCVOID)RemoteMoveCall, 0x1000, NULL);
	if (bRet == false)
	{
		//�ͷ��ڴ�ռ�
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	//��̬װ��ָ����DLL�ļ�
	HANDLE hthread = CreateRemoteThread(hTarget, NULL, 0, LPTHREAD_START_ROUTINE(CallBase), NULL, 0, NULL);
	if (hthread == NULL)//��������DLL���߳�ʧ��
	{
		//�ͷ��ڴ�ռ�
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	WaitForSingleObject(hthread, INFINITE);
	//�ͷ���Դ
	VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
	CloseHandle(hthread);
	CloseHandle(hTarget);
}

void GameData::turn_about(int nDirection)
{
	//����call��ʽ
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

	//����������ص� ģ�ⰴ����ʽ
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
	//	//YunLai::SetFocusToWnd(m_gameHwnd);//�Ѿ��ǽ����� ����Ҫ
	//LPARAM newl = MAKELPARAM(points.x, points.y);
	//PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	//Sleep(GetDoubleClickTime());
	//PostMessage(m_gameHwnd, WM_RBUTTONDOWN, WM_RBUTTONDOWN, newl);
	//PostMessage(m_gameHwnd, WM_RBUTTONUP, WM_RBUTTONUP, newl);
	//return;
	//::ClientToScreen(m_gameHwnd, &points);
	//::SetCursorPos(points.x, points.y);
	////YunLai::SetFocusToWnd(m_gameHwnd);//�Ѿ��ǽ����� ����Ҫ
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
	int nTime = 100;//100��
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
//void RemoteWorkCall()
//{
//	//	DWORD pAddress = 0x00462EF8;
//	_asm
//	{
//		mov eax, 0
//		mov ecx, 0
//		mov edx, 0x00462EF8
//		call edx
//	}
//}

__declspec(naked) void  RemoteWorkCall()
{
//	DWORD pAddress = 0x00462EF8;
	_asm
	{
		mov eax, 0
		mov ecx, 0
		mov edx, 0x00462EF8
		call edx
		ret
	}
}


 //�������̷���Ȩ��   
 bool EnableDebugPriv()
 {
	 HANDLE hToken;
	 LUID sedebugnameValue;
	 TOKEN_PRIVILEGES tkp;

	 if (!OpenProcessToken(GetCurrentProcess(),
		 TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		 return false;
	 }
	 if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	 {
		 CloseHandle(hToken);
		 return false;
	 }
	 tkp.PrivilegeCount = 1;
	 tkp.Privileges[0].Luid = sedebugnameValue;
	 tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	 if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	 {
		 CloseHandle(hToken);
		 return false;
	 }
	 return true;
 }
void GameData::Work(int code, const QString itemName/*=""*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD byWrite;
	//��Ϸ���̾��
	EnableDebugPriv();//��������Ȩ�� VS2008�Ժ�İ汾����Ҫ
	HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, false, m_gameProcessID);
	//��Ŀ����̷����ڴ�ռ� �Է���д��Ҫִ�еĴ���
	PVOID FarCall = VirtualAllocEx(hp, NULL, 0x8FFF, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//��Ŀ����̵� Ŀ���ַд������Ҫִ�еĴ��� 
	WriteProcessMemory(hp, FarCall, RemoteWorkCall, 0x8FFF, &byWrite);
	//��Ŀ����� ָ����ַ ִ�д���
	//TRACE("\n addr=%x \n", FarCall);
	CreateRemoteThread(hp, NULL, NULL, (LPTHREAD_START_ROUTINE)FarCall, NULL, NULL, NULL);

	return;
	//value1 = 101 ����
	//value1 = 201 �������
	//value1 = 102 �ھ�
	//value1 = 202 �����ھ�
	//value1 = 103 ����
	//value1 = 203 ��������
	//value1 = 104 ��ľ
	//value1 = 204 ����ľ
	//value1 = 105 ��������
	//value1 = 205 ������������
	//value1 = 106 �������
	//value1 = 206 �����������
	int nIndex = m_skillCodeForIndex.value(code);
	EnableDebugPriv();//��������Ȩ�� VS2008�Ժ�İ汾����Ҫ
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);

	DWORD nThreadSize = 0x8FFF;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{
		QMessageBox::information(nullptr, "��ʾ��", "����ռ�ʧ��", QMessageBox::Ok);
		return;
	}
	BYTE buf[40] = { 0 };
	QStringList szAssemble;
	szAssemble << QString("mov eax, %1").arg(nIndex)
		<< "mov ecx, 0"
		<< "mov edx, 0x00462EF8"
		<< "call edx";
	/*bool bTAssable=transAssemble(callBase,buf, nThreadSize, szAssemble);
	if (bTAssable == false)
		return;*/
	//DWORD  nWirteSize = ((DWORD)RemoteWorkCallEnd - (DWORD)RemoteWorkCall);
	//WriteProcessMemory(hp, FarCall, plant1, 0x8FFF, &byWrite);
	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)RemoteWorkCall, nThreadSize, 0);
//	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)buf, nThreadSize, nullptr);
//	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)RemoteWorkCall, 4096, nullptr);
	if (bRet == false)
	{
		QMessageBox::information(nullptr, "��ʾ��", "д�����ʧ��", QMessageBox::Ok);
		return;
	}
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, NULL, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		QMessageBox::information(nullptr, "��ʾ��", "Զ�̵��ô���ʧ��", QMessageBox::Ok);
		return;
	}
	/*CloseHandle(hRemoteThread);
	CloseHandle(hProcess);*/
}

void GameData::Work(const QString& skillName, const QString itemName /*= ""*/)
{
	int nIndex = m_indexForSkillName.key(skillName);
	int mySkillCode = m_skillCodeForIndex.key(nIndex);
	Work(mySkillCode, itemName);
}

bool GameData::isWorking()
{
	return YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0xBD06FC");	
}

#define  BYTE_MAX 4096
//#define ASMJIT_BUILD_X86
//#define ASMJIT_STATIC
bool GameData::transAssemble(void* pAddr,BYTE* buf, int nLen, QStringList szCmdList)
{
	QString szMsg;
	//foreach (QString szCmd,szCmdList)
	//{
	//	XEDPARSE xed = { 0 };
	//	xed.cip;
	//}
	char error[256] = { 0 };
	int count = szCmdList.size();
	int j = 0;
	int m = 0;
	for (int i = 0;i < count;i++)
	{
		char cmd[256] = { 0 };
		QString szCmd = szCmdList.at(i);
		int len = szCmd.size();// m_edit.GetLine(i, cmd, 256);
		if (len == 0) continue;
		memcpy(cmd,szCmd.toStdString().c_str(),len);
		cmd[len] = '\0';
		t_asmmodel t_asm;
		j = m_asm.Assemble(cmd, (DWORD)pAddr + m, &t_asm, 0, 4, error);
		if (j <= 0)
		{
			szMsg.append(QString("error=\"%1\"").arg(error));
		}
		for (int k = 0; k < j; k++)
		{
			buf[m] = (BYTE)t_asm.code[k];
			szMsg.append(QString("%1").arg(buf[m]));
			m = m + 1;
			if (m > nLen)
				return false;//�����ڴ��ַ
		}
	}
	if ((m+2) > nLen)
		return false;//�����ڴ��ַ
	buf[m] = 0x0c2;//ret 4
	buf[m + 1] = 0x04;
	buf[m + 2] = 0x00;
	//JitRuntime runtime;
	//CodeHolder code;                        // Holds code and relocation information.
	//code.init(runtime.codeInfo());               // Initialize to the same arch as JIT runti
	//x86::Assembler a(&code);                // Create and attach x86::Assembler to `code`.
	//a.mov(x86::eax, 1);                     // Move one to 'eax' register.
	//a.ret();
	//X86::Compiler c(&runtime);
	return true;
}

void GameData::selectRenew()
{
	POINT points;
	points.x = g_defResumeHMP.x();
	points.y = g_defResumeHMP.y();
	//ClientToScreen(m_gameHwnd, &points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//�Ѿ��ǽ����� ����Ҫ
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(500);//GetDoubleClickTime());
	points.x = g_defYes.x();
	points.y = g_defYes.y();
	newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(500);//GetDoubleClickTime());
	points.x = g_defOk.x();
	points.y = g_defOk.y();
	newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);


}

void GameData::SetCharacterState(int state)
{	
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00E2A554", state);
}

void GameData::SetCharacterSwitch(int v1, int v2, int v3, int v4, int v5, int v6)
{
	int nVal = 0;
	if (v1)
		nVal += CHARACTER_Battle;
	if (v2)
		nVal += CHARACTER_Chat;
	if (v3)
		nVal += CHARACTER_Troop;	
	if (v4)
		nVal += CHARACTER_Card;
	if (v5)
		nVal += CHARACTER_Exchange;
	if (v6)
		nVal += CHARACTER_Home;
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00E2A554", nVal);

}

void GameData::readBattleInfo()
{
	memset(m_Infos, 0, sizeof(PERSONINFO) * 20);
	char* pText = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000);//0x005590E4
	if (strlen(pText) < 1)
		return;
	LPWSTR wText = ANSITOUNICODE1(pText);
	QString szBattleInfo = QString::fromStdWString(wText);// YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000));
	QStringList szBattleInfoList = szBattleInfo.split("|");//11��Ϊһ��ս����Ϣ
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
