#pragma once
#include <string>

#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800Ϊ2013  2013����֧��c++11��׼
	#include <mutex>
	#include <thread>
	#define LOCK_MUTEX std::mutex
#else 
	#ifdef _WIN32
		#include <Windows.h>
		#include <process.h>
		#define LOCK_MUTEX CRITICAL_SECTION		///�߳��� �ķ�ʱ��� �����ܿ����
	#else
		#include <pthread.h>
		#define LOCK_MUTEX pthread_mutex_t			///�߳��� ʱ�䳤
	#endif
#endif

///Ŀǰֻ��windows�汾��linux�汾��ûд
typedef void (*UsrThreadFun)(void* pThis);
class ITThread
{
public:
	ITThread(void);	
	ITThread(const char * ThreadName);  
	ITThread(std::string ThreadName);  
	~ITThread(void);
	bool Start(bool bSuspend = false);  	
	bool Start(UsrThreadFun usrFun,void* pUsrData=NULL,bool bSuspend = false);  	

	void Join(int timeout = -1);  
	void Resume();  
	void Suspend();  
	bool Terminate(unsigned long ExitCode);  

	unsigned int GetThreadID();  
	std::string GetThreadName();  
	void SetThreadName(std::string ThreadName);  
	void SetThreadName(const char * ThreadName);  	
	void SetThreadFun(UsrThreadFun tFun,void* pThis);
	static int ThreadFunc(void* pParam);// Adapter
protected:
	int ThreadFuncKernal();// Kernal
private:  	
	HANDLE			m_handle;
	UsrThreadFun		m_threadfun;
	void*		    m_pThis;	
	unsigned int	m_nThreadID;  
	std::string		m_strThreadName;  
	volatile bool	m_bRun;				//�߳�״̬
	bool			m_bRunThreadCond;	//��������
};



