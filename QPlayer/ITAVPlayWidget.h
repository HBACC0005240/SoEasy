#ifndef ITAVPLAYWIDGET_H
#define ITAVPLAYWIDGET_H

#include <QOpenGLFunctions>
#include <QGLWidget>
#include <QtOpenGL/qgl.h>
#include "Global_Define.h"
#include "al.h"
#include "alc.h"
#include "ITAVStreamParse.h"
#include "ui_ITAVPlayWidget.h"
#include "MemBuffer.h"
///音视频数据播放界面 
///混合使用OpenGL渲染显示，OpenAL音频播放
///音频没有封类，和

const int g_audioBuffSize=192000;
const int AUDIO_BUFFS_NUM=4;
#define USE_BUFFERS	1//是否使用音频缓冲区 音频缓冲区也需要
#define	SERVICE_UPDATE_PERIOD	(20)	///音频sleep时间
class ITAVPlayWidget : public QGLWidget,protected QOpenGLFunctions
{
	Q_OBJECT

public:
	ITAVPlayWidget(QWidget *parent = 0);
	~ITAVPlayWidget();


	bool openDstAVFile(const QString& strFilePath);
	int	 initAudio();
	int  finiAudio();
	bool updateAudioBuff(int nSampleFmt,int nBitRate,unsigned char* pData,int nLen);
	bool playSound();
	bool stopSound();
	bool pauseSound();
	bool refreshAudioQueue();
	bool isPlaySounding();
	bool isAudioQueueEmpty();
	bool fillAudioBuff();
	bool closeLastPlay();
	int  finiVideo();
protected:
	void contextMenuEvent(QContextMenuEvent *event);
	static void playAudioThread(ITAVPlayWidget* pThis);
	static void playVideoThread(ITAVPlayWidget* pThis);
	void paintGL();
	void resizeGL(int width, int height);
	void initializeGL();
	void displayVideo();
	void paintEvent( QPaintEvent *ev );
	void InitShaders();
	void dragEnterEvent(QDragEnterEvent *eve);
	void dropEvent(QDropEvent *eve);
signals:
	void audioPlayFini();
public slots:
	void doOpenAVFile();
	void doAudioPlayFini();
	void doStopPlay();
private:
	Ui::ITAVPlayWidget ui;
	bool			m_bStopPlay;
	///Audio
	ALCdevice*		m_pDevice;
	ALCcontext*		m_pContext;
	ALuint			m_outSourceID;
	ALuint			m_outBuffID;
	ALuint			m_uiBuffers[AUDIO_BUFFS_NUM];
	ALenum			m_audioFormat;
	ALuint			m_audioBitRate;
	ITAVStreamParse*	m_pAVStreamPares;
	MemBuffer<uint8_t>* m_pAudioBuffer;	//音频缓冲区
	unsigned char*	m_pAudioData;		//当前播放音频数据
	int				m_nAudioDataSize;	//当前播放音频需要大小
	
	//video
	unsigned char*	m_pVideoData;
	int				m_nVideoWidth;
	int				m_nVideoHeight;
	unsigned char*	m_pVideoPlane[3];

	GLuint m_textureUniformY;
	GLuint m_textureUniformU;
	GLuint m_textureUniformV;
	GLuint m_id_y;
	GLuint m_id_u;
	GLuint m_id_v; // Texture id
	GLuint m_program;
	bool m_bPreview;
	bool m_bDefaultImageShow;
	bool m_bLoadedImage;
	QImage m_defaultImage;
	GLuint m_defaultTexure;
	QFuture<void> m_videoThread;
	QFuture<void> m_audioThread;
};

#endif // ITAVPLAYWIDGET_H
