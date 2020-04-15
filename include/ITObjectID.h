#pragma once

#include <string>
#include <algorithm>
//#include <xfunctional>
#define I_INVALID_HANDLE 0
typedef unsigned __int64 UINT64, *PUINT64;
typedef unsigned int UINT, *PUINT;
typedef unsigned __int64 GOBJECTID;

class ITObjectID
{
public:
	//��ʼ����ѯ�����κ�NewID()֮ǰ����
	static void InitSeed();

	//�����µ�OBjectID
	static unsigned __int64 NewID();

	static unsigned __int64 NewID(std::string & seed);

	//�����µ�OBjectID, �����׵� ID, Ϊ�� ����32λ����ID ����
	static unsigned int NewID32();
};
