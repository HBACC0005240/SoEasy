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
	m_nTotalSecond = 0;
	m_pH264PPS = NULL;
	m_pH264SPS = NULL;
	m_nH264PPSLen = 0;
	m_nH264SPSLen = 0;
	m_nFileFrameCount = 0;
	m_nCurReadFrameIndex = 0;
	m_nCurReadFrameTime = 0;
	m_bSeekFromPts = true;
	m_nFrameRate = 25;	//��ʼ��1��25֡	
	m_bReadFrameEnd = false;
	m_pVideoPacketQueue = new PacketQueue;
	memset(m_pVideoPacketQueue,0,sizeof(PacketQueue));
	m_pAudioPacketQueue = new PacketQueue;
	memset(m_pAudioPacketQueue,0,sizeof(PacketQueue));
	///�ļ��洢��һ����Ƶ һ����Ƶ����ȡʱ�����5��6����Ƶ���󣬲ſ�ʼ����Ƶ��
	///����һ���Դ���10��sleep�������ݱ�ȡ���ټ���
	///�ֽڲ��֣���ʱ�����ǣ���Ϊ�е�h264�ɱ�֡���ݲ��ÿ���
	m_nDstVideoQueuePktBuff = 50;	//��ʼ�����Ի���50��NAL������
	m_nDstVideoQueueByteBuff = 0;
	m_nDstAudioQueuePktBuff = 50;	//��ʼ�����Ի���50����Ƶ��
	m_nDstAudioQueueByteBuff = 0;
	m_bCloseReadThread = false;
	m_pReadThread = NULL;
	m_nAudioChannels=2;	///��Ƶͨ��
	m_nAudioSampleRate=44100;	///��Ƶ������
	m_nAudioBitRate=44100;
	//	m_nAudioSamples=1024;	///��Ƶ������
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
	m_pVideoConvertBuf=NULL;
	m_nVideoConvertBufSize=0;
}


ITAVStreamParse::~ITAVStreamParse(void)
{
}

bool ITAVStreamParse::openDstAVFile(const char* strPath)
{
	m_bCloseReadThread=false;
	m_pAudioConvertBuf=false;
	//if((access(strPath,0))==-1)	//�ļ������� ��Щ��ⲻ����
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
	if (err < 0) {
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
	if(m_nVStreamIndex == -1 && m_nAStreamIndex==-1)	//2����û�ҵ�  �˳�����Ļ�Ȳ�����
	{
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
		return false;
	}
	if(m_nVStreamIndex != -1)
		m_pVCodecCtx=m_pFormatCtx->streams[m_nVStreamIndex]->codec;
	if(m_nAStreamIndex != -1)
		m_pACodecCtx =m_pFormatCtx->streams[m_nAStreamIndex]->codec;
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
			m_pVConvertCtx = sws_getContext(m_pVCodecCtx->width, m_pVCodecCtx->height, m_pVCodecCtx->pix_fmt, 
				m_pVCodecCtx->width, m_pVCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
			m_nVideoWidth=m_pVCodecCtx->width;
			m_nVideoHeight=m_pVCodecCtx->height;
			///������Ƶʱ��
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
			if(duration <= 0)	//��ȡ���������������
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
			m_nTotalSecond = secs;	//������
			mins  = secs / 60;
			secs %= 60;
			hours = mins / 60;
			mins %= 60;	
			printf("%02d:%02d:%02d.%02d", hours, mins, secs,(100 * us) / AV_TIME_BASE);//ʱ����
			int64_t stremdu = m_pFormatCtx->streams[m_nVStreamIndex]->duration/AV_TIME_BASE;
			printf("��ʱ�䣺%ld",m_pFormatCtx->duration* av_q2d(m_pFormatCtx->streams[m_nVStreamIndex]->time_base));	
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
	}		
	int dummy_size;  		
	m_pReadThread = new ITThread("��ȡ�߳�");
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
			Sleep(30);	//sleep 300����	����и����ѵ�һ���̻߳�����
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
			pThis->m_bReadFrameEnd = true;///��ȡ����ļ�
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
	/* duplicate the packet */	///���pkt��������
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
//������:  packet_queue_get
//�������Ӷ����л�ȡ����
//����������ָ�룬���ݰ�
//ע�⣺pkt�ͷ��ɵ������ͷ�
//����ֵ��bool
//ʱ�䣺2016/1/12 WZQ
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
//������:  getNextNalData
//��������ȡ��һ��NAL����
//��������ǰ����pts,��һ��NAL����ָ�룬����
//����ֵ��bool
//ʱ�䣺2016/1/11 WZQ
//ע�⣺pts��¼���ǵ�ǰ����pts����AVCodecCtext->timeBase��ʱ���
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
//������:  getNextAudioData
//��������ȡ��һ֡��Ƶ����
//������������Ƶ�ĵ�ַ��nLen���ݴ�С
//����ֵ��bool
//ʱ�䣺2016/1/12 WZQ
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
	
			///���ADTSͷ�������ļ��޷�����
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
//������:  closeDstFile
//�������ر�Ŀ���ļ�
//��������
//����ֵ��bool
//ʱ�䣺2016/1/11 WZQ
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
	///�ر�ת��
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
	return true;
}
bool ITAVStreamParse::getNextYuvData(int64_t& pts,unsigned char** pData,int& nLen,int& nWidth,int& nHeight)
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
				AVRational frame_rate = m_pVCodecCtx->framerate;
				AVRational tb = m_pVCodecCtx->time_base;				
				AVRational framerator;
				framerator.num = frame_rate.den;
				framerator.den = frame_rate.num;
				//��֡����ʱ��
				double duration = (frame_rate.num && frame_rate.den ? av_q2d(framerator) : 0);		
				double pts = (pVFrame->pts == AV_NOPTS_VALUE) ? NAN : pVFrame->pts * av_q2d(tb);	
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
				//if (pAFrame->pts != AV_NOPTS_VALUE)	//�ж��Ƿ�ɹ������ȡ��Pts�������ȡ���ˣ�������avcodecctx��׼�͵�ǰ֡������rationa��ȡpts
				//	pAFrame->pts = av_rescale_q(pAFrame->pts, m_pACodecCtx->time_base, tb);
				//else if (pAFrame->pkt_pts != AV_NOPTS_VALUE)	//�������֡û�У��жϰ����Ƿ�Я�������Я�����԰���pts�����Ļ�׼���»�ȡpts
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
				int64_t in_channel_layout=av_get_default_channel_layout(m_nAudioChannels);///ͨ��ԭ����������Ϊ����Ӳ��ѡ�� 
				m_pAConvertCtx=swr_alloc_set_opts(m_pAConvertCtx,out_channel_layout, out_sample_fmt, 44100,
					in_channel_layout,m_pACodecCtx->sample_fmt , m_pACodecCtx->sample_rate,0, NULL);
				swr_init(m_pAConvertCtx);	
			}
			if(!m_pAudioConvertBuf)
			{
				AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;	///AACһ�����λ1024
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
				if (pAFrame->pts != AV_NOPTS_VALUE)	//�ж��Ƿ�ɹ������ȡ��Pts�������ȡ���ˣ�������avcodecctx��׼�͵�ǰ֡������rationa��ȡpts
					pAFrame->pts = av_rescale_q(pAFrame->pts, m_pACodecCtx->time_base, tb);
				else if (pAFrame->pkt_pts != AV_NOPTS_VALUE)	//�������֡û�У��жϰ����Ƿ�Я�������Я�����԰���pts�����Ļ�׼���»�ȡpts
					pAFrame->pts = av_rescale_q(pAFrame->pkt_pts, m_pACodecCtx->time_base, tb);

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