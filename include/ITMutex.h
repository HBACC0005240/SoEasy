#pragma once
//************************************
//��ƽ̨�߳�����
//ʱ�䣺2016/1/18 WZQ
//************************************
//************************************
#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800Ϊ2013  2013����֧��c++11��׼
	#include <mutex>
	#define LOCK_MUTEX std::mutex
#else 
	#ifdef _WIN32
		#include <Windows.h>
		#define LOCK_MUTEX CRITICAL_SECTION		///�߳��� �ķ�ʱ��� �����ܿ����
	#else
		#include <pthread.h>
		#define LOCK_MUTEX pthread_mutex_t			///�߳��� ʱ�䳤
	#endif
#endif


class ITMutex
{
public:
	ITMutex(void)
	{
		Init();
	}
	~ITMutex()
	{
		Close();
	}
	void Init();
	void Lock();
	void UnLock();	
	void Close();
private:
	LOCK_MUTEX m_lock;	
};

//����һ���Զ���������
class ITAutoLock{
public:	
	ITAutoLock(ITMutex& pThreadLock)
	{
		m_pThreadLock=&pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}	
	ITAutoLock(ITMutex *pThreadLock)
	{
		m_pThreadLock=pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}
	~ITAutoLock()
	{
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->UnLock();
		}
	}
private:
	ITMutex* m_pThreadLock;	
};
