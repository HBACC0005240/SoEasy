#pragma once

#ifndef int64_t
typedef long long  int64_t;
#endif

///��������
typedef enum OperationType
{
	OPE_NONE = 0,						///��
	OPE_Audio = 0x0100,					///��Ƶ
	OPE_AudioEncode = 0x0101,			///��Ƶ����
	OPE_AudioDecode = 0x0102,			///��Ƶ����
	OPE_AudioDecodeFile = 0x0103,		///���������Ƶ�ļ�

	OPE_Video = 0x0200,					///��Ƶ
	OPE_VideoEncode = 0x0201,			///��Ƶ����
	OPE_VideoDecode = 0x0202,			///��Ƶ����
	OPE_VideoDecodeFile = 0x0203,		///���������Ƶ�ļ�
	
	OPE_AudioEncode_Hardware = 0x0301,	///��Ƶ���� Ӳ��
	OPE_AudioDecode_Hardware = 0x0302,	///��Ƶ���� Ӳ��
	OPE_VideoEncode_Hardware = 0x0401,	///��Ƶ���� Ӳ��
	OPE_VideoDecode_Hardware = 0x0402,	///��Ƶ���� Ӳ��


	OPE_AVMuxer = 0x300,				///����Ƶ�ļ�����
	
	OPE_AVMemuxer = 0x310,				///����Ƶ�ϲ�
	OPE_AuidoMemuxer = 0x311,			///��Ƶ���úϲ�
	OPE_VideoMemuxer = 0x312,			///��Ƶ���úϲ�
	OPE_AVDemuxer = 0x320,				///����Ƶ����
	OPE_AudioDemuxer = 0x321,			///��Ƶ����
	OPE_VideoDemuxer = 0x322,			///��Ƶ����
	OPE_AVRemuxer = 0x330,				///����Ƶת��
	OPE_AudioRemuxer = 0x331,			///��Ƶת��
	OPE_VideoRemuxer = 0x332,			///��Ƶת��
};
///��������Ƶ���������
///��ֻ�а�׿֧��Ӳ����� ����ƽ̨������
///Ĭ�϶�Ϊ������
typedef enum ITAVEncodeDecodeType
{
	ITAV_ENCODE_Audio_Software,	///�����Ƶ
	ITAV_ENCODE_Audio_Hardware,	///Ӳ����Ƶ

	ITAV_ENCODE_Video_Software,	///�����Ƶ	
	ITAV_ENCODE_Video_Hardware,	///Ӳ����Ƶ

	ITAV_DECODE_Audio_Software,	///�����Ƶ
	ITAV_DECODE_Audio_Hardware,	///Ӳ����Ƶ

	ITAV_DECODE_Video_Software,	///�����Ƶ		
	ITAV_DECODE_Video_Hardware,	///Ӳ����Ƶ
};
///����Ƶ�ϲ� ��������
///CodecType��CodecID ���ַ���������һ�ּ���
typedef struct ITAVCodecCtxFmt	
{
	///���ò���
	char strDstCodecType[100];	///�����ʽ�ַ���������".aac"���ļ���׺�����ɣ�
	char strSrcCodecType[100];	///ԭ�ļ���ʽ  
	int nSrcCodecID;			///�����ԭ��ʽ ITAVCodecIDö��ֵ	
	int	nDstCodecID;			///�����Ŀ���ʽITAVCodecIDö��ֵ
	int nSrcBitRate;			///ԭ����	���� ��Ƶ��˵��һ�벥���� ���� 64000 128000  64kbp/s 128kbp/s
	int nDstBitRate;			///Ŀ������

	///��Ƶ����		///����ʱ����Ҫԭ��ʽ ���Ŀ��û���ã�����ԭ��ʽ���б��� �������ز��� ���������Ŀ���ʽ������ز���
					//����ʱ�� ԭ��Ŀ�궼����Ҫ���ã������Ҫ�ز���������Ŀ����뼴��		
	int nSrcSampleRate;			///������ ���� 44100
	int nSrcSampleNb;			///������ ����aac 1024 �����޸�
	int nSrcChannels;			///ͨ������������������������������
	int nSrcSampleFmt;			///������ʽ ITAVSampleFmtö��ֵ 

	int nDstSampleRate;			///������ ���� 44100
	int nDstSampleNb;			///������ ����aac 1024 �����޸�
	int nDstChannels;			///ͨ������������������������������
	int nDstSampleFmt;			///������ʽ ITAVSampleFmtö��ֵ 


	///��Ƶ����
	int nSrcWidth;				///ԭ��
	int nSrcHeight;				///ԭ��
	int nDstWidth;				///Ŀ���
	int nDstHeight;				///Ŀ���	
	int nGopSize;				///ͼƬ���С ������ΪͼƬ����  ��������	
	int nSrcTimeBaseNum;		///< numerator	ͨ��Ϊ1 ���Ͽ��� ��PTSת��Ϊʵ�ʵ�ʱ�䣨��λΪ��s��
	int nSrcTimeBaseDen;		///< denominator ֡�� 
	int nDstTimeBaseNum;		///< numerator	ͨ��Ϊ1 ���Ͽ��� ��PTSת��Ϊʵ�ʵ�ʱ�䣨��λΪ��s��
	int nDstTimeBaseDen;		///< denominator ֡�� 
	int nSrcPixelFmt;			///ԭ����		���ͬһ������ ���洢��ʽ��ͬ����������
	int nDstPixelFmt;			///Ŀ�����ظ�ʽ
	int nSrcFrameRateNum;		
	int nSrcFrameRateDen;
	int nDstFrameRateNum;		///ֻ�н���ʱ��Ч��  ����Ҫ���ã�����timeBase���ֻ��е��ã�FrameRate������Ч�� ���ܱ���� 
	int nDstFrameRateDen;		
	int64_t nCurDecodePts;		///��ǰ������pts

	///�ϲ���Ƶ�ļ����� �����ļ�Ҳ����
	char strDstAVFile[256];		///Ŀ��AV ����Ƶ�ϲ��ļ�
	char strDstAudioFile[256];	///Ŀ����Ƶ�ļ�
	char strDstVideoFile[256];	///Ŀ����Ƶ�ļ�
	char strSrcAudioFile[256];	///ԭ��Ƶ�ļ�
	char strSrcVideoFile[256];	///ԭ��Ƶ�ļ�
	char strSrcAVFile[256];		///ԭAV ����Ƶ�ļ�
}ITAVCODECCTXFMT;


#define MAXFrameSize 1000
typedef struct ITAVDecodeFrame
{
	ITAVCodecCtxFmt		struVideoFmt;			///�����ṹ��
	unsigned 	int 	nFrameSize;				//�ܽṹ����
	unsigned	int		nDataSize[MAXFrameSize];///ÿ��֡����
	unsigned char*		strData[MAXFrameSize];	///֡����
}ITAVDECODEFRAME;
///���ñ����Ŀ������  ��׺����ʽ
typedef enum ITAVCodecID
{
	ITAV_CODEC_None=0,
	ITAV_CODEC_Audio=0x0100,
	ITAV_CODEC_AAC,
	ITAV_CODEC_MP2,
	ITAV_CODEC_MP3,
	ITAV_CODEC_WAV,
	ITAV_CODEC_PCM,	///�ֺܶ���  �۲�����ôϸ ֱ��ʹ��Ĭ�����ü���	
	ITAV_CODEC_Video=0x0200,
	ITAV_CODEC_H264,
	ITAV_CODEC_MP4,
	ITAV_CODEC_YUV,
	ITAV_CODEC_M2V,
	ITAV_CODEC_H265,
	ITAV_CODEC_HEVC,
	ITAV_CODEC_RM,
	ITAV_CODEC_AVI,	
	ITAV_CODEC_RGB,	
	ITAV_CODEC_FLV,
	ITAV_CODEC_MKV,
	ITAV_CODEC_RMVB,
	ITAV_CODEC_TS,
	ITAV_CODEC_MPEG4,
};
///��Ƶ���  ���ñ����Ŀ������  ��׺����ʽ
//���涼��һЩ���ظ�ʽ 
typedef enum ITAVPixelID
{
	ITAV_PIXEL_NONE,
	ITAV_PIXEL_GRAY8=0x0100,
	ITAV_PIXEL_YUV420P,
	ITAV_PIXEL_YUV420SP,		///NV12 NV21������YUV420SP  
	ITAV_PIXEL_YUV422P,
	ITAV_PIXEL_YUV444P,
	ITAV_PIXEL_YUYV422,
	ITAV_PIXEL_RGB24,	
	ITAV_PIXEL_RGB32,
	ITAV_PIXEL_BGR24,
	ITAV_PIXEL_NV12,///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	ITAV_PIXEL_NV21,///< as above, but U and V bytes are swapped  ��NV12һ��������UV����	vu vu 	
};
///��Ƶ����λ��ʽ���� 
//p��Ϊƽ���ʽ
//8λ��������16��������32 �Լ��������� 
typedef enum ITAVSampleFmt
{
	ITAV_SAMPLE_FMT_NONE=-1,	///��
	ITAV_SAMPLE_FMT_U8=1,          ///< unsigned 8 bits
	ITAV_SAMPLE_FMT_S16,         ///< signed 16 bits
	ITAV_SAMPLE_FMT_S32,         ///< signed 32 bits
	ITAV_SAMPLE_FMT_FLT,         ///< float
	ITAV_SAMPLE_FMT_DBL,         ///< double

	ITAV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
	ITAV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
	ITAV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
	ITAV_SAMPLE_FMT_FLTP,        ///< float, planar
	ITAV_SAMPLE_FMT_DBLP,        ///< double, planar

	ITAV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};
//#define GETDEVSUPPERCLASS(n)	( n & 0xfffff000)			
//#define GETDEVCLASS(n)	( n & 0xffffff00)			
//#define GETDEVSUBCLASS(n)	( n & 0xfffffff0)	

#ifndef GETDEVSUBCLASS
#define GETDEVSUBCLASS(n) (n & 0xfffffff0)
#endif

#ifndef GETDEVCLASS
#define GETDEVCLASS(n)	( n & 0xffffff00)		
#endif

#ifndef GETDEVSUPPERCLASS
#define GETDEVSUPPERCLASS(n)	( n & 0xfffff000)
#endif

#ifdef OS_LINUX
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif
