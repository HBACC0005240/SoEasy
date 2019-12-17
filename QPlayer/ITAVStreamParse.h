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
#define __STDC_CONSTANT_MACROS		//屏蔽一些错误？  ffmpeg中的一些宏开关
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
	int nb_packets;		///pktList数量
	int size;			///pktList总字节数
	ITMutex* mutex;	///线程锁
} PacketQueue;

///音视频解码类，返回音视频buff
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
	AVFormatContext* m_pFormatCtx;	//文件格式化ctx
	int		m_nVStreamIndex;		//视频index
	int		m_nAStreamIndex;		//音频index
	AVCodecContext* m_pVCodecCtx;	//视频ctx
	AVCodec*		m_pVCodec;		//视频编码器
	AVCodec*		m_pACodec;		//音频编码器
	AVCodecContext*	m_pACodecCtx;	//音频Ctx
	AVStream*		m_pVideoStream;
	AVStream*		m_pAudioStream;
	int				m_nTotalSecond;	//总秒数
	SwsContext*		m_pVConvertCtx;	//视频转换
	SwrContext*		m_pAConvertCtx;	//音频转换
	unsigned char*	m_pH264PPS;		//h264PPS
	unsigned char*  m_pH264SPS;		//h264SPS
	unsigned char*  m_pVExtradata;	//视频扩展数据，处理过的
	int				m_nVExtradataLen;//大小
	int				m_nH264PPSLen;	//h264PPSLen
	int				m_nH264SPSLen;	//h624SPSLen
	AVBitStreamFilterContext*	m_pVBitStreamFilterCtx;///视频流过滤ctx
	AVBitStreamFilterContext*	m_pABitStreamFilterCtx;///音频流过滤Ctx
	///当前帧数和当前时间，是顺序增加的，如果跳转过，则不准，必须以总时间自己计算了
	int		m_nFileFrameCount;		//文件总帧数
	int		m_nCurReadFrameIndex;	//当前读取的帧位置
	int		m_nCurReadFrameTime;	//当前读取帧所在时间
	bool	m_bSeekFromPts;			//是否有获取到pts信息
	int		m_nFrameRate;			//帧率
	PacketQueue*	m_pVideoPacketQueue;//视频队列包
	PacketQueue*	m_pAudioPacketQueue;//音频队列包
	int		m_nDstVideoQueuePktBuff;	///指定视频队列缓冲包大小	按最小的 
	int		m_nDstVideoQueueByteBuff;	///指定视频队列缓冲字节大小 
	int		m_nDstAudioQueuePktBuff;
	int		m_nDstAudioQueueByteBuff;

	bool	m_bReadFrameEnd;	//是否已经读取文件结束
	bool	m_bCloseReadThread;	///是否关闭读取线程
	ITThread*	m_pReadThread;

	int		m_nAudioChannels;		///音频通道
	int		m_nAudioSampleRate;		///音频采样率
	int		m_nAudioBitRate;		///播放bitrate
	uint8_t* m_pAudioConvertBuf;	///音频转换用buff
	int		m_nAudioConvertBufSize;	//音频转换缓冲区大小
	double  m_dAudioClock;			///音频pts转换后的时钟
	//	int		m_nAudioSamples;	///音频采样数	
	//	bool	m_bFirstGetNal;		///第一次读取nal数据  返回sps+pps
	bool	m_bAudioBuffEmpty;		//音频数据是否已读取完毕
	bool	m_bVideoBuffEmpty;		///视频数据是否已读取完毕
	//video
	uint8_t* m_pVideoConvertBuf;	///视频转换buff
	int		m_nVideoConvertBufSize;	///视频转换缓冲区大小
	int		m_nVideoWidth;
	int		m_nVideoHeight;
	double	m_dVideoClock;		///视频时钟
};

