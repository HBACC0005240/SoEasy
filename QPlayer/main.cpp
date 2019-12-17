#include "stdafx.h"
#include "QPlayer.h"
#include <QtWidgets/QApplication>
#include "DbgHelp.h"
#include <windows.h>
//
////ANSI转化成UNICODE
//LPWSTR ANSITOUNICODE(const char* pBuf)
//{
//	int lenA = lstrlenA(pBuf);
//	int lenW=0;
//	LPWSTR lpszFile;
//	lenW = MultiByteToWideChar(CP_ACP,0,pBuf,lenA,0,0);	
//	if(lenW > 0)
//	{
//		lpszFile = SysAllocStringLen(0,lenW);	//申请一个指定字符长度的 BSTR 指针，并初始化为一个字符串
//		MultiByteToWideChar(CP_ACP,0,pBuf,lenA,lpszFile,lenW);	//
//	}
//	return lpszFile;
//} 
typedef	BOOL (WINAPI * MINIDUMP_WRITE_DUMP)(
	IN HANDLE			hProcess,
	IN DWORD			ProcessId,
	IN HANDLE			hFile,
	IN MINIDUMP_TYPE	DumpType,
	IN CONST PMINIDUMP_EXCEPTION_INFORMATION	ExceptionParam, OPTIONAL
	IN PMINIDUMP_USER_STREAM_INFORMATION		UserStreamParam, OPTIONAL
	IN PMINIDUMP_CALLBACK_INFORMATION			CallbackParam OPTIONAL
	);
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件	
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;
	MINIDUMP_WRITE_DUMP	MiniDumpWriteDump_;
	HMODULE hDbgHelp = LoadLibrary(ANSITOUNICODE("DBGHELP.DLL"));
	MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	MiniDumpWriteDump_(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}
// 处理Unhandled Exception的回调函数
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{	
	QDir temp;
	QString errorPath;
	bool exist = temp.exists(QCoreApplication::applicationDirPath()+"/Log");
	if(!exist)		
	{
		bool ok = temp.mkdir(QCoreApplication::applicationDirPath()+"/Log");
		if(!ok)	//失败 放在执行路径下
			errorPath = QCoreApplication::applicationDirPath()+QString("/%1崩溃日志.dmp").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss"));
	}
	errorPath = QCoreApplication::applicationDirPath()+QString("/Log/%1崩溃日志.dmp").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss"));
	// 这里弹出一个错误对话框并退出程序
	CreateDumpFile(ANSITOUNICODE(errorPath.toStdString().c_str()), pException);  
	FatalAppExit(-1,ANSITOUNICODE("*** 未知 错误! ***"));
	return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc, char *argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);  	
	QApplication a(argc, argv);
	QApplication::addLibraryPath("./");
	a.setWindowIcon(QIcon(":/QPlayer/Resources/soeasy.ico"));
	//qRegisterMetaType<INT64>("INT64");


#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#else
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

	QPlayer w;
	w.show();
	return a.exec();
}
