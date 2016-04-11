#pragma once
/*
* MemBuffer �� ����buf������  
* AutoLock  �Զ���
* ThreadLock �߳���
* ʱ�䣺2015/7/27 WZQ
*/

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
#include "assert.h"
#include<iostream>
using namespace std;

class ThreadLock;
class AutoLock;
template<typename T>
class MemBuffer
{
public:	
	typedef size_t   size_type;	
	MemBuffer(size_t nSize) : m_pMemBuffer(new T[nSize+1]),m_size(nSize),m_count(0) 
	{		

	}

	~MemBuffer() 
	{ 
		delete []m_pMemBuffer; 
		m_pMemBuffer = NULL;
	}
	

	MemBuffer& operator=(const MemBuffer<T>& other) 
	{
		if (this != &other) 
		{
			AutoLock	guard(m_mutex);
			MemBuffer<T>	temp(other);        // invoke copy constructor
			_Swap(temp);                      // this->_Swap();
		}
		return (*this);
	}

	bool is_full() const 
	{
		AutoLock guard(m_mutex);
		return (m_count == m_size);
	}

	bool is_empty() const 
	{
		AutoLock guard(m_mutex);
		return (m_count == 0);
	}
	size_type getRemainSize() const
	{
		return (m_size - m_count);
	}
	size_type size()// const 
	{
		AutoLock guard(m_mutex);
		return m_count;
	}

	size_type capacity() const 
	{ 
		return m_size; 
	}
	size_type getData(T* data,size_type datasize)
	{
		AutoLock guard(m_mutex);
		if(data == NULL)
			return -1;
		if(datasize < m_count)
		{
			///�����ڴ��С
			return 0;
		}
		::memmove(data,m_pMemBuffer,m_count);
		return m_count;
	}
	////���������ָ�룬���ݳ���  ����ʵ������Ĵ�С
	size_type push_back(const T *data, size_type length) 
	{
		AutoLock guard(m_mutex);
		if(data == NULL)
			return 0;
		//assert(data != NULL);
		if (length == 0 || length > (m_size - m_count))
		{
			cout<<"\nlength > (m_size - m_count)\n";
			return 0;
		}
		
		size_type rearLen = m_size - m_count;    // β��ʣ��ռ�
		if (length <= rearLen) 
		{
			::memmove(&m_pMemBuffer[m_count], data, length);			
		}else
		{
			length = rearLen;		///ʵ��push����
			::memmove(&m_pMemBuffer[m_count], data, rearLen);			
		}
		m_count += length;
		return (length);
	}
	size_type push_front(const T *data, size_type length) 
	{

		AutoLock guard(m_mutex);
		if(data == NULL)
			return 0;
		//assert(data != NULL);
		if (length == 0 || length > (m_size - m_count))
		{
	//		cout<<"\nlength > (m_size - m_count)\n";
			return 0;
		}

		size_type rearLen = m_size - m_count;    // β��ʣ��ռ�
		if (length <= rearLen) 
		{
			///��ԭ������������ �ٰ���������ǰ��
			::memmove(&m_pMemBuffer[0],&m_pMemBuffer[rearLen],m_count);
			::memmove(&m_pMemBuffer[0], data, length);			
		}else
		{
			length = rearLen;		///ʵ��push����
			::memmove(&m_pMemBuffer[0],&m_pMemBuffer[rearLen],m_count);
			::memmove(&m_pMemBuffer[0], data, rearLen);			
		}
		m_count += length;
		return (length);
	}
	size_type pop_back(T *buf, size_type length)
	{
		AutoLock guard(m_mutex);
		//assert(buf != NULL);
		if(buf == NULL)
			return 0;
		if (length == 0)// || length > m_count) 
		{
			//		OutputDebugString("\n�����ݿ�ȡ!\n");
			return 0;
		}
		//���ȡ�ĳ��� > ȫ�����ݳ��ȣ���ȫȡ
		if (length > m_count)
		{
			length = m_count;
			::memmove(buf, &m_pMemBuffer[0], length);		
		}else
		{
			size_type rearLen = m_count - length;    // ǰ��ʣ������		
			::memmove(buf, &m_pMemBuffer[rearLen], length);			
		}		
		m_count -= length;				
		return (length);
	}
	size_type pop_front(T *buf, size_type length)
	{
		AutoLock guard(m_mutex);
//		assert(buf != NULL);
		if(buf == NULL)
			return 0;
		if (length == 0)// || length > m_count) 
		{
			//		OutputDebugString("\n�����ݿ�ȡ!\n");
			return 0;
		}
		//���ȡ�ĳ��� > ȫ�����ݳ��ȣ���ȫȡ
		if (length > m_count)
		{
			length = m_count;
			::memmove(buf, &m_pMemBuffer[0], length);		
		}else
		{
			::memmove(buf, &m_pMemBuffer[0], length);	
			size_type rearLen = m_count - length;    // ʣ������		
			::memmove(m_pMemBuffer, &m_pMemBuffer[length], rearLen);	//ʣ�������Ƶ�ǰ��
		}	
		m_count -= length;				
		return (length);
	}
	size_type pop_front(size_type length)
	{
		AutoLock guard(m_mutex);
		//		assert(buf != NULL);		
		if (length == 0)// || length > m_count) 
		{			
			return 0;
		}
		//���ȡ�ĳ��� > ȫ�����ݳ��ȣ���ȫȡ
		if (length > m_count)
		{
			length = m_count;			
		}else
		{			
			size_type rearLen = m_count - length;    // ʣ������		
			::memmove(m_pMemBuffer, &m_pMemBuffer[length], rearLen);	//ʣ�������Ƶ�ǰ��
		}	
		m_count -= length;				
		return (length);
	}
	size_type remove_front(size_type length)
	{
		AutoLock guard(m_mutex);
		//		assert(buf != NULL);		
		if (length == 0)// || length > m_count) 
		{			
			return 0;
		}
		//���ȡ�ĳ��� > ȫ�����ݳ��ȣ���ȫȡ
		if (length > m_count)
		{
			length = m_count;			
		}else
		{			
			size_type rearLen = m_count - length;    // ʣ������		
			::memmove(m_pMemBuffer, &m_pMemBuffer[length], rearLen);	//ʣ�������Ƶ�ǰ��
		}	
		m_count -= length;				
		return (length);
	}
	void clear()
	{
		AutoLock guard(m_mutex);
		m_count = 0;
		memset(m_pMemBuffer,0,m_size);
	}
private:
	void _Swap(MemBuffer<T>& other)
	{
		std::swap(m_pMemBuffer, other.m_pMemBuffer);
		std::swap(m_size, other.m_size);	
		std::swap(m_count, other.m_count);
	}
	
private:
	T*					m_pMemBuffer;		// buffer	
	size_type			m_size;				//buffer�ܴ�С	
	size_type           m_count;            //��Ч�ֽ���	
	mutable  ThreadLock			m_mutex;			//�߳���
};



class ThreadLock
{
public:
	ThreadLock(void)
	{
		Init();
	}
	~ThreadLock()
	{
		Close();
	}
	void Lock()
	{
		#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800Ϊ2013  2013����֧��c++11��׼
				m_lock.lock();
		#else
			#ifdef _WIN32
					EnterCriticalSection(&m_lock);
			#else
					pthread_mutex_lock(&m_lock);
			#endif
		#endif
	}
	void UnLock()
	{
		#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800Ϊ2013  2013����֧��c++11��׼
				m_lock.unlock();
		#else
			#ifdef _WIN32
					LeaveCriticalSection(&m_lock);
			#else
					pthread_mutex_unlock(&m_lock);
			#endif
		#endif
	}
	//protected:

	void Init()
	{
		#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800Ϊ2013  2013����֧��c++11��׼
		#else
			#ifdef _WIN32
					InitializeCriticalSection(&m_lock);
			#else
					pthread_mutex_init(&m_lock,NULL);
			#endif
		#endif
	}
	void Close()
	{
	#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800Ϊ2013  2013����֧��c++11��׼
	#else
		#ifdef _WIN32
				DeleteCriticalSection(&m_lock);
		#else
				pthread_mutex_destroy(&m_lock);
		#endif
	#endif
	}

private:
	LOCK_MUTEX m_lock;	
};

//����һ���Զ���������
class AutoLock{
public:	
	AutoLock(ThreadLock& pThreadLock)
	{
		m_pThreadLock=&pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}	
	AutoLock(ThreadLock *pThreadLock)
	{
		m_pThreadLock=pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}
	~AutoLock()
	{
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->UnLock();
		}
	}
private:
	ThreadLock* m_pThreadLock;	
};
