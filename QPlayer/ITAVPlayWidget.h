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
///����Ƶ���ݲ��Ž��� 
///���ʹ��OpenGL��Ⱦ��ʾ��OpenAL��Ƶ����
///��Ƶû�з��࣬��

const int g_audioBuffSize=192000;
class ITAVPlayWidget : public QGLWidget,protected QOpenGLFunctions
{
	Q_OBJECT

public:
	ITAVPlayWidget(QWidget *parent = 0);
	~ITAVPlayWidget();

	int initAudio();
	int finiAudio();
	bool updateAudioBuff(int nSampleFmt,int nBitRate,unsigned char* pData,int nLen);
	bool playSound();
	bool stopSound();
	bool pauseSound();
	bool refreshAudioQueue();
	bool isPlaySounding();
	bool isAudioQueueEmpty();
	bool fillAudioBuff();
	bool closeLastPlay();
protected:
	void contextMenuEvent(QContextMenuEvent *event);
	static void playAudioThread(ITAVPlayWidget* pThis);
	static void playVideoThread(ITAVPlayWidget* pThis);
	void paintGL();
	void resizeGL(int width, int height);
	void initializeGL();
	void displayVideo();
	void paintEvent( QPaintEvent *ev );
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
	ALenum			m_audioFormat;
	ALuint			m_audioBitRate;
	ITAVStreamParse*	m_pAVStreamPares;
	MemBuffer<uint8_t>* m_pAudioBuffer;	//��Ƶ������
	unsigned char*	m_pAudioData;		//��ǰ������Ƶ����
	int				m_nAudioDataSize;	//��ǰ������Ƶ��Ҫ��С
	unsigned char*	m_pVideoData;
	int				m_nVideoWidth;
	int				m_nVideoHeight;

	bool m_bPreview;
	bool m_bDefaultImageShow;
	bool m_bLoadedImage;
	QImage m_defaultImage;
	GLuint m_defaultTexure;
};

#endif // ITAVPLAYWIDGET_H
