#include "stdafx.h"
#include "QDetection.h"
#include "Public_opencv.h"
#include "ITDataMgr.h"
const int numCharacter=34;
const int numAll=65;
const int numChinese=31;

const char strCharacters[] = {'0','1','2','3','4','5',\
	'6','7','8','9','A','B', 'C', 'D', 'E','F', 'G', 'H',\
	'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', \
	'U','V', 'W', 'X', 'Y', 'Z'};
const string strChinese[] = {
	"��","��","��","��","��","��",
	"��","��","��","��","��","��",
	"��","³","��","��","��","��",
	"��","��","��","��","��","��",
	"��","ԥ","��","��","��","��",
	"��" };


QDetection::QDetection(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.frame_video->setAutoFillBackground(true); 
	ui.tableWidget->setColumnCount(2);   
	ui.frame_video->installEventFilter(this);
	QStringList saveHeadList;
	saveHeadList<<"���ƺ�"<<"�鿴ͼƬ";
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->verticalHeader()->setVisible(true);
	connect(ui.pushButton_openPic,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_openVideo,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_play,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_stop,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_pre,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_next,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
}

QDetection::~QDetection()
{

}

void QDetection::doBtnClicked()
{
	QObject* obj=sender();
	if(obj ==ui.pushButton_openPic)
	{
		QString strPicPath=QFileDialog::getOpenFileName(this,"��ͼƬ","./","Images (*.png *.bmp *.jpg)");
		openDstPicFile(strPicPath);
	}else if(obj==ui.pushButton_openVideo)
	{

	}else if(obj==ui.pushButton_play)
	{

	}else if(obj==ui.pushButton_stop)
	{

	}else if(obj==ui.pushButton_pre)
	{

	}else if(obj==ui.pushButton_next)
	{

	}
}
void QDetection::setVideoFrameImage(const QImage& image)
{
	m_curDisImage=image;
	QPalette palette = ui.frame_video->palette();     //ʹ��ƽ�������ŷ�ʽ
	palette.setBrush(QPalette::Window,QBrush(QPixmap::fromImage(image).scaled( ui.frame_video->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
	ui.frame_video->setPalette(palette); // ���� �Ѹ�widget�����˱���ͼ 
}
void QDetection::openDstPicFile(const QString& strFile)
{
	if(strFile.isEmpty())
		return;
	if(!QFile::exists(strFile))
	{
		QMessageBox::information(this,"��ʾ:","ͼƬ�����ڣ�");
		return;
	}
	///����opencv���д�
	string strpath;
	UNICODETOANSI(strFile.toStdWString(),strpath);
	Mat img=imread(strpath);
	if(img.empty())
	{
		QMessageBox::information(this,"��ʾ:","��ȡͼƬ���ݴ���");
		return;
	}
	//imshow("ԭ����",img); //��ʾͼ��
	//waitKey(0);
	QImage pixMap=cvMat2QImage(img);
	QImage srcImage=pixMap;
	setVideoFrameImage(pixMap);	
	//˫���˲�
	Mat lvbo;
	bilateralFilter(img,lvbo,3,300,300,BORDER_DEFAULT);
	pixMap=cvMat2QImage(lvbo);
//	setVideoFrameImage(pixMap);	

	Mat huidu;//Mat frame = Mat(Size(width,height), CV_8UC3);   
	cvtColor(lvbo,huidu,CV_BGR2GRAY);//�Ҷ�ת��
	pixMap=cvMat2QImage(huidu);
//	setVideoFrameImage(pixMap);	

	Mat zhongzhi;
	medianBlur(huidu,zhongzhi,3);
	pixMap=cvMat2QImage(zhongzhi);
//	setVideoFrameImage(pixMap);	

	Mat bianyuan;
	Sobel(zhongzhi,bianyuan,0,1,0,3,1,0,BORDER_DEFAULT);
	pixMap=cvMat2QImage(bianyuan);
//	setVideoFrameImage(pixMap);	//�ٴ���

	Mat gaosi;
	cv::Size s(5,1);
	GaussianBlur(bianyuan,gaosi,s,0,0);
	pixMap=cvMat2QImage(gaosi);
//	setVideoFrameImage(pixMap);

	Mat erzhi;
	threshold( gaosi,erzhi,30,255, CV_THRESH_BINARY|CV_THRESH_OTSU); 
	pixMap=cvMat2QImage(erzhi);
//	setVideoFrameImage(pixMap);

	//��̬ѧ����
	Mat element=getStructuringElement(MORPH_RECT,Size(3,3));//Ԫ�ؽṹ1*3��3*3�ĸ����أ�����
	Mat elementtwo=getStructuringElement(MORPH_RECT,Size(9,3));
	Mat pengzhang;
	Mat fushi;
	erode(erzhi,fushi,element,Point(-1,-1),1);//dilate����
	dilate(fushi,pengzhang,elementtwo,Point(-1,-1),5);//erode��ʴ//����Ӧ���ȸ�ʴ1�� ������ 
	pixMap=cvMat2QImage(pengzhang);
//	setVideoFrameImage(pixMap);

	int thresh = 100;
	int max_thresh = 255;
	RNG rng(12345);                      //vector<Vec4i>hierarchy�Ƕ���Ĳ㼶
	Mat threshold_output;               //contours������ɶ�ά�����������������潫����洢�ҵ��ı߽�ģ�x,y������
	vector<vector<Point> > contours;//vector������ ��������һ���ɱ䳤������ ��һ������һ��point�͵Ķ�ά�������contours��ÿһά���Ⱦ��ɱ䣩
	vector<Vec4i> hierarchy;//����һ��Vec4i�͵�һά����hierarchy <>����д������������������ Vec4iָ�����ĸ�������
	threshold( pengzhang, threshold_output, thresh, 255, THRESH_BINARY );
	pixMap=cvMat2QImage(threshold_output);
//	setVideoFrameImage(pixMap);	
	findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );/// �ҵ�����
	pixMap=cvMat2QImage(threshold_output);
//	setVideoFrameImage(pixMap);

	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );//bounkRect�ǻ�ȡ���γߴ��һ��������һ����4������eg����0��0��344��,567��

	for( int i = 0; i < contours.size(); i++ )
	{ approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
	boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	}
	// ����������� + ��Χ�ľ��ο� ������ʡ�ԣ��������ڼ��ʶ�������
	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );//������ɫ
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );//������
	}
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//����ͼ
	
	//����λ�ô���ȡ
	int area=drawing.rows*drawing.cols;
	//cout<<"ͼ�������"<<area<<endl;
	int j,squ=0,num=(-1);
	int width[3333],height[3333];
	for(j=0;j<contours.size();j++)
	{
		//boundRect[j].tl()
		width[j]=boundRect[j].br().x-boundRect[j].tl().x;
		height[j]=boundRect[j].br().y-boundRect[j].tl().y;
		float abc=width[j]/height[j];
		if(abc>=2.2&&abc<=4)//�߳���ɸѡ�����ɸѡ
		{
			if(width[j]*height[j]>squ)
			{
				if(width[j]*height[j]<=area/12)
				{
					squ=width[j]*height[j];
					num=j;
				}
			}
		}
	}
	if(num<0)//������ֹ���
	{
		cout<<"û�����ջ�δ��ʶ�𣨴�����룺001��"<<endl;//û��ʶ���roi����
		QMessageBox::information(this,"��ʾ��","û�����ջ�δ��ʶ�𣨴�����룺001��");
		return;
	}
	int qix,qiy,chanx,chany;
	qix=boundRect[num].tl().x;//+(kuandu[num]/9);
	qiy=boundRect[num].tl().y;//+(gaodu[num]/9);
	chanx=width[num];//*7/9;
	chany=height[num];//*7/9;
	Mat check=lvbo(Rect(qix,qiy,chanx,chany));
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//���ƴֶ�λ
	//����RGB��Ϣ��ȷ��ȡԤ����
	Mat liuzhe;
	liuzhe=check.clone();
	unsigned char R = 28,G = 63,B = 130;//90<B<125;0<G<40;0<R15...R = 50,G = 75,B = 145
	for (int i = 0; i < check.cols; i++) 
	{
		for (int j = 0; j < check.rows; j++) 
		{
			Vec3b &intensity = check.at<Vec3b>(j, i);
			if(intensity.val[0]-B<40&&intensity.val[1]-G<30&&intensity.val[2]-R<30)// R = 50,G = 75,B = 145����55.35.40
			{
				for(int k = 0; k < check.channels(); k++)
				{// calculate pixValue
					intensity.val[k] =0;
				}
			}else
			{
				for(int k = 0; k < check.channels(); k++)
				{// calculate pixValue
					intensity.val[k] =255;
				}
			}
		}
	}
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//RGB����

	//���ƴ���λ��ͼ��Ԥ����
	Mat hbcp;
	cvtColor(check,hbcp,CV_BGR2GRAY);
	Mat threshold_outputtwo;
	vector<vector<Point> > contoursb;
	vector<Vec4i> hierarchyb;
	// ʹ��Threshold��ֵ������Ե
	threshold( hbcp, threshold_outputtwo, 30, 255, CV_THRESH_BINARY|CV_THRESH_OTSU );
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//���ƶ�ֵ��
	/// �ҵ�����
	findContours(threshold_outputtwo , contoursb, hierarchyb, CV_RETR_TREE ,CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );// CV_RETR_LIST
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//// CV_RETR_EXTERNAL, //��ȡ�����ķ����������ȡ��Χ������  
	//  CV_CHAIN_APPROX_NONE //�������Ƶķ��������ﲻ���ƣ���ȡȫ��������  
	// minAreaRect
	// ��ÿ���ҵ���������������б�ı߽�����Բ
	vector<RotatedRect> minRect( contoursb.size() );

	for( int i = 0; i < contoursb.size(); i++ )
	{ 
		minRect[i] = minAreaRect( Mat(contoursb[i]) );
	}

	// ��������������б�ı߽��ͱ߽���Բ
	Mat drawingb = Mat::zeros( threshold_outputtwo.size(), CV_8UC3 );
	for( int i = 0; i< contoursb.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawingb, contoursb, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		Point2f rect_points[4];
		minRect[i].points( rect_points );
		for( int j = 0; j < 4; j++ )
			line( drawingb, rect_points[j], rect_points[(j+1)%4], color, 2, 8 );
	}
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//Contours

	//��ȷ��ȡ�㷨 ��ɸѡ
	int cpsqu;
	cpsqu=chanx*chany;
	// cout<<"�������"<<cpsqu<<endl;//60822
	int h,squb=999999,numb=(-1);
	float kuandub[3333];
	float gaodub[3333];
	Point2f rect_points[4];
	for(h=0;h<contoursb.size();h++)
	{
		minRect[h].points( rect_points );
		int rep[4],nuk=99999,kk;
		for(int i=0;i<4;i++)
			rep[i]=rect_points[i].x+rect_points[i].y;
		for(int j=0;j<4;j++)
		{
			if(rep[j]<nuk)
			{nuk=rep[j];
			kk=j;}
		}
		if(kk%2==1)
		{
			kuandub[h]=sqrt(pow(rect_points[1].x-rect_points[2].x,2)+pow(rect_points[1].y-rect_points[2].y,2));
			gaodub[h]=sqrt(pow(rect_points[1].x-rect_points[0].x,2)+pow(rect_points[1].y-rect_points[0].y,2));
		}
		else
		{
			gaodub[h]=sqrt(pow(rect_points[1].x-rect_points[2].x,2)+pow(rect_points[1].y-rect_points[2].y,2));
			kuandub[h]=sqrt(pow(rect_points[1].x-rect_points[0].x,2)+pow(rect_points[1].y-rect_points[0].y,2));
		}

		float bizhi=kuandub[h]/gaodub[h];
		if(bizhi>=3&&bizhi<=28/7)//�߳���ɸѡ�����ɸѡ   ***��ע��������ȦͼƬ������ֵ3.332  ��Ȧ3.5��  ��ʵ�ʲ���������
		{
			if(kuandub[h]*gaodub[h]>cpsqu/2&&kuandub[h]*gaodub[h]<squb)
			{
				squb=kuandub[h]*gaodub[h];
				numb=h;
			}
		}
	}

	if(numb<0)//
	{	
		QMessageBox::information(this,"��ʾ��","û�����ջ�δ��ʶ�𣨴�����룺001��");
		return;
	}
	//ƫ�ƽǶȼ���
	double jiaodu;//������б��
	double jiaj;//�Խ��� ��� �н�
	double duiq;//�Խ�����б�Ƕ�
	minRect[numb].points( rect_points );
	int xa=rect_points[1].x;//����������ʼ��
	int ya=rect_points[1].y;
	int xb=rect_points[3].x;//����������ֹ��
	int yb=rect_points[3].y;
	double cha=yb-ya;
	double chb=xb-xa;
	double he=gaodub[numb];//��
	double wi=kuandub[numb];//��
	double ss=cha/chb;//(yb-ya)/(xb-xa);
	jiaj=0;//atan(he/wi)*180/3.1416;
	duiq=0;//atan(ss)*180/3.1416;
	jiaodu=duiq-jiaj;

	//cout<<"�Ƕ�"<<jiaodu<<endl;
	//cout<<"�н�"<<jiaj<<endl;
	//cout<<"�Խ�����б"<<duiq<<endl;

	Point2f centers[4];
	minRect[numb].points(centers);
	Mat rot_mat;
	double scale=1;
	rot_mat= getRotationMatrix2D( centers[1],jiaodu , scale );//minRect[numb].center
	Mat centermat;
	warpAffine(liuzhe,centermat, rot_mat,liuzhe.size());//��rgb����ͼ��ת
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//��ת��

	//��ȡ�Ż�
	int qixb,qiyb,chanxb,chanyb;
	qixb=centers[1].x+(width[num]*2/100);
	qiyb=centers[1].y+(gaodub[numb]*16/100);//gaodu[num]*3/26....//////////////////////////////��//////////////////////////////////////////////////////////
	chanxb=kuandub[numb]*96/100;
	chanyb=gaodub[numb]*72/100;//9/13;//*7/9;
	if(qixb>centermat.cols||qiyb>centermat.rows||chanxb>centermat.cols||chanyb>centermat.rows)
	{
		return;
	}
	Mat youwudi=centermat(Rect(qixb,qiyb,chanxb,chanyb));//hbcp
	Mat huiwudi;
	cvtColor(youwudi,huiwudi,CV_BGR2GRAY);//�Ҷ�ת��
	Mat erwudi;
	threshold( huiwudi,erwudi,30,255, CV_THRESH_BINARY|CV_THRESH_OTSU); 
	pixMap=cvMat2QImage(drawing);
///	setVideoFrameImage(pixMap);//���ƾ���λ

	//�ָ���ַ�
	//cout<<"����ͼ��߶�"<<chanyb<<endl;
	//����1�߶�74  ��ʴԪ��1.6һ��(6*1)
	//����2�߶�32  ��ʴԪ�� ���ԣ�1.3һ�Σ�3*1��
	//chepai5 63  δ���� 6*1
	int suanzi;
	suanzi=chanyb/14;
	//��̬ѧ����
	Mat elementc=getStructuringElement(MORPH_RECT,Size(1,7));
	Mat elementd=getStructuringElement(MORPH_RECT,Size(1,suanzi));
	Mat zxdilate;
	Mat zxerode;
	erode(erwudi,zxerode,elementd,Point(-1,-1),1);//dilate����erode��ʴ
	dilate(zxerode,zxdilate,elementc,Point(-1,-1),9);//zxdilate�������ʹ���ͼ
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//�����ַ�����ͼ

	vector<vector<Point> > contoursc;
	vector<Vec4i> hierarchyc;
	findContours( zxdilate, contoursc, hierarchyc, CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	/// ����αƽ����� + ��ȡ����
	vector<vector<Point> > contours_polyc( contoursc.size() );
	vector<Rect> boundRectc( contoursc.size() );//bounkRect�ǻ�ȡ���γߴ��һ��������һ����4������eg����0��0��344��,567��

	for( int i = 0; i < contoursc.size(); i++ )
	{
		approxPolyDP( Mat(contoursc[i]), contours_polyc[i], 3, true );
		boundRectc[i] = boundingRect( Mat(contours_polyc[i]) );
	}


	/// ����������� + ��Χ�ľ��ο� 
	Mat drawingc = Mat::zeros( zxdilate.size(), CV_8UC3 );
	for( int i = 0; i<contoursc.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );//������ɫ
		//drawContours( drawingb, contours_polyb, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//������
		rectangle( drawingc, boundRectc[i].tl(), boundRectc[i].br(), color, 2, 8, 0 );//������
	}
	pixMap=cvMat2QImage(drawing);
//	setVideoFrameImage(pixMap);//���Ƶ�����ͼ
	
	int m=-1;
	int kuanc[100],gaoc[100];
	int qidianx[10],qidiany[10],zhongdianx[10],zhongdiany[10],juxing[10];//��ʵ������8��9�͹���
	for(int k=0;k<contoursc.size();k++)
	{
		kuanc[k]=boundRectc[k].br().x-boundRectc[k].tl().x;
		gaoc[k]=boundRectc[k].br().y-boundRectc[k].tl().y;
		if(kuanc[k]*gaoc[k]>chanxb*chanyb/60)
		{
			m++;
			qidianx[m]=boundRectc[k].tl().x;
			qidiany[m]=boundRectc[k].tl().y;
			zhongdianx[m]=boundRectc[k].br().x;
			zhongdiany[m]=boundRectc[k].br().y;
			juxing[m]=(boundRectc[k].br().x-boundRectc[k].tl().x)*(boundRectc[k].br().y-boundRectc[k].tl().y);
		}
	}
	m=m+1;
	//cout<<"��ѡ�ַ�����"<<m<<endl;
	if(m<0)
	{
		QMessageBox::information(this,"��ʾ��","û�����ջ�δ��ʶ�𣨴�����룺003��");
		return;
	}

	//ɸѡ���������7��
	int ca,cb,cc[7];//ѭ������
	int k=-1;
	int l=m;
	int wud[7];//���ڳ�װ7���������
	for(ca=0;ca<7;ca++)//ɸѡ����ѡ������ �������7��
	{
		wud[ca]=0;//juxing[0];
		for(cb=0;cb<l;cb++)
		{

			if(wud[ca]<juxing[cb])//&&k!=cb
			{
				wud[ca]=juxing[cb];//342 434 753
				k=cb;//1 2 3
			}
			else k=k;//3
		}
		juxing[k]=0;
		//cout<<"������"<<wud[ca]<<endl;
		//cout<<"����juxing���"<<k<<endl;
		cc[ca]=k;

	}
	if(k<0)
	{
		QMessageBox::information(this,"��ʾ��","û�����ջ�δ��ʶ�𣨴�����룺001��");
		return;
	}
	int munn[7];
	int nb,mb;
	int a[7];
	for(nb=0;nb<7;nb++)
	{
		munn[nb]=100000;//
		for(mb=0;mb<7;mb++)
		{

			if(munn[nb]>qidianx[cc[mb]] )//&&k!=cb
			{
				munn[nb]=qidianx[cc[mb]];//342 434 753
				k=cc[mb];//1 2 3
			}
			else k=k;//3
		}
		qidianx[k]=100000;
		//cout<<"��С����"<<munn[nb]<<endl;//4
		//cout<<"���Ķ�Ӧ���"<<k<<endl;//6
		a[nb]=k;

	}
	//system("pause");
	int aaa[7],bbb[7],ccc[7],ddd[7];
	Mat zifua[7];
	for(int ii=0;ii<7;ii++)
	{
		aaa[ii]=munn[ii];
		bbb[ii]=qidiany[a[ii]];
		ccc[ii]=zhongdianx[a[ii]]-munn[ii];
		ddd[ii]=zhongdiany[a[ii]]-qidiany[a[ii]];
		zifua[ii] =erwudi(Rect(aaa[ii],bbb[ii],ccc[ii],ddd[ii]));
	}//system("pause");

	//namedWindow( "һ���ַ�");
	//imshow( "һ���ַ�",zifua[0] );
	//waitKey(0);

	//namedWindow( "�����ַ�");
	//imshow( "�����ַ�",zifua[1] );
	//waitKey(0);

	//namedWindow( "�����ַ�");
	//imshow( "�����ַ�",zifua[2] );
	//waitKey(0);

	//namedWindow( "�ĺ��ַ�");
	//imshow( "�ĺ��ַ�",zifua[3] );
	////imwrite("E:\\VSproject\\11\\11\\�����ַ�\\4���ַ�.jpg",zifua[3]);
	//waitKey(0);

	//namedWindow( "����ַ�");
	//imshow( "����ַ�",zifua[4] );
	//waitKey(0);

	//namedWindow( "�����ַ�");
	//imshow( "�����ַ�",zifua[5] );
	//waitKey(0);


	//namedWindow( "�ߺ��ַ�");
	//imshow( "�ߺ��ַ�",zifua[6] );
	//waitKey(0);

	///////////////////////////////////////////////////////////////////////////////////////    ͼ���һ��
	//�ߴ����Ϊ20*20
	Mat outt[7];

	//cout<<"zifu"<<endl<<zifua[0].cols<<endl<<zifua[0].rows<<endl;
//	Mat CharsChuli(Mat);
	for(int i=0;i<7;i++)
	{
		outt[i]=CharsChuli(zifua[i]);
		//threshold( outt[i],outt[i], 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	}

	double qq=zifua[0].cols;
	double ww=zifua[0].rows;
	double bi=qq/ww*20;//8.8^
	int ci;
	int di=bi;//doubleת��int
	if((bi-di)>=0.5){ci=di+1;}
	else {ci=di;}
	cv::resize(zifua[0],outt[0],Size(ci,20),0,0,INTER_LINEAR);//ԭͼ��ת��Ϊ��20��x��ԭ����ͼ��������Ϻڱߣ���Ϊ20*20ͼ��
	int al=(20-ci)/2;
	int ar=20-al-ci;
	copyMakeBorder( outt[0],outt[0],0,0,al,ar,BORDER_CONSTANT,0);
	threshold( outt[0],outt[0], 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	for(int i=1;i<7;i++)
	{
		//if(zifua[i].rows/zifua[i].cols>=3)
		//{
		int aa=20*zifua[i].cols/zifua[i].rows;
		cv::resize(zifua[i],outt[i],Size(aa,20),0,0,INTER_LINEAR);

		int xl=(20-aa)/2;
		int xr=20-xl-aa;
		copyMakeBorder( outt[i],outt[i],0,0,xl,xr,BORDER_CONSTANT,0);

		//cvtColor(outt[i], outt[i], CV_RGB2GRAY);
		threshold( outt[i],outt[i], 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	}
	//cout<<outt[0]<<"/////"<<endl;;
	/////////////////////////////////////////////////////////////////////////////////////////    ������ȡ
	Mat features[7];
	for(int p=0;p<7;p++)
	{
		Mat outtt=outt[p];
		//imshow("outtt",outtt);
		//waitKey(0);
		Mat vhist;
		Mat hhist;
//		Mat ReturnMat(Mat,int);////////////////��������
		vhist=ReturnMat(outt[p],0);
		hhist=ReturnMat(outt[p],1);
		Mat lowData;
		cv::resize(outtt, lowData, Size(10, 10) );
		//Last 10 is the number of moments components
		int numCols=vhist.cols+hhist.cols+lowData.cols*lowData.cols;//////////////��140ά������
		Mat out=Mat::zeros(1,numCols,CV_32F);
		//Asign values to feature,ANN����������Ϊˮƽ����ֱֱ��ͼ�͵ͷֱ���ͼ������ɵ�ʸ��
		int j=0;
		for(int i=0; i<vhist.cols; i++)
		{
			out.at<float>(j)=vhist.at<float>(i);
			j++;
		}
		for(int i=0; i<hhist.cols; i++)
		{
			out.at<float>(j)=hhist.at<float>(i);
			j++;
		}
		for(int x=0; x<lowData.cols; x++)
		{
			for(int y=0; y<lowData.rows; y++){
				out.at<float>(j)=(float)lowData.at<unsigned char>(x,y);
				j++;
			}
		}
		features[p]=out.clone();
		//cout<<out<<"\n============\n";
		//waitKey(0);
	}
	////////////////////////////////////////////////////////////////////     ����ANNģ��
	m_annMlp.clear();
	QString strPath=QCoreApplication::applicationDirPath()+"/practice/ann.xml";
	m_annMlp.load(strPath.toStdString().c_str(),"ann");//װ��
	Mat outkk[7];
	for(int i=0;i<7;i++)
	{
		cv::resize(outt[i],outkk[i],Size(70,70),0,0);
	}
	/*imwrite("out_0.jpg",outkk[0]);
	imwrite("out_1.jpg",outkk[1]);
	imwrite("out_2.jpg",outkk[2]);
	imwrite("out_3.jpg",outkk[3]);
	imwrite("out_4.jpg",outkk[4]);
	imwrite("out_5.jpg",outkk[5]);
	imwrite("out_6.jpg",outkk[6]);*/


	int result_one[7];
	/*int CharsChinese(Mat);
	int CharsCharacter(Mat);*/
//	string InttoChars(int);
	result_one[0]=CharsChinese(features[0]);
	for(int dd=1;dd<7;dd++)
	{
		result_one[dd]=CharsCharacter(features[dd]);
	}
	string outchars="";
	for(int k=0;k<7;k++)
	{

		string outchar=InttoChars(result_one[k]);
		outchars=outchars+outchar;
	}
	cout<<outchars<<endl;
	ITDevice* pVehicle=ITDataMgr::getInstance().createNewDevice();
	pVehicle->setVehicleNum(QString::fromStdString(outchars));
	pVehicle->setVehicleImage(srcImage);
	refreshTable();
//	QMessageBox::information(this,"���ƺţ�",QString::fromStdString(outchars));
//	ui.frame_showBrandNum->setText(outchars);
	//waitKey(0);
	//system("pause");

}
int QDetection::CharsChinese(Mat f)
{
	int result = -1;
	Mat output(1, numAll, CV_32FC1);
	m_annMlp.predict(f,output);//ʹ��ann���ַ����ж�

	float maxVal = -2;
	for(int j = numCharacter; j < numAll; j++)
	{
		float val = output.at<float>(j);
		//cout << "j:" << j << "val:"<< val << endl;
		if (val > maxVal)
			//��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
		{
			maxVal = val;
			result = j;
		}
	}
	return result;
}

int QDetection::CharsCharacter(Mat in)
{

	int result = -1;
	Mat output(1, numAll, CV_32FC1);
	m_annMlp.predict(in,output);//ʹ��ann���ַ����ж�

	float maxVal = -2;
	for(int j = 0; j < numCharacter; j++)
	{
		float val = output.at<float>(j);
		//cout << "j:" << j << "val:"<< val << endl;
		if (val > maxVal)
			//����ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
		{
			maxVal = val;
			result = j;
		}
	}


	return result;
}
Mat QDetection::ReturnMat(Mat img, int t)
{
	int sz = (t) ? img.rows : img.cols;
	Mat mhist = Mat::zeros(1, sz, CV_32F);

	for (int j = 0; j<sz; j++){
		Mat data = (t) ? img.row(j) : img.col(j);

		mhist.at<float>(j) =countNonZero(data);	//ͳ����һ�л�һ���У�����Ԫ�صĸ����������浽mhist��
	}

	//Normalize histogram
	double min, max;
	minMaxLoc(mhist, &min, &max);

	if (max>0)
		mhist.convertTo(mhist, -1, 1.0f / max, 0);//��mhistֱ��ͼ�е����ֵ����һ��ֱ��ͼ

	return mhist;
}
string QDetection::InttoChars(int q)
{
	string result="";
	if(q>33)
	{
		result=strChinese[q-34];

	}
	else{result=strCharacters[q];}
	return result;
}

Mat QDetection::histeq(Mat in)//ֱ��ͼ���⻯
{
	Mat out(in.size(), in.type());
	if (in.channels() == 3)
	{
		Mat hsv;
		vector<Mat> hsvSplit;
		cvtColor(in, hsv, CV_BGR2HSV);
		split(hsv, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, hsv);
		cvtColor(hsv, out, CV_HSV2BGR);
	}
	else if (in.channels() == 1)
	{
		equalizeHist(in, out);
	}
	return out;
}
Mat QDetection::CharsChuli(Mat in)
{
	int CHAR_SIZE=20;
	//Remap image
	int h = in.rows;
	int w = in.cols;
	int charSize = CHAR_SIZE;	//ͳһÿ���ַ��Ĵ�С
	Mat transformMat = Mat::eye(2, 3, CV_32F);
	int m = max(w, h);
	transformMat.at<float>(0, 2) = m / 2 - w / 2;
	transformMat.at<float>(1, 2) = m / 2 - h / 2;

	Mat warpImage(m, m, in.type());
	warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));

	Mat out;
	cv::resize(warpImage, out, Size(charSize, charSize));

	return out;
}


//************************************/
//������:  showTableItems
//���������������ʾ
//��������
//����ֵ��bool
//ʱ�䣺2016/7/22 WZQ
//************************************/
void QDetection::showTableItems(ITDeviceList pVehicles)
{
	clearTableItems();
	for(int i=0;i<pVehicles.size();++i)
	{
		ITDevice* pVehicle=pVehicles[i];
		if(!pVehicle)
			continue;
		QString strItem1,strItem2,strItem3,strItem4,strItem5,strItem6,strItem7,strItem8;
		strItem1=pVehicle->getVehicleNum();
		
		int rowno = ui.tableWidget->rowCount();
		QTableWidgetItem* item1 = new QTableWidgetItem(strItem1);
		item1->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(rowno-1,0,item1);

		QPushButton* pBtn=new QPushButton();		
		pBtn->setText("�鿴");
		QHBoxLayout* pHLayout = new QHBoxLayout();
		QWidget* pWidget = new QWidget(ui.tableWidget);
		pHLayout->addWidget(pBtn);
		pHLayout->setMargin(0);
		pHLayout->setAlignment(pBtn,Qt::AlignCenter);
		pWidget->setLayout(pHLayout);                       
		ui.tableWidget->setCellWidget(rowno-1,1,pWidget);
		connect(pBtn,SIGNAL(clicked()),this,SLOT(doDisplayImage()));
		m_nRowForBtn.insert(rowno-1,pBtn);               
		m_rowForVehicle.insert(rowno-1,pVehicle);
		if(rowno == pVehicles.size())
		{
			return;
		}
		rowno++;
		ui.tableWidget->setRowCount(rowno);                       
	}

}
//************************************/
//������:  clearTableItems
//���������������������ϵ
//��������
//����ֵ��bool
//ʱ�䣺2016/7/22 WZQ
//************************************/
void QDetection::clearTableItems()
{
	int rowCounts = ui.tableWidget->rowCount();
	int colCounts = ui.tableWidget->columnCount();
	int nWidgetCol=-1;
	for(int i=0;i<rowCounts-1;i++)
	{
		for(int j=0;j<colCounts;j++)
		{
			QTableWidgetItem* tableitem = ui.tableWidget->item(i,j);
			if(tableitem!=NULL)
				delete tableitem;
		}
		if(nWidgetCol >= 0)
		{
			QWidget* tempWidget =  ui.tableWidget->cellWidget(i,nWidgetCol);//��checkBoxֵȡ����  ȥ�ͷ�
			if(tempWidget!=NULL)
				delete tempWidget;
		}
		ui.tableWidget->removeRow(i);               
	}
	ui.tableWidget->clearContents();
	m_rowForVehicle.clear();
	m_nRowForBtn.clear();       
	ui.tableWidget->setRowCount(1);

}
void QDetection::refreshTable()
{
	ITDeviceList pDevList=ITDataMgr::getInstance().getAllDevices();
	showTableItems(pDevList);
}
void QDetection::doDisplayImage()
{
	QObject* obj=sender();
	int nRow=m_nRowForBtn.key((QPushButton*)obj);
	if(nRow < 0)
		return;
	ITDevice* pDev=m_rowForVehicle.value(nRow);
	if(!pDev)
		return;
	QDialog dlg(this);
	dlg.setWindowFlags(dlg.windowFlags() &~ Qt::WindowContextHelpButtonHint);
	dlg.setWindowTitle("������Ϣ");
	QVBoxLayout vLayout(&dlg);
	dlg.setMinimumWidth(100);
	dlg.setMinimumHeight(100);
	QHBoxLayout hLayout(&dlg);
	QLineEdit lineEdit(&dlg);
	QLabel label(&dlg);
	label.setText("ʶ���ƺţ�");
	lineEdit.setText(pDev->getVehicleNum());
	lineEdit.setEnabled(false);
	hLayout.addWidget(&label);
	hLayout.addWidget(&lineEdit);
	QLabel imageLabel(&dlg);
	imageLabel.setPixmap(QPixmap::fromImage(pDev->getVehicleImage()));
	QPushButton btn1(&dlg),btn2(&dlg);
	btn1.setText("ȷ��");
	btn2.setText("ȡ��");
	QHBoxLayout hLayout2(&dlg);
	hLayout2.addStretch();
	hLayout2.addWidget(&btn1);
	hLayout2.addWidget(&btn2);
	hLayout2.addStretch();
	vLayout.addLayout(&hLayout);
	vLayout.addWidget(&imageLabel);
	vLayout.addLayout(&hLayout2);
	connect(&btn1,SIGNAL(clicked()),&dlg,SLOT(accept()));
	connect(&btn2,SIGNAL(clicked()),&dlg,SLOT(reject()));
	if(dlg.exec() == QDialog::Accepted)
	{
	}
}

//void QDetection::resizeEvent(QResizeEvent *eve)
//{
//	setVideoFrameImage(m_curDisImage);
//}
bool QDetection::eventFilter(QObject *obj, QEvent *eve)
{
	if(obj == ui.frame_video)
	{
		if(eve->type() == QEvent::Resize)
		{
			setVideoFrameImage(m_curDisImage);
			return true;
		}
	}
	return QWidget::eventFilter(obj,eve);
}