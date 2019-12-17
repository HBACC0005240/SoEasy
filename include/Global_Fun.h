#pragma once
#include "Global_Define.h"
#include <windows.h>

static int isnan(float x)
{
	uint32_t v = av_float2int(x);
	if ((v & 0x7f800000) != 0x7f800000)
		return 0;
	return v & 0x007fffff;
}
static inline unsigned char CONVERT_ADJUST(double tmp)
{
	return (unsigned char)((tmp >= 0 && tmp <= 255)?tmp:(tmp < 0 ? 0 : 255));
}
//YUV420P to RGB24
static void CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight)
{
	unsigned char *tmpbuf=(unsigned char *)malloc(nWidth*nHeight*3);
	unsigned char Y,U,V,R,G,B;
	unsigned char* y_planar,*u_planar,*v_planar;
	int rgb_width , u_width;
	rgb_width = nWidth * 3;
	u_width = (nWidth >> 1);
	int ypSize = nWidth * nHeight;
	int upSize = (ypSize>>2);
	int offSet = 0;

	y_planar = yuv_src;
	u_planar = yuv_src + ypSize;
	v_planar = u_planar + upSize;

	for(int i = 0; i < nHeight; i++)
	{
		for(int j = 0; j < nWidth; j ++)
		{
			Y = *(y_planar + nWidth * i + j);
			offSet = (i>>1) * (u_width) + (j>>1);
			V = *(u_planar + offSet);
			U = *(v_planar + offSet);

			R = CONVERT_ADJUST((Y + (1.4075 * (V - 128))));
			G = CONVERT_ADJUST((Y - (0.3455 * (U - 128) - 0.7169 * (V - 128))));
			B = CONVERT_ADJUST((Y + (1.7790 * (U - 128))));			
			offSet = rgb_width * i + j * 3;

			rgb_dst[offSet] = B;
			rgb_dst[offSet + 1] = G;
			rgb_dst[offSet + 2] = R;
		}
	}
	free(tmpbuf);
}
static AVCodecID TranslationCodecID(int nID)
{
	AVCodecID avcodecid=AV_CODEC_ID_NONE;
	switch(nID)
	{
	case ITAV_CODEC_AAC:
		avcodecid = AV_CODEC_ID_AAC;
		break;
	case ITAV_CODEC_MP2:
		avcodecid = AV_CODEC_ID_MP2;
		break;
	case ITAV_CODEC_MP3:
		avcodecid = AV_CODEC_ID_MP3;
		break;
	case ITAV_CODEC_WAV:
		avcodecid = AV_CODEC_ID_ADPCM_IMA_WAV;
		break;
	case ITAV_CODEC_PCM:
//		avcodecid = AV_CODEC_ID_PCM;
		break;
	case ITAV_CODEC_H264:
		avcodecid = AV_CODEC_ID_H264;
		break;
	case ITAV_CODEC_MP4:
		avcodecid = AV_CODEC_ID_MP4ALS;
		break;
	case ITAV_CODEC_YUV:
		avcodecid = AV_CODEC_ID_YUV4;
		break;
	case ITAV_CODEC_M2V:
		avcodecid = AV_CODEC_ID_MPEG2VIDEO;
		break;
	case ITAV_CODEC_HEVC:
	case ITAV_CODEC_H265:
		avcodecid = AV_CODEC_ID_HEVC;
		break;
	case ITAV_CODEC_RM:
//		avcodecid = AV_CODEC_ID_RM;
		break;
	case ITAV_CODEC_AVI:
//		avcodecid = AV_CODEC_ID_AVI;
		break;
	case ITAV_CODEC_FLV:
		avcodecid = AV_CODEC_ID_FLV1;
		break;
	case ITAV_CODEC_MPEG4:
		avcodecid = AV_CODEC_ID_MPEG4;
		break;
	default:
		break;
	}
	return avcodecid;
}
///根据id转换出匹配的格式  音频无此项
static AVPixelFormat TranslationPixelFmt(int nID)
{
	AVPixelFormat avpixelfmt=AV_PIX_FMT_NONE;
	switch(nID)
	{	
	case ITAV_PIXEL_GRAY8:
		avpixelfmt = AV_PIX_FMT_GRAY8;
		break;
	case ITAV_PIXEL_YUV420P:
		avpixelfmt = AV_PIX_FMT_YUV420P;
		break;
	case ITAV_PIXEL_YUV420SP:
		avpixelfmt = AV_PIX_FMT_NV21;
		break;
	case ITAV_PIXEL_NV21:
		avpixelfmt = AV_PIX_FMT_NV21;
		break;
	case ITAV_PIXEL_NV12:
		avpixelfmt = AV_PIX_FMT_NV12;
		break;
	case ITAV_PIXEL_YUV422P:
		avpixelfmt = AV_PIX_FMT_YUV422P;
		break;
	case ITAV_PIXEL_YUV444P:
		avpixelfmt = AV_PIX_FMT_YUV444P;
		break;
	case ITAV_PIXEL_YUYV422:
		avpixelfmt = AV_PIX_FMT_YUYV422;
		break;
	case ITAV_PIXEL_RGB24:
		avpixelfmt = AV_PIX_FMT_RGB24;
		break;
	case ITAV_PIXEL_RGB32:
		avpixelfmt = AV_PIX_FMT_RGB32;	///实际和这个值貌似一样AV_PIX_FMT_BGRA
		break;
	case ITAV_PIXEL_BGR24:
		avpixelfmt = AV_PIX_FMT_BGR24;
		break;
	default:
		break;
	}
	return avpixelfmt;
}
static int TranslationPixelFmt(AVPixelFormat nFmt)
{
	int avpixelID=ITAV_PIXEL_NONE;
	switch(nFmt)
	{	
	case AV_PIX_FMT_GRAY8:
		avpixelID = ITAV_PIXEL_GRAY8;
		break;
	case AV_PIX_FMT_YUV420P:
		avpixelID = ITAV_PIXEL_YUV420P;
		break;
	case AV_PIX_FMT_NV12:
		avpixelID = ITAV_PIXEL_NV12;
		break;
	case AV_PIX_FMT_NV21:
		avpixelID = ITAV_PIXEL_NV21;
		break;
	case AV_PIX_FMT_YUV422P:
		avpixelID = ITAV_PIXEL_YUV422P;
		break;
	case AV_PIX_FMT_YUV444P:
		avpixelID = ITAV_PIXEL_YUV444P;
		break;
	case AV_PIX_FMT_YUYV422:
		avpixelID = ITAV_PIXEL_YUYV422;
		break;
	case AV_PIX_FMT_RGB24:
		avpixelID = ITAV_PIXEL_RGB24;
		break;
	case AV_PIX_FMT_RGB32:
		avpixelID = ITAV_PIXEL_RGB32;	///实际和这个值貌似一样AV_PIX_FMT_BGRA
		break;
	case AV_PIX_FMT_BGR24:
		avpixelID = ITAV_PIXEL_BGR24;
		break;
	default:
		break;
	}
	return avpixelID;
}
//************************************
//函数名:  TranslationCodecID
//描述：转换ITAVSampleFmt至AVSampleFormat
//参数：ITAVSampleFmt枚举值
//返回值：AVSampleFormat
//时间：2015/9/7 WZQ
//************************************
static AVSampleFormat TranslationSampleFmt(int nFmt)
{
	AVSampleFormat sampleFmt=AV_SAMPLE_FMT_NONE;
	switch(nFmt)
	{
	case ITAV_SAMPLE_FMT_U8:
		sampleFmt = AV_SAMPLE_FMT_U8;
		break;
	case ITAV_SAMPLE_FMT_S16:
		sampleFmt = AV_SAMPLE_FMT_S16;
		break;
	case ITAV_SAMPLE_FMT_S32:
		sampleFmt = AV_SAMPLE_FMT_S32;
		break;
	case ITAV_SAMPLE_FMT_FLT:
		sampleFmt = AV_SAMPLE_FMT_FLT;
		break;
	case ITAV_SAMPLE_FMT_DBL:
		sampleFmt = AV_SAMPLE_FMT_DBL;
		break;
	case ITAV_SAMPLE_FMT_U8P:
		sampleFmt = AV_SAMPLE_FMT_U8P;
		break;
	case ITAV_SAMPLE_FMT_S16P:
		sampleFmt = AV_SAMPLE_FMT_S16P;
		break;
	case ITAV_SAMPLE_FMT_S32P:
		sampleFmt = AV_SAMPLE_FMT_S32P;
		break;
	case ITAV_SAMPLE_FMT_FLTP:
		sampleFmt = AV_SAMPLE_FMT_FLTP;
		break;
	case ITAV_SAMPLE_FMT_DBLP:
		sampleFmt = AV_SAMPLE_FMT_DBLP;
		break;
	case ITAV_SAMPLE_FMT_NB:
		sampleFmt = AV_SAMPLE_FMT_NB;
		break;
	default:
		break;
	}
	return sampleFmt;
}
static int TranslationSampleFmt(AVSampleFormat nFmt)
{
	int sampleFmt=ITAV_SAMPLE_FMT_NONE;
	switch(nFmt)
	{
	case AV_SAMPLE_FMT_U8:
		sampleFmt = ITAV_SAMPLE_FMT_U8;
		break;
	case AV_SAMPLE_FMT_S16:
		sampleFmt = ITAV_SAMPLE_FMT_S16;
		break;
	case AV_SAMPLE_FMT_S32:
		sampleFmt = ITAV_SAMPLE_FMT_S32;
		break;
	case AV_SAMPLE_FMT_FLT:
		sampleFmt = ITAV_SAMPLE_FMT_FLT;
		break;
	case AV_SAMPLE_FMT_DBL:
		sampleFmt = ITAV_SAMPLE_FMT_DBL;
		break;
	case AV_SAMPLE_FMT_U8P:
		sampleFmt = ITAV_SAMPLE_FMT_U8P;
		break;
	case AV_SAMPLE_FMT_S16P:
		sampleFmt = ITAV_SAMPLE_FMT_S16P;
		break;
	case AV_SAMPLE_FMT_S32P:
		sampleFmt = ITAV_SAMPLE_FMT_S32P;
		break;
	case AV_SAMPLE_FMT_FLTP:
		sampleFmt = ITAV_SAMPLE_FMT_FLTP;
		break;
	case AV_SAMPLE_FMT_DBLP:
		sampleFmt = ITAV_SAMPLE_FMT_DBLP;
		break;
	case AV_SAMPLE_FMT_NB:
		sampleFmt = ITAV_SAMPLE_FMT_NB;
		break;
	default:
		break;
	}
	return sampleFmt;
}

//ANSI转化成UNICODE
static LPWSTR ANSITOUNICODE(const char* pBuf)
{
	int lenA = lstrlenA(pBuf);
	int lenW=0;
	LPWSTR lpszFile=NULL;
	lenW = MultiByteToWideChar(CP_ACP,0,pBuf,lenA,0,0);	
	if(lenW > 0)
	{
		lpszFile = SysAllocStringLen(0,lenW);	
		MultiByteToWideChar(CP_ACP,0,pBuf,lenA,lpszFile,lenW);	//
	}
	return lpszFile;
} 
