#include "stdafx.h"
#include "ITAVStreamParse.h"
#include "io.h"


ITAVStreamParse::ITAVStreamParse(void)
{
	av_register_all();
	m_pVConvertCtx = NULL;
	avformat_network_init();
	m_nVStreamIndex = -1;
	m_pFormatCtx = NULL;
	m_nAStreamIndex = -1;
	m_pVCodecCtx = NULL;
	m_pVCodec = NULL;
	m_pACodecCtx=NULL;
	m_pACodec=NULL;
	m_nTotalSecond = 0;
	m_pH264PPS = NULL;
	m_pH264SPS = NULL;
	m_nH264PPSLen = 0;
	m_nH264SPSLen = 0;
	m_nFileFrameCount = 0;
	m_nCurReadFrameIndex = 0;
	m_nCurReadFrameTime = 0;
	m_bSeekFromPts = true;
	m_nFrameRate = 25;	//初始化1秒25帧	
	m_bReadFrameEnd = false;
	m_pVideoPacketQueue = new PacketQueue;
	memset(m_pVideoPacketQueue,0,sizeof(PacketQueue));
	m_pAudioPacketQueue = new PacketQueue;
	memset(m_pAudioPacketQueue,0,sizeof(PacketQueue));
	///文件存储是一段视频 一段音频，读取时候可能5、6个视频包后，才开始有音频包
	///所以一次性大于10就sleep来等数据被取出再继续
	///字节部分，暂时不考虑，因为有的h264可变帧数据不好控制
	m_nDstVideoQueuePktBuff = 50;	//初始化可以缓存50个NAL包以上
	m_nDstVideoQueueByteBuff = 0;
	m_nDstAudioQueuePktBuff = 50;	//初始化可以缓存50个音频包
	m_nDstAudioQueueByteBuff = 0;
	m_bCloseReadThread = false;
	m_pReadThread = NULL;
	m_nAudioChannels=2;	///音频通道
	m_nAudioSampleRate=44100;	///音频采样率
	m_nAudioBitRate=44100;
	//	m_nAudioSamples=1024;	///音频采样数
	m_pVBitStreamFilterCtx = NULL;
	m_pABitStreamFilterCtx = NULL;
	m_pVExtradata = NULL;
	m_nVExtradataLen = 0;
	m_pVConvertCtx = NULL;
	m_pAConvertCtx=NULL;
	m_pAudioConvertBuf=NULL;
	m_nAudioConvertBufSize=0;
	m_dAudioClock=0;
	m_bAudioBuffEmpty=false;
	m_bVideoBuffEmpty=false;
	m_pVideoConvertBuf=NULL;
	m_nVideoConvertBufSize=0;
	m_pAudioStream=NULL;
	m_pVideoStream=NULL;
}


ITAVStreamParse::~ITAVStreamParse(void)
{
}

bool ITAVStreamParse::openDstAVFile(const char* strPath,ITAVCodecCtxFmt* pFmt)
{
	m_bCloseReadThread=false;
	m_pAudioConvertBuf=false;
	//if((access(strPath,0))==-1)	//文件不存在 有些检测不出来
	//	return false;	
	m_strDstAVFile = strPath;	
	
	m_pFormatCtx=avformat_alloc_context();
	int err = avformat_open_input(&m_pFormatCtx,strPath, NULL,NULL);
	if (err < 0) 
	{		
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
		return false;
	}
	err = avformat_find_stream_info(m_pFormatCtx, NULL);
	if (err < 0 && !pFmt)	//如果没找到信息，并且没传入所需数据，则退出
	{
		av_log(NULL, AV_LOG_WARNING,"%s: could not find codec parameters\n", m_pFormatCtx->filename);
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
		return false;
	}
	av_dump_format(m_pFormatCtx,NULL,strPath,0);
	for(int i=0; i<m_pFormatCtx->nb_streams; i++) 
	{
		if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			m_nVStreamIndex=i;
			continue;
		}else if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
		{
			m_nAStreamIndex = i;
			continue;
		}
	}
	if(m_nVStreamIndex == -1 && m_nAStreamIndex==-1)	//2个都没找到  退出，字幕先不考虑
	{
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
		return false;
	}
	if(m_nVStreamIndex != -1)
	{
		m_pVCodecCtx=m_pFormatCtx->streams[m_nVStreamIndex]->codec;		
		m_pVideoStream=m_pFormatCtx->streams[m_nVStreamIndex];
	}
	if(m_nAStreamIndex != -1)
	{
		m_pACodecCtx =m_pFormatCtx->streams[m_nAStreamIndex]->codec;
		m_pAudioStream=m_pFormatCtx->streams[m_nAStreamIndex];;
	}
	if(m_pACodecCtx)
	{
		m_nAudioSampleRate = m_pACodecCtx->sample_rate;
		m_nAudioChannels = m_pACodecCtx->channels;	
		m_nAudioBitRate=m_pACodecCtx->bit_rate;
	}	
	if(m_pVCodecCtx)
	{
		m_pVCodec=avcodec_find_decoder(m_pVCodecCtx->codec_id);
		if(m_pVCodec)
		{
			if(avcodec_open2(m_pVCodecCtx, m_pVCodec,NULL)<0)
			{
				printf("Could not open codec.\n");
				avformat_free_context(m_pFormatCtx);
				m_pFormatCtx = NULL;
				m_pVCodecCtx = NULL;
				return false;
			}	
			if(m_pVCodecCtx->width <= 0 && pFmt)
			{
				m_pVCodecCtx->width=pFmt->nSrcWidth;
			}
			if(m_pVCodecCtx->height <= 0 && pFmt)
			{
				m_pVCodecCtx->height=pFmt->nSrcHeight;
			}
			m_pVConvertCtx = sws_getContext(m_pVCodecCtx->width, m_pVCodecCtx->height, m_pVCodecCtx->pix_fmt, 
				m_pVCodecCtx->width, m_pVCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
			m_nVideoWidth=m_pVCodecCtx->width;
			m_nVideoHeight=m_pVCodecCtx->height;
			///计算视频时长
			int nFrameCount=0;
			int64_t duration = m_pFormatCtx->duration;
			int hours=0;
			int mins=0;
			int secs=0;
			int us=0;
			bool bUseFrame = true;
			int nStreamFrameRateDen = m_pFormatCtx->streams[m_nVStreamIndex]->avg_frame_rate.den;
			if(nStreamFrameRateDen <= 0)
				nStreamFrameRateDen = 1;
			m_nFrameRate = m_pFormatCtx->streams[m_nVStreamIndex]->avg_frame_rate.num/nStreamFrameRateDen;
			if(duration <= 0)	//获取不到才以这个计算
			{
				m_bSeekFromPts = false;
				AVPacket packet;
				av_init_packet(&packet);
				packet.data = (uint8_t *)&packet;
				while(av_read_frame(m_pFormatCtx, &packet)>=0)
				{
					if(packet.stream_index==m_nVStreamIndex)
						nFrameCount++;
				}
				bUseFrame = false;
				secs = nFrameCount/m_nFrameRate;	
				avformat_seek_file(m_pFormatCtx,m_nVStreamIndex,INT64_MIN,0,INT64_MAX,2);
			}else
			{
				m_bSeekFromPts = true;	
				secs  = duration / AV_TIME_BASE;
				us    = duration % AV_TIME_BASE;
				nFrameCount = secs*m_nFrameRate;
			}	
			m_nFileFrameCount = nFrameCount;
			m_nTotalSecond = secs;	//总秒数
			mins  = secs / 60;
			secs %= 60;
			hours = mins / 60;
			mins %= 60;	
			printf("%02d:%02d:%02d.%02d", hours, mins, secs,(100 * us) / AV_TIME_BASE);//时分秒
			int64_t stremdu = m_pFormatCtx->streams[m_nVStreamIndex]->duration/AV_TIME_BASE;
			printf("总时间：%ld",m_pFormatCtx->duration* av_q2d(m_pFormatCtx->streams[m_nVStreamIndex]->time_base));	
		}
	}	
	if(m_pACodecCtx)
	{
		m_pACodec=avcodec_find_decoder(m_pACodecCtx->codec_id);
		if(m_pACodec)
		{
			if(avcodec_open2(m_pACodecCtx, m_pACodec,NULL)<0)
			{
				printf("Could not open codec.\n");
				avformat_free_context(m_pFormatCtx);
				m_pFormatCtx = NULL;
				m_pVCodecCtx = NULL;
				m_pACodecCtx = NULL;
				return false;
			}	
		}
		if(!m_pAConvertCtx)
		{
			m_pAConvertCtx = swr_alloc();
			int64_t out_channel_layout = av_get_default_channel_layout(m_nAudioChannels);
			AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;	
			int64_t in_channel_layout=av_get_default_channel_layout(m_nAudioChannels);///通道原样输出，最佳为根据硬件选择 
			m_pAConvertCtx=swr_alloc_set_opts(m_pAConvertCtx,out_channel_layout, out_sample_fmt, 44100,
				in_channel_layout,m_pACodecCtx->sample_fmt , m_pACodecCtx->sample_rate,0, NULL);
			swr_init(m_pAConvertCtx);	
		}
		/*if(!m_pAudioConvertBuf)
		{
			AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;	///AAC一般采样位1024
			int dst_nb_samples=1024;								
			dst_nb_samples = av_rescale_rnd(1024, 44100,m_nAudioSampleRate, AV_ROUND_UP);
			m_nAudioConvertBufSize=av_samples_get_buffer_size(NULL,m_nAudioChannels ,dst_nb_samples,out_sample_fmt, 1);	
			m_pAudioConvertBuf=(uint8_t *)av_malloc(m_nAudioConvertBufSize);				
			memset(m_pAudioConvertBuf,0,m_nAudioConvertBufSize);
			
		}*/
	}		
	int dummy_size;  		
	m_pReadThread = new ITThread("读取线程");
	m_pReadThread->Start(read_thread,this);
	return true;
}

void ITAVStreamParse::read_thread(void *arg)
{
	ITAVStreamParse* pThis = (ITAVStreamParse*)arg;
	if(!pThis)
		return;
	AVPacket packet;
	av_init_packet(&packet);
	packet.data = (uint8_t *)&packet;
	int nReadEroNum = 0;
	while(!pThis->m_bCloseReadThread)
	{
		if(!pThis->m_pFormatCtx)
			return;
		if(pThis->m_pVideoPacketQueue->nb_packets >= pThis->m_nDstVideoQueuePktBuff && \
			pThis->m_pAudioPacketQueue->nb_packets >= pThis->m_nDstAudioQueuePktBuff)
		{
			Sleep(30);	//sleep 300毫秒	最好有个唤醒的一个线程机制来
			continue;
		}
		if(av_read_frame(pThis->m_pFormatCtx, &packet)>=0)
		{
			if(packet.stream_index==pThis->m_nVStreamIndex)
			{
				pThis->packet_queue_put(pThis->m_pVideoPacketQueue, &packet);
			}
			else if(packet.stream_index ==pThis->m_nAStreamIndex)
			{
				pThis->packet_queue_put(pThis->m_pAudioPacketQueue, &packet);
			}
		}else
		{
			nReadEroNum++;
		}
		if(nReadEroNum >= 3)
		{
			pThis->m_bReadFrameEnd = true;///读取完毕文件
			return;
		}
	}
}

void ITAVStreamParse::packet_queue_init(PacketQueue *q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = new ITMutex();	
}
void ITAVStreamParse::packet_queue_clear(PacketQueue *q)
{	
	ITAutoLock lock(q->mutex);
	AVPacketList *pkt, *pkt1;
	for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1)
	{
		pkt1 = pkt->next;
		av_free_packet(&pkt->pkt);
		av_freep(&pkt);
	}
	q->last_pkt = NULL;
	q->first_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;	
}
bool ITAVStreamParse::packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
	/* duplicate the packet */	///检测pkt包完整性
	if ( av_dup_packet(pkt) < 0)
		return false;
	ITAutoLock lock(q->mutex);	
	AVPacketList *pkt1;
	pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pkt1)
		return false;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;	
	if (!q->last_pkt)
		q->first_pkt = pkt1;
	else
		q->last_pkt->next = pkt1;
	q->last_pkt = pkt1;
	q->size += pkt->size;
	q->nb_packets++;
	return true;
}
//************************************
//函数名:  packet_queue_get
//描述：从队列中获取数据
//参数：队列指针，数据包
//注意：pkt释放由调用者释放
//返回值：bool
//时间：2016/1/12 WZQ
//************************************
bool ITAVStreamParse::packet_queue_get(PacketQueue *q, AVPacket *pkt)
{
	if(!q)
		return false;
	if(q->nb_packets <= 0)
		return false;
	ITAutoLock lock(q->mutex);	
	AVPacketList *pkt1;
	for (;;) {
		pkt1 = q->first_pkt;
		if (pkt1)
		{
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			q->size -= pkt1->pkt.size;
			*pkt = pkt1->pkt;		
			av_free(pkt1);			
			break;
		} 
	}	
	return true;
}

//************************************
//函数名:  getNextNalData
//描述：获取下一个NAL数据
//参数：当前数据pts,下一个NAL数据指针，长度
//返回值：bool
//时间：2016/1/11 WZQ
//注意：pts记录的是当前数据pts，以AVCodecCtext->timeBase做时间基
//************************************
bool ITAVStreamParse::getNextNalData(int64_t& pts,unsigned char** strData,int& nLen)
{
	/*if(m_bFirstGetNal)
	{
		unsigned char* pTempData = new unsigned char[m_pVCodecCtx->extradata_size];
		memset(pTempData,0,m_pVCodecCtx->extradata_size);	
		memmove(pTempData,m_pVCodecCtx->extradata,m_pVCodecCtx->extradata_size);		
		*strData = pTempData;		
		nLen = m_pVCodecCtx->extradata_size;
		m_bFirstGetNal = false;
		return true;
	}*/
	AVPacket pPacket = { 0 };
	if(!packet_queue_get(m_pVideoPacketQueue,&pPacket) && m_bReadFrameEnd)
	{		
		return false;		
	}
	if(pPacket.size > 0)
	{
		av_bitstream_filter_filter(m_pVBitStreamFilterCtx, m_pVCodecCtx, NULL, &pPacket.data, &pPacket.size, pPacket.data, pPacket.size, 0);
		unsigned char* pTempData = new unsigned char[pPacket.size];
		memset(pTempData,0,pPacket.size);
	//	memmove(pTempData,"\x0\x0\x0\x1",4);
	//	memmove(pTempData+4,pPacket.data+4,pPacket.size-4);		
		memmove(pTempData,pPacket.data,pPacket.size);	
		*strData = pTempData;		
		nLen = pPacket.size;
		if (pPacket.pts != AV_NOPTS_VALUE)
		{					
			pts = av_rescale_q(pPacket.pts,  m_pFormatCtx->streams[m_nVStreamIndex]->time_base,m_pVCodecCtx->time_base);		
		}else
			pts=0;
		return true;
	}
	return false;
}
//************************************
//函数名:  getNextAudioData
//描述：获取下一帧音频数据
//参数：返回音频的地址，nLen数据大小
//返回值：bool
//时间：2016/1/12 WZQ
//************************************
bool ITAVStreamParse::getNextAudioData(int64_t& pts,unsigned char** pData,int& nLen)
{
	AVPacket pPacket = { 0 };
	if(!packet_queue_get(m_pAudioPacketQueue,&pPacket) && m_bReadFrameEnd)
	{		
		return false;		
	}
	if(pPacket.size > 0)
	{
		if(m_pACodecCtx->extradata)
		{		
			unsigned char* pTempData = new unsigned char[pPacket.size+8];
			memset(pTempData,0,pPacket.size+8);
	
			///添加ADTS头，否则文件无法播放
			char bits[7] = {0};
			int sample_index = 0 , channel = 0;
			char temp = 0;
			int length = 7 + pPacket.size;
		//	sample_index = (m_pACodecCtx->extradata[0] & 0x07) << 1;
			temp = (m_pACodecCtx->extradata[1]&0x80);
			switch(m_pACodecCtx->sample_rate)
			{
			case 96000:
				{
					sample_index = 0x0;
				}break;
			case 88200:
				{
					sample_index = 0x1;
				}break;
			case 64000:
				{
					sample_index = 0x2;
				}break;
			case 48000:
				{
					sample_index = 0x3;
				}break;
			case 44100:
				{
					sample_index = 0x4;
				}break;
			case 32000:
				{
					sample_index = 0x5;
				}break;
			case 24000:
				{
					sample_index = 0x6;
				}break;
			case 22050:
				{
					sample_index = 0x7;
				}break;
			case 16000:
				{
					sample_index = 0x8;
				}break;
			case 12000:
				{
					sample_index = 0x9;
				}break;
			case 11025:
				{
					sample_index = 0xA;
				}break;
			case 8000:
				{
					sample_index = 0xB;
				}break;
			case 7350:
				{
					sample_index = 0xC;
				}break;		
			default:
				{
					sample_index = sample_index + (temp>>7);
				}break;
			}
			channel = ((m_pACodecCtx->extradata[1] - temp) & 0xff) >> 3;
			bits[0] = 0xff;	//1111 1111
			bits[1] = 0xf1;	//1111 0001
			bits[2] = 0x40 | (sample_index<<2) | (channel>>2);
			bits[3] = ((channel&0x3)<<6) | (length >>11);
			bits[4] = (length>>3) & 0xff;
			bits[5] = ((length<<5) & 0xff) | 0x1f;
			bits[6] = 0xfc;
			memmove(pTempData,bits,7);
			memmove(pTempData+7,pPacket.data,pPacket.size);
			*pData = pTempData;	
			nLen = pPacket.size+7;			
		}else
		{
			unsigned char* pTempData = new unsigned char[pPacket.size];
			memset(pTempData,0,pPacket.size);
			memmove(pTempData,pPacket.data,pPacket.size);
			*pData = pTempData;	
			nLen = pPacket.size;
		}
		AVRational tb; 	
		tb.num = 1;
		tb.den = m_nAudioSampleRate;
		int64_t audioPts = av_rescale_q(pPacket.pts, m_pFormatCtx->streams[m_nAStreamIndex]->time_base, tb);
		if (audioPts != AV_NOPTS_VALUE)
			pts = audioPts;
		else
			pts=0;
		return true;
	}
	return false;
}

//************************************
//函数名:  closeDstFile
//描述：关闭目标文件
//参数：无
//返回值：bool
//时间：2016/1/11 WZQ
//************************************
bool ITAVStreamParse::closeDstAVFile()
{
	m_bCloseReadThread=true;
	if(m_pReadThread)
	{
		m_pReadThread->Join();
		delete m_pReadThread;
		m_pReadThread=NULL;

	}
	if(m_pH264SPS)
	{
		delete[] m_pH264SPS;
		m_pH264SPS = NULL;
		m_nH264SPSLen = 0;
	}
	if(m_pH264PPS)
	{
		delete[] m_pH264PPS;
		m_pH264PPS = NULL;
		m_nH264PPSLen = 0;
	}
	m_nTotalSecond = 0;
	m_nFileFrameCount = 0;
	m_nCurReadFrameIndex = 0;
	m_nCurReadFrameTime = 0;
	
	m_nAStreamIndex = -1;
	m_nVStreamIndex = -1;
	m_bSeekFromPts = true;
	m_nFrameRate = 25;
	if(m_strDstAVFile.empty())
		return false;
	m_strDstAVFile = "";
	///关闭转换
	if(m_pAConvertCtx)
	{
		swr_close(m_pAConvertCtx);
		swr_free(&m_pAConvertCtx);
		m_pAConvertCtx=NULL;
	}
	if(m_pVConvertCtx)
	{
		sws_freeContext(m_pVConvertCtx);
		m_pVConvertCtx=NULL;
	}
	if(m_pFormatCtx)
	{
		avcodec_close(m_pACodecCtx);
		avcodec_close(m_pVCodecCtx);
		avformat_close_input(&m_pFormatCtx);  
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
	}			
	m_pVCodecCtx = NULL;
	m_pVCodec = NULL;
	av_bitstream_filter_close(m_pVBitStreamFilterCtx);     
	m_pVBitStreamFilterCtx = NULL;
	av_bitstream_filter_close(m_pABitStreamFilterCtx);
	m_pABitStreamFilterCtx = NULL;
	if(m_pVideoPacketQueue)
	{
		packet_queue_clear(m_pVideoPacketQueue);
		/*delete m_pVideoPacketQueue;
		m_pVideoPacketQueue = NULL;*/
	}
	if(m_pAudioPacketQueue)
	{
		packet_queue_clear(m_pAudioPacketQueue);
		/*delete m_pAudioPacketQueue;
		m_pAudioPacketQueue = NULL;*/
	}
	if(m_pVExtradata)
	{
		delete [] m_pVExtradata;
		m_pVExtradata = NULL;
	}
	m_nVExtradataLen = 0;
	if(m_pAudioConvertBuf)
	{
		av_free(m_pAudioConvertBuf);
		m_pAudioConvertBuf=NULL;
		m_nAudioConvertBufSize=0;
	}
	m_pAudioStream=NULL;
	m_pVideoStream=NULL;
	return true;
}
bool ITAVStreamParse::getNextYuvData(int64_t& pts,unsigned char** pData,int& nLen,int& nWidth,int& nHeight,int& nSynVal)
{
	AVPacket vPacket = { 0 };
	if(!packet_queue_get(m_pVideoPacketQueue,&vPacket) && m_bReadFrameEnd)
	{		
		return false;		
	}
	if(vPacket.size > 0)
	{
		AVFrame* pVFrame = av_frame_alloc();	
		AVFrame* pFrameYUV=av_frame_alloc();
		int got_picture=0;
		int ret = avcodec_decode_video2( m_pVCodecCtx, pVFrame,&got_picture, &vPacket);			
		if ( ret >= 0 && got_picture )
		{				
			if(!m_pVConvertCtx)
			{
				m_pVConvertCtx = sws_getContext(m_pVCodecCtx->width, m_pVCodecCtx->height, m_pVCodecCtx->pix_fmt, \
					m_pVCodecCtx->width, m_pVCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
			}			
			if(!m_pVideoConvertBuf)
			{
				m_pVideoConvertBuf=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, m_pVCodecCtx->width, m_pVCodecCtx->height));
			}
			avpicture_fill((AVPicture *)pFrameYUV,m_pVideoConvertBuf,AV_PIX_FMT_YUV420P,m_pVCodecCtx->width, m_pVCodecCtx->height);
			if(m_pVConvertCtx)
			{
				sws_scale(m_pVConvertCtx, (const uint8_t* const*)pVFrame->data, pVFrame->linesize, 0, m_pVCodecCtx->height, 
					pFrameYUV->data, pFrameYUV->linesize);		
				pVFrame->pts = av_frame_get_best_effort_timestamp(pVFrame);	
				///直接使用pVCodecCtx会有误差，感觉像是打包pts出的问题？  使用下面这个基本就可以了
				AVRational frame_rate = av_guess_frame_rate(m_pFormatCtx, m_pVideoStream, NULL);//m_pVCodecCtx->framerate;
				if(frame_rate.num<1)
					frame_rate.num=25;
				if(frame_rate.den<1)
					frame_rate.den=1;
					
				AVRational tb =m_pVideoStream->time_base;//m_pVCodecCtx->time_base;				
				AVRational framerator;
				framerator.num = frame_rate.den;
				framerator.den = frame_rate.num;
				//此帧持续时间
				double duration = (frame_rate.num && frame_rate.den ? av_q2d(framerator) : 0);		
				m_dVideoClock = (pVFrame->pts == AV_NOPTS_VALUE) ? NAN : pVFrame->pts * av_q2d(tb);
				///计算sleep时间
				double delay = duration;		
				double diff = m_dVideoClock - m_dAudioClock;	
				double max_frame_duration = 3600.0;
				double sync_threshold = FFMAX(0.04, FFMIN(0.1, delay));
				if (!isnan(diff) && fabs(diff) < max_frame_duration)
				{
					if (diff <= -sync_threshold)	//判断音频和视频差值 <= 视频慢了 需要快速播放 
						delay = FFMAX(0, delay + diff);	//取最大值
					else if (diff >= sync_threshold && delay > 0.1)	//视频快了 需要快速播放
						delay = delay + diff;
					else if (diff >= sync_threshold)	
						delay = 2 * delay;
				}
				delay = delay*1000;
				if(delay > 0 && delay < 1000000)
					nSynVal=delay;
				else
					nSynVal=10;

				nWidth = m_pVCodecCtx->width;
				nHeight= m_pVCodecCtx->height;
				int nYsize=nWidth*nHeight;
				unsigned char* pTempData=new unsigned char[nYsize*3/2+1];
				memset(pTempData,0,nWidth*nHeight*3/2+1);
				memmove(pTempData,(unsigned char*)pFrameYUV->data[0],nYsize);
				memmove(pTempData+nYsize,(unsigned char*)pFrameYUV->data[1],nYsize/4);
				memmove(pTempData+nYsize+nYsize/4,(unsigned char*)pFrameYUV->data[2],nYsize/4);
				*pData=pTempData;
				nLen=nYsize*3/2;				
				//AVRational tb; 	
				//tb.num = 1;
				//tb.den = pAFrame->sample_rate;
				//if(m_pACodecCtx->time_base.den < 1)
				//	m_pACodecCtx->time_base.den  = tb.den;
				//if (pAFrame->pts != AV_NOPTS_VALUE)	//判断是否成功解码获取了Pts，如果获取到了，重新以avcodecctx基准和当前帧率做的rationa获取pts
				//	pAFrame->pts = av_rescale_q(pAFrame->pts, m_pACodecCtx->time_base, tb);
				//else if (pAFrame->pkt_pts != AV_NOPTS_VALUE)	//如果解码帧没有，判断包中是否携带，如果携带，以包的pts和流的基准重新获取pts
				//	pAFrame->pts = av_rescale_q(pAFrame->pkt_pts, m_pACodecCtx->time_base, tb);

				//pts = pAFrame->pts;
				//if (pAFrame->pts != AV_NOPTS_VALUE)
				//	m_dAudioClock = pAFrame->pts * av_q2d(tb) + (double) pAFrame->nb_samples / pAFrame->sample_rate;
				//else
				//	m_dAudioClock = NAN;	

			}					
		}
		av_frame_free(&pVFrame);
		av_free_packet(&vPacket);
	}else
	{
		return false;
	}
	return true;
}
bool ITAVStreamParse::getNextPcmData(int64_t& pts,unsigned char** pData,int& nLen)
{
	AVPacket aPacket = { 0 };
	if(!packet_queue_get(m_pAudioPacketQueue,&aPacket) && m_bReadFrameEnd)
	{		
		return false;		
	}
	if(aPacket.size > 0)
	{
		AVFrame* pAFrame = av_frame_alloc();	
		int got_picture=0;
		int ret = avcodec_decode_audio4( m_pACodecCtx, pAFrame,&got_picture, &aPacket);			
		if ( ret > 0 )
		{				
			if(!m_pAConvertCtx)
			{
				m_pAConvertCtx = swr_alloc();
				int64_t out_channel_layout = av_get_default_channel_layout(m_nAudioChannels);
				AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;	
				int64_t in_channel_layout=av_get_default_channel_layout(m_nAudioChannels);///通道原样输出，最佳为根据硬件选择 
				m_pAConvertCtx=swr_alloc_set_opts(m_pAConvertCtx,out_channel_layout, out_sample_fmt, 44100,
					in_channel_layout,m_pACodecCtx->sample_fmt , m_pACodecCtx->sample_rate,0, NULL);
				swr_init(m_pAConvertCtx);	
			}
			if(!m_pAudioConvertBuf)
			{
				AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;	///AAC一般采样位1024
				int dst_nb_samples=1024;
				if(pAFrame->nb_samples > 0 ||  pAFrame->sample_rate>0)
				{					
					dst_nb_samples = av_rescale_rnd(pAFrame->nb_samples, 44100, pAFrame->sample_rate, AV_ROUND_UP);
					m_nAudioConvertBufSize=av_samples_get_buffer_size(NULL,m_nAudioChannels ,dst_nb_samples,out_sample_fmt, 1);	
					m_pAudioConvertBuf=(uint8_t *)av_malloc(m_nAudioConvertBufSize);				
					memset(m_pAudioConvertBuf,0,m_nAudioConvertBufSize);
				}
			}
			if(m_pAConvertCtx)
			{
				swr_convert(m_pAConvertCtx,&m_pAudioConvertBuf,m_nAudioConvertBufSize,(const uint8_t **)pAFrame->data , pAFrame->nb_samples);	
				unsigned char* pTempData=new unsigned char[m_nAudioConvertBufSize];
				memset(pTempData,0,m_nAudioConvertBufSize);
				memmove(pTempData,m_pAudioConvertBuf,m_nAudioConvertBufSize);
				*pData=pTempData;
				nLen=m_nAudioConvertBufSize;

				AVRational tb; 	
				tb.num = 1;
				tb.den = pAFrame->sample_rate;
				if(m_pACodecCtx->time_base.den < 1)
					m_pACodecCtx->time_base.den  = tb.den;
				if (pAFrame->pts != AV_NOPTS_VALUE)	//判断是否成功解码获取了Pts，如果获取到了，重新以avcodecctx基准和当前帧率做的rationa获取pts
					pAFrame->pts = av_rescale_q(pAFrame->pts, m_pACodecCtx->time_base, tb);
				else if (pAFrame->pkt_pts != AV_NOPTS_VALUE && m_pAudioStream)	//如果解码帧没有，判断包中是否携带，如果携带，以包的pts和流的基准重新获取pts
					pAFrame->pts = av_rescale_q(pAFrame->pkt_pts, m_pAudioStream->time_base, tb);

				pts = pAFrame->pts;
				if (pAFrame->pts != AV_NOPTS_VALUE)
					m_dAudioClock = pAFrame->pts * av_q2d(tb) + (double) pAFrame->nb_samples / pAFrame->sample_rate;
				else
					m_dAudioClock = NAN;				
			}					
		}
		av_frame_free(&pAFrame);
		av_free_packet(&aPacket);
	}else
	{
		return false;
	}
	return true;
}

bool ITAVStreamParse::isAudioBuffEmpty()
{
	m_pAudioPacketQueue->nb_packets <= 0 ? m_bAudioBuffEmpty=true : m_bAudioBuffEmpty=false;		
	return m_bAudioBuffEmpty;
}
bool ITAVStreamParse::isVideoBuffEmpty()
{
	m_pVideoPacketQueue->nb_packets <= 0 ? m_bVideoBuffEmpty=true : m_bVideoBuffEmpty=false;		
	return m_bVideoBuffEmpty;
}
//************************************
//函数名:  getAudioBuffSize
//描述：获取每帧音频数据大小
//参数：无
//返回值：int
//时间：2016/4/25 WZQ
//************************************
int ITAVStreamParse::getAudioBuffSize()
{
	int nAudioBuffSize=0;
	if(!m_pACodecCtx)
		return 0;
	AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;	///AAC一般采样位1024
	int dst_nb_samples=1024;								
	dst_nb_samples = av_rescale_rnd(m_pACodecCtx->frame_size, 44100,m_nAudioSampleRate, AV_ROUND_UP);
	nAudioBuffSize=av_samples_get_buffer_size(NULL,m_nAudioChannels ,dst_nb_samples,out_sample_fmt, 1);	
	return nAudioBuffSize;
}