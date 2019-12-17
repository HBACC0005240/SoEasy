#pragma once
#include "ITMutex.h"
#include "ITThread.h"
#include <string>
using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif
#define UINT64_C(val) val##ULL
#define __STDC_CONSTANT_MACROS		//����һЩ����  ffmpeg�е�һЩ�꿪��
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/frame.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
};
#endif
#include "Global_ErrorNo.h"
#include "Global_Define.h"
#include "Global_Fun.h"


typedef struct PacketQueue {
	AVPacketList *first_pkt, *last_pkt;
	int nb_packets;		///pktList����
	int size;			///pktList���ֽ���
	ITMutex* mutex;	///�߳���
} PacketQueue;

///����Ƶ�����࣬��������Ƶbuff
class ITAVStreamParse
{
public:
	ITAVStreamParse(void);
	~ITAVStreamParse(void);

	bool openDstAVFile(const char* strPath,ITAVCodecCtxFmt* pFmt=NULL);
	bool getNextNalData(int64_t& pts,unsigned char** strData,int& nLen);
	bool getNextAudioData(int64_t& pts,unsigned char** pData,int& nLen);
	bool getNextYuvData(int64_t& pts,unsigned char** pData,int& nLen,int& nWidth,int& nHeight,int& nSynVal);
	bool getNextPcmData(int64_t& pts,unsigned char** pData,int& nLen);
	bool isAudioBuffEmpty();
	bool isVideoBuffEmpty();
	bool closeDstAVFile();

	///audio info
	bool getAudioInitState(){return m_pACodecCtx?true:false;}
	int getAudioFrequency(){return m_nAudioSampleRate;}
	int getAudioChannel(){return m_nAudioChannels;}
	int getAudioBitRate(){return m_nAudioBitRate;}
	double getAudioClock(){return m_dAudioClock;}
	int getAudioBuffSize();

	///Video info
	bool getVideoInitState(){return m_pVCodecCtx?true:false;}
	int getVideoWidth(){return m_nVideoWidth;}
	int getVideoHeight(){return m_nVideoHeight;}
	double getVideoClock(){return m_dVideoClock;}
protected:
	static void read_thread(void *arg);	
	void packet_queue_init(PacketQueue *q);
	void packet_queue_clear(PacketQueue *q);
	bool packet_queue_put(PacketQueue *q, AVPacket *pkt);
	bool packet_queue_get(PacketQueue *q, AVPacket *pkt);
private:
	std::string m_strDstAVFile;
	std::string			m_lastError;
	AVFormatContext* m_pFormatCtx;	//�ļ���ʽ��ctx
	int		m_nVStreamIndex;		//��Ƶindex
	int		m_nAStreamIndex;		//��Ƶindex
	AVCodecContext* m_pVCodecCtx;	//��Ƶctx
	AVCodec*		m_pVCodec;		//��Ƶ������
	AVCodec*		m_pACodec;		//��Ƶ������
	AVCodecContext*	m_pACodecCtx;	//��ƵCtx
	AVStream*		m_pVideoStream;
	AVStream*		m_pAudioStream;
	int				m_nTotalSecond;	//������
	SwsContext*		m_pVConvertCtx;	//��Ƶת��
	SwrContext*		m_pAConvertCtx;	//��Ƶת��
	unsigned char*	m_pH264PPS;		//h264PPS
	unsigned char*  m_pH264SPS;		//h264SPS
	unsigned char*  m_pVExtradata;	//��Ƶ��չ���ݣ��������
	int				m_nVExtradataLen;//��С
	int				m_nH264PPSLen;	//h264PPSLen
	int				m_nH264SPSLen;	//h624SPSLen
	AVBitStreamFilterContext*	m_pVBitStreamFilterCtx;///��Ƶ������ctx
	AVBitStreamFilterContext*	m_pABitStreamFilterCtx;///��Ƶ������Ctx
	///��ǰ֡���͵�ǰʱ�䣬��˳�����ӵģ������ת������׼����������ʱ���Լ�������
	int		m_nFileFrameCount;		//�ļ���֡��
	int		m_nCurReadFrameIndex;	//��ǰ��ȡ��֡λ��
	int		m_nCurReadFrameTime;	//��ǰ��ȡ֡����ʱ��
	bool	m_bSeekFromPts;			//�Ƿ��л�ȡ��pts��Ϣ
	int		m_nFrameRate;			//֡��
	PacketQueue*	m_pVideoPacketQueue;//��Ƶ���а�
	PacketQueue*	m_pAudioPacketQueue;//��Ƶ���а�
	int		m_nDstVideoQueuePktBuff;	///ָ����Ƶ���л������С	����С�� 
	int		m_nDstVideoQueueByteBuff;	///ָ����Ƶ���л����ֽڴ�С 
	int		m_nDstAudioQueuePktBuff;
	int		m_nDstAudioQueueByteBuff;

	bool	m_bReadFrameEnd;	//�Ƿ��Ѿ���ȡ�ļ�����
	bool	m_bCloseReadThread;	///�Ƿ�رն�ȡ�߳�
	ITThread*	m_pReadThread;

	int		m_nAudioChannels;		///��Ƶͨ��
	int		m_nAudioSampleRate;		///��Ƶ������
	int		m_nAudioBitRate;		///����bitrate
	uint8_t* m_pAudioConvertBuf;	///��Ƶת����buff
	int		m_nAudioConvertBufSize;	//��Ƶת����������С
	double  m_dAudioClock;			///��Ƶptsת�����ʱ��
	//	int		m_nAudioSamples;	///��Ƶ������	
	//	bool	m_bFirstGetNal;		///��һ�ζ�ȡnal����  ����sps+pps
	bool	m_bAudioBuffEmpty;		//��Ƶ�����Ƿ��Ѷ�ȡ���
	bool	m_bVideoBuffEmpty;		///��Ƶ�����Ƿ��Ѷ�ȡ���
	//video
	uint8_t* m_pVideoConvertBuf;	///��Ƶת��buff
	int		m_nVideoConvertBufSize;	///��Ƶת����������С
	int		m_nVideoWidth;
	int		m_nVideoHeight;
	double	m_dVideoClock;		///��Ƶʱ��
};

