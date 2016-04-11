#pragma once
typedef enum ITAVErrno
{
	ITAV_NoEro = 0,
	ITAV_ContextEro = 1,	///���Ӿ������
	ITAV_UnknownOpeType,	///δ֪�������� �ο�Gloabl_Define.h ö��OperationType
	ITAV_DataIncomplete,	///���ݲ�ȫ ���� ����1֡
	ITAV_EncodeEro,			///�������
	ITAV_DecodeEro,			///�������
	ITAV_EncodeInBuffFlow,	///���뻺�����
	ITAV_EncodeOutBuffFlow,	///�������������� ��������ݴ�ŵĻ��� ��ǰ�������ݹ����»���buf����
	ITAV_DecodeInBuffFlow,	///���뻺�����
	ITAV_DecodeOutBuffFlow,	///�������������� ��������ݴ�ŵĻ��� ��ǰ�������ݹ����»���buf����
	ITAV_CreateNewStreamEro,///���������ʧ�ܣ��������context����
	ITAV_FindEncoderEro,	///���ұ�����ʧ��
	ITAV_FindDecoderEro,	///���ҽ�����ʧ��
	ITAV_OpenEncoderEro,	///�򿪱�����ʧ�� �鿴��������
	ITAV_OpenDecoderEro,	///�򿪽�����ʧ��
	ITAV_EncoderParamCfgEro,///����������ô���
	ITAV_EncoderParamCfgOk,	///����������óɹ�
	ITAV_DecoderParamCfgEro,///����������ô���
	ITAV_DecoderParamCfgOk,	///����������óɹ�	
	ITAV_EncodeOpenInEro,	///������ı�������ʧ��
	ITAV_DecodeOpenInEro,	///������Ľ�������ʧ��
	ITAV_EncodeFindStreamEro,///������������Ϣʧ��
	ITAV_DecodeFindStreamEro,///������������Ϣʧ��
	ITAV_DecodeFailBuffFlow,///����ʧ�����ݻ���buf���
	ITAV_EncodeFailBuffFlow,///����ʧ�����ݻ���buf���
	ITAV_DecodeAllocCtxFailed,	///��������ڴ�ʧ��
	ITAV_EncodeAllocCtxFailed,	///��������ڴ�ʧ��
	ITAV_DecodeAllocFrameFailed,///���������֡�ڴ�ʧ��
	ITAV_EncodeAllocFrameFailed,///���������֡�ڴ�ʧ��
	ITAV_DecodeFrameCfgFailed,///����֡���ô���
	ITAV_EncodeFrameCfgFailed,///����֡���ô���
	ITAV_DecodeCodecParserCtxFailed,///����ʹ�õĽ��������Ĵ���
	ITAV_EncodeCodecParserCtxFailed,///����ʹ�õĽ��������Ĵ���
	
	ITAV_ParamOpeTypeSetEro = 0x0100,///���������У������������ô���
	ITAV_ParamSetEro = 0x0100,		///�������ô���

}ITAVErrno;
