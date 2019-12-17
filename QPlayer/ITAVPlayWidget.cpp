#include "stdafx.h"
#include "ITAVPlayWidget.h"
#include <QtConcurrent>
#include "Global_Fun.h"

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
ITAVPlayWidget::ITAVPlayWidget(QWidget *parent)
	: QGLWidget(parent)
{
	ui.setupUi(this);

	m_pDevice=NULL;
	m_pContext=NULL;
	m_pAVStreamPares = new ITAVStreamParse;
	m_bStopPlay=true;
	m_pAudioBuffer=NULL;
	m_pAudioData=NULL;
	m_nAudioDataSize=0;
	m_outSourceID=0;
	m_outBuffID=0;
	m_nVideoWidth=0;
	m_nVideoHeight=0;
	m_defaultImage.load(QString(":/MOSSOCX/Resources/novideo.png"));
	m_bDefaultImageShow = false;
	m_bPreview=false;

	m_pVideoPlane[0]=NULL;
	m_pVideoPlane[1]=NULL;
	m_pVideoPlane[2]=NULL;
	m_nVideoHeight=0;
	m_nVideoWidth=0;
	m_pVideoData=NULL;
	connect(this,SIGNAL(audioPlayFini()),this,SLOT(doAudioPlayFini()));
	this->setAcceptDrops(true);
}

ITAVPlayWidget::~ITAVPlayWidget()
{
	closeLastPlay();
}
int ITAVPlayWidget::initAudio()
{
	m_pDevice=alcOpenDevice(NULL);	//��Ĭ����Ƶ��	
	if(m_pDevice == NULL)
	{
		printf("open Default Device Ero");
		return -1;
	}
	m_pContext=alcCreateContext(m_pDevice,NULL);
	if(m_pContext==NULL)
	{
		alcCloseDevice(m_pDevice);
		m_pDevice=NULL;
	}
	alcMakeContextCurrent(m_pContext);		//����ж�����Ż���ʱ�������������ǰ�Ĳ��Ż�����
	alGenSources(1,&m_outSourceID);		//������ƵԴ �൱����һ���ռ��з�һ����ƵԴ�����ڽ������Ż���
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
	alSourcef(m_outSourceID, AL_PITCH, 1.0f);
	alSourcef(m_outSourceID, AL_GAIN, 1.0f);
	alSourcei(m_outSourceID, AL_LOOPING, AL_FALSE);
	alSourcef(m_outSourceID, AL_SOURCE_TYPE, AL_STREAMING);

	m_pAudioData=new unsigned char[m_nAudioDataSize+1];
	memset(m_pAudioData,0,m_nAudioDataSize+1);

#ifdef USE_BUFFERS
	alGenBuffers(AUDIO_BUFFS_NUM, m_uiBuffers);	

	///first Fill Audio Buff
	for (int iLoop = 0; iLoop < AUDIO_BUFFS_NUM; iLoop++)
	{				
		unsigned char* pBuf=NULL;
		int nLen=0;
		int64_t pps=0;
		m_pAVStreamPares->getNextPcmData(pps,&pBuf,nLen);
		if(pBuf && nLen>0)
		{
			memmove(m_pAudioData,pBuf,nLen);			
			alBufferData(m_uiBuffers[iLoop], AL_FORMAT_STEREO16, m_pAudioData, nLen, 44100);
			// Queue Buffer on the Source
			alSourceQueueBuffers(m_outSourceID, 1,&m_uiBuffers[iLoop]);
			delete[] pBuf;
			pBuf=NULL;
			nLen=0;
		}			
	}
	alSourcePlay(m_outSourceID);
#else
	alGenBuffers(1, &m_outBuffID);	
	alSourceQueueBuffers(m_outSourceID, 1, &m_outBuffID);
	Sleep(20);
	m_pAudioBuffer=new MemBuffer<uint8_t>(g_audioBuffSize);	
#endif	
	
}
int ITAVPlayWidget::finiAudio()
{
	stopSound();
	if(m_pAudioBuffer)
	{
		delete m_pAudioBuffer;
		m_pAudioBuffer=NULL;
	}
	if(m_pAudioData)
	{
		delete[] m_pAudioData;
		m_pAudioData=NULL;
	}
	m_nAudioDataSize=0;
	if(m_outSourceID)
	{
		alDeleteSources( 1, &m_outSourceID );
		m_outSourceID=0;
	}
#ifdef USE_BUFFERS
	alDeleteBuffers( AUDIO_BUFFS_NUM, m_uiBuffers );
#else
	if(m_outBuffID)
	{
		alDeleteBuffers( 1, &m_outBuffID );
		m_outBuffID=0;
	}
#endif	
	if (m_pContext != NULL) 
	{ 
		alcDestroyContext(m_pContext); 
		m_pContext=NULL; 
	} 
	if (m_pDevice != NULL) 
	{ 
		alcCloseDevice(m_pDevice); 
		m_pDevice=NULL; 
	} 
	return true;
}
bool ITAVPlayWidget::updateAudioBuff(int nSampleFmt,int nBitRate,unsigned char* pData,int nLen)
{
	if(!pData || nLen <= 0)
	{
		return false;
	}
	/*if(m_audioFormat != nSampleFmt || m_audioBitRate != nBitRate )
	{
		m_audioFormat = nSampleFmt;
		m_audioBitRate = nBitRate;
		///��ջ��������������
	}*/
	ALenum format=AL_FORMAT_STEREO16;	///�����ϰ�����ĸ�ʽ���в��ţ��ٴ�Ĭ����Ϊ��16����

	ALuint  buffer=0; 
	alSourceUnqueueBuffers(m_outSourceID, 1, &buffer);	//������ŵ�����		
	//��ȡ�Ѿ�������Ļ�������������µĽ�ȥ���������½��������һֱѭ��һ��  
	alBufferData(buffer, format, pData, (ALsizei)nLen,44100);
	alSourceQueueBuffers(m_outSourceID, 1, &buffer);		
	return true;
}
bool ITAVPlayWidget::playSound()
{
	ALint  state,iQueuedBuffers; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	if (state != AL_PLAYING) 
	{ 
		alGetSourcei(m_outSourceID, AL_BUFFERS_QUEUED, &iQueuedBuffers);
		if (iQueuedBuffers)
		{
			alSourcePlay(m_outSourceID);
		}		
	}    
	return true;
}
bool ITAVPlayWidget::stopSound()
{
	ALint  state; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	if (state != AL_STOPPED) 
	{ 
		alSourceStop(m_outSourceID); 
	} 
	return true;
}
bool ITAVPlayWidget::pauseSound()
{
	ALint  state; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	if (state != AL_PAUSED) 
	{ 
		alSourcePause(m_outSourceID); 
	} 
	return true;
}
bool ITAVPlayWidget::isPlaySounding()
{
	ALint  state; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	return state == AL_PLAYING ? true:false;	
}
bool ITAVPlayWidget::isAudioQueueEmpty()
{
	ALint iQueuedBuffers=0;
	alGetSourcei(m_outSourceID, AL_BUFFERS_QUEUED, &iQueuedBuffers);
	return iQueuedBuffers?false:true;
}
void ITAVPlayWidget::contextMenuEvent(QContextMenuEvent *event)    
{
	QMenu menu;
	menu.addAction("��", this, SLOT(doOpenAVFile()));
	menu.addAction("ֹͣ", this, SLOT(doStopPlay()));
	menu.exec(QCursor::pos());
}
bool ITAVPlayWidget::closeLastPlay()
{
	m_bStopPlay=true;
	m_videoThread.waitForFinished();
	m_audioThread.waitForFinished();
	m_pAVStreamPares->closeDstAVFile();
	finiAudio();
	finiVideo();
	update();	
	return true;
}
void ITAVPlayWidget::doOpenAVFile()
{
	closeLastPlay();
	QString filePath = QFileDialog::getOpenFileName(NULL,"���ļ�","./","*");//��һ���ǻ����Ǹ����ڣ��ڶ���������������Ϣ����������·�������ĸ����˵�������ʽ
	if(filePath.isEmpty())
	{
		return;
	}	
	openDstAVFile(filePath);
}
void ITAVPlayWidget::playAudioThread(ITAVPlayWidget* pThis)
{
	while(!pThis->m_bStopPlay)
	{		
#ifdef USE_BUFFERS
		ALint	iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;
		alGetSourcei(pThis->m_outSourceID, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
		while (iBuffersProcessed)
		{
			ALuint uiBuffer = 0;
			alSourceUnqueueBuffers(pThis->m_outSourceID, 1, &uiBuffer);
			if(!pThis->m_pAVStreamPares->isAudioBuffEmpty())
			{				
				unsigned char* pBuf=NULL;
				int nLen=0;
				int64_t pps=0;
				pThis->m_pAVStreamPares->getNextPcmData(pps,&pBuf,nLen);
				if(pBuf && nLen>0)
				{
					memmove(pThis->m_pAudioData,pBuf,nLen);					
					alBufferData(uiBuffer, AL_FORMAT_STEREO16, pThis->m_pAudioData, nLen, 44100);
					// Queue Buffer on the Source
					alSourceQueueBuffers(pThis->m_outSourceID, 1, &uiBuffer);
					delete[] pBuf;
					pBuf=NULL;
					nLen=0;
				}				
			}
			iBuffersProcessed--;
		}
		ALint iState;
		alGetSourcei(pThis->m_outSourceID, AL_SOURCE_STATE, &iState);
		if (iState != AL_PLAYING)
		{		
			alGetSourcei(pThis->m_outSourceID, AL_BUFFERS_QUEUED, &iQueuedBuffers);
			if (iQueuedBuffers)
			{
				alSourcePlay(pThis->m_outSourceID);
			}
			else
			{
				// Finished playing
				emit pThis->audioPlayFini();
				break;
			}
		}else
		{
			Sleep(SERVICE_UPDATE_PERIOD);
		}
#else
		int processed=0;
		alGetSourcei(pThis->m_outSourceID, AL_BUFFERS_PROCESSED, &processed); 
		while (processed)		
		{	
			while(!pThis->m_pAVStreamPares->isAudioBuffEmpty())
			{
				if(pThis->m_pAudioBuffer->size() >= pThis->m_nAudioDataSize)
				{					
					int nRealLen = pThis->m_pAudioBuffer->pop_front(pThis->m_pAudioData,pThis->m_nAudioDataSize);
					pThis->updateAudioBuff(16,44100,pThis->m_pAudioData,nRealLen);							
					break;
				}else
				{
					unsigned char* pBuf=NULL;
					int nLen=0;
					int64_t pps=0;
					pThis->m_pAVStreamPares->getNextPcmData(pps,&pBuf,nLen);
					if(pBuf && nLen>0)
					{
						pThis->m_pAudioBuffer->push_back(pBuf,nLen);
						delete[] pBuf;
						pBuf=NULL;
						nLen=0;
					}
				}
			}
			if(pThis->m_pAVStreamPares->isAudioBuffEmpty())
			{
				if(pThis->m_pAudioBuffer->size() < 1)
				{
					pThis->refreshAudioQueue();
					break;
				}
				int nRealLen = pThis->m_pAudioBuffer->pop_front(pThis->m_pAudioData,pThis->m_nAudioDataSize);
				pThis->updateAudioBuff(16,44100,pThis->m_pAudioData,nRealLen);					
				break;
			}			
			--processed;
		}		
		if(!pThis->isPlaySounding())
		{
			if(!pThis->isAudioQueueEmpty())
				pThis->playSound();
			else
			{
				emit pThis->audioPlayFini();
				break;	///��Ƶ�ļ��Ѳ�����ɣ��˳�
			}
		}		
		Sleep(10);		
#endif
	}
}
void ITAVPlayWidget::playVideoThread(ITAVPlayWidget* pThis)
{
	while(!pThis->m_bStopPlay)
	{
		if(pThis->m_pAVStreamPares->isVideoBuffEmpty())
		{
			return;
		}
		int64_t pts;
		unsigned char* pBuf=NULL;
		int nLen=0;
		int nWidth=0;
		int nHeight=0;
		int nSynVal=0;
		pThis->m_pAVStreamPares->getNextYuvData(pts,&pBuf,nLen,nWidth,nHeight,nSynVal);		
		if(pBuf && nLen>0)
		{			
			//unsigned char* buffer_convert=new unsigned char[nWidth*nHeight*3+1];
			//memset(buffer_convert,0,nWidth*nHeight*3+1);
			//CONVERT_YUV420PtoRGB24(pBuf,buffer_convert,nWidth,nHeight);
			//memset(pThis->m_pVideoData,0,nWidth*nHeight*3/2+1);
			memmove(pThis->m_pVideoData,pBuf,nLen);
			delete[] pBuf;
			pBuf=NULL;
			//pThis->displayVideo();//
			pThis->update();
			Sleep(nSynVal);
		}
	}
}
void ITAVPlayWidget::doAudioPlayFini()
{
	//finiAudio();
	///��Ƶͬ����Ƶ��������Ƶ��Ϻ󣬰���ƵҲ�ر� 
	closeLastPlay();
}
void ITAVPlayWidget::doStopPlay()
{
	stopSound();///�Ȱ�������ͣ �������ǿ���
	closeLastPlay();
}
bool ITAVPlayWidget::refreshAudioQueue()
{
	ALuint  uiBuffer= 0;
	alSourceUnqueueBuffers(m_outSourceID, 1, &uiBuffer);
	return true;
}

void ITAVPlayWidget::initializeGL()
{
	initializeOpenGLFunctions(); 	
	InitShaders();
}

void ITAVPlayWidget::resizeGL( int width, int height )
{
	if(0 == height)
		height = 1;//��ֹһ����Ϊ0
	glViewport(0, 0, (GLint)width, (GLint)height);//���õ�ǰ�ӿڣ����������ô��ڵģ�ֻ���������ﱻQt����װ����
	glMatrixMode(GL_PROJECTION);//ѡ��ͶӰ����
	glLoadIdentity();//����ѡ��õ�ͶӰ����
	// gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0);//����͸��ͶӰ����
	//glMatirxMode(GL_MODELVIEW);//����2���������ֵĽ���һ��	
}

void ITAVPlayWidget::paintGL()
{
	//glClear()������������Ƕ�initializeGL()���������õ���ɫ�ͻ�����ȵ�������
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();//���õ�ǰ��ģ�͹۲���󣿲��Ǻ���⣡
	if(!m_bStopPlay)
	{
		displayVideo();
	}else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Clear
		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void ITAVPlayWidget::displayVideo()
{
	//glClear()������������Ƕ�initializeGL()���������õ���ɫ�ͻ�����ȵ�������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Clear
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Y
	//
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_id_y);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoWidth, m_nVideoHeight, 0, GL_RED, GL_UNSIGNED_BYTE, m_pVideoPlane[0]); 

	glUniform1i(m_textureUniformY, 0);    
	//U
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_id_u);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoWidth/2, m_nVideoHeight/2, 0, GL_RED, GL_UNSIGNED_BYTE, m_pVideoPlane[1]);       
	glUniform1i(m_textureUniformU, 1);
	//V
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_id_v);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoWidth/2, m_nVideoHeight/2, 0, GL_RED, GL_UNSIGNED_BYTE, m_pVideoPlane[2]);    
	glUniform1i(m_textureUniformV, 2);   

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// Show
	glFlush();
}


void ITAVPlayWidget::paintEvent( QPaintEvent *ev )
{
	if(m_bDefaultImageShow)
	{
		int nWidth = m_defaultImage.width();
		int nHeight = m_defaultImage.height();
		QImage fixedImage(nWidth,nHeight,QImage::Format_RGB32);
		QPainter painter(&fixedImage);	
		painter.fillRect(fixedImage.rect(),Qt::transparent);
		painter.drawImage(0,0,m_defaultImage);
		painter.end();
		QPainter pat(this);
		pat.drawImage(rect(),fixedImage);
	}
	QGLWidget::paintEvent(ev);
}
int ITAVPlayWidget::finiVideo()
{
	if(m_pVideoData)
	{
		delete[] m_pVideoData;
		m_pVideoData=NULL;
	}
	m_pVideoPlane[0]=NULL;
	m_pVideoPlane[1]=NULL;
	m_pVideoPlane[2]=NULL;
	m_nVideoHeight=0;
	m_nVideoWidth=0;
	m_pVideoData=NULL;
	return 0;
}
//Init Shader
void ITAVPlayWidget::InitShaders()
{
	GLint vertCompiled, fragCompiled, linked;

	GLint v, f;
	const char *vs,*fs;
	//Shader: step1
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	//Get source code
	vs = "attribute vec4 vertexIn;\
		attribute vec2 textureIn;\
		varying vec2 textureOut;\
		void main(void)\
		{\
		gl_Position = vertexIn; \
		textureOut = textureIn;\
		}";
	fs ="varying vec2 textureOut;\
		uniform sampler2D tex_y;\
		uniform sampler2D tex_u;\
		uniform sampler2D tex_v;\
		void main(void)\
		{\
			vec3 yuv;\
			vec3 rgb; \
			yuv.x = texture2D(tex_y, textureOut).r;\
			yuv.y = texture2D(tex_u, textureOut).r - 0.5;\
			yuv.z = texture2D(tex_v, textureOut).r - 0.5;\
			rgb = mat3( 1,       1,         1,\
				0,       -0.39465,  2.03211,\
				1.13983, -0.58060,  0) * yuv;    \
			gl_FragColor = vec4(rgb, 1);\
		}";
	//Shader: step2
	glShaderSource(v, 1, &vs,NULL);
	glShaderSource(f, 1, &fs,NULL);
	//Shader: step3
	glCompileShader(v);
	//Debug
	glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);

	//Program: Step1
	m_program = glCreateProgram(); 
	//Program: Step2
	glAttachShader(m_program,v);
	glAttachShader(m_program,f); 

	glBindAttribLocation(m_program, ATTRIB_VERTEX, "vertexIn");
	glBindAttribLocation(m_program, ATTRIB_TEXTURE, "textureIn");
	//Program: Step3
	glLinkProgram(m_program);
	//Debug
	glGetProgramiv(m_program, GL_LINK_STATUS, &linked);  
	//Program: Step4
	glUseProgram(m_program);


	//Get Uniform Variables Location
	m_textureUniformY = glGetUniformLocation(m_program, "tex_y");
	m_textureUniformU = glGetUniformLocation(m_program, "tex_u");
	m_textureUniformV = glGetUniformLocation(m_program, "tex_v"); 


	static const GLfloat vertexVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f,
	};    

	static const GLfloat textureVertices[] = {
		0.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,
	}; 
	//Set Arrays
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
	//Enable it
	glEnableVertexAttribArray(ATTRIB_VERTEX);    
	glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXTURE);


	//Init Texture
	glGenTextures(1, &m_id_y); 
	glBindTexture(GL_TEXTURE_2D, m_id_y);    
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_id_u);
	glBindTexture(GL_TEXTURE_2D, m_id_u);   
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_id_v); 
	glBindTexture(GL_TEXTURE_2D, m_id_v);    
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void ITAVPlayWidget::dragEnterEvent(QDragEnterEvent *eve)
{
	//if (eve->mimeData()->hasFormat("text/plain"))
		eve->acceptProposedAction();
}
void ITAVPlayWidget::dropEvent(QDropEvent *eve)
{
	QList<QUrl> urls = eve->mimeData()->urls();
	if(urls.isEmpty())
		return;
	closeLastPlay();
	QUrl firstUrl=urls.first();
	QString strFilePath=firstUrl.toLocalFile();
	openDstAVFile(strFilePath);
}
//************************************
//������:  openDstAVFile
//��������ָ������Ƶ�ļ�
//�������ļ�·����
//����ֵ��bool
//ʱ�䣺2016/4/25 WZQ
//************************************
bool ITAVPlayWidget::openDstAVFile(const QString& strFilePath)
{
	ITAVCodecCtxFmt fmt;
	memset(&fmt,0,sizeof(ITAVCodecCtxFmt));
	fmt.nSrcWidth=480;
	fmt.nSrcHeight=272;
	bool bRet=m_pAVStreamPares->openDstAVFile(strFilePath.toStdString().c_str(),&fmt);
	if(!bRet)
		return bRet;
	int64_t pps;
	int nLen=0;
	m_bStopPlay=false;
	if(m_pAVStreamPares->getAudioInitState())
	{
#ifdef USE_BUFFERS
		m_nAudioDataSize=m_pAVStreamPares->getAudioBuffSize();
		if(m_nAudioDataSize<=0)
		{
			m_nAudioDataSize=g_audioBuffSize;//wav��С ����4096
		}
#else 
		m_nAudioDataSize=m_pAVStreamPares->getAudioFrequency();//m_pAVStreamPares->getAudioChannel()*m_pAVStreamPares->getAudioFrequency();
		if(m_nAudioDataSize<=4096)
		{
			m_nAudioDataSize=44100;
		}
#endif
		
		initAudio();	
		playSound();	
	}
	if(m_pAVStreamPares->getVideoInitState())
	{
		m_nVideoWidth=m_pAVStreamPares->getVideoWidth();
		m_nVideoHeight=m_pAVStreamPares->getVideoHeight();
		int ysize=m_nVideoWidth*m_nVideoHeight;
		if(ysize <= 0)
		{
			closeLastPlay();
			return false;
		}
		m_pVideoData=new unsigned char[ysize*3+1];
		memset(m_pVideoData,0,ysize*3+1);		
		m_pVideoPlane[0] = m_pVideoData;
		m_pVideoPlane[1] = m_pVideoPlane[0] + m_nVideoWidth*m_nVideoHeight;
		m_pVideoPlane[2] = m_pVideoPlane[1] + m_nVideoWidth*m_nVideoHeight/4;
		m_bPreview=true;		
		initializeGL();
	}
	if(m_pAVStreamPares->getAudioInitState())
		m_audioThread=QtConcurrent::run(playAudioThread,this);
	if(m_pAVStreamPares->getVideoInitState())	
		m_videoThread=QtConcurrent::run(playVideoThread,this);	
	
	return true;
}