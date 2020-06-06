#include "TestCGMap.h"
#include "global_def.h"
#include <QPainter>
HINSTANCE hTSComponentDLL;
#ifdef _DEBUG
#include ".\Debug\tsplug.tlh"
#else
#include ".\Release\tsplug.tlh"
#endif

ITSPlugInterFace* tsPlug = nullptr;
//#import "TSPlug.tlb" no_namespace,raw_interfaces_only
#import "TSPlug.tlb" no_namespace

TestCGMap::TestCGMap(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_collision = new cga_map_cells_t;
	readMap();
	loadTSPlug();
}

TestCGMap::~TestCGMap()
{
	unLoadTSPlug();
}

void TestCGMap::readMap()
{	
	char mapname[1024]="60099.dat";
//	format_mapname(mapname, *g_map_index1, *g_map_index2, *g_map_index3);
	auto fp = fopen(mapname, "rb");
	if (fp)
	{
		fseek(fp, 12, SEEK_SET);
		int xsize = 0, ysize = 0;
		fread(&xsize, 4, 1, fp);
		fread(&ysize, 4, 1, fp);

		if (xsize > 0 && ysize > 0 && xsize < 10000 && ysize < 10000)
		{
			short *collisiondata = (short *)malloc(2 * xsize * ysize);

			fseek(fp, 20 + 2 * xsize * ysize, SEEK_SET);
			fread(collisiondata, 2 * xsize * ysize, 1, fp);

			m_collision->x_bottom = 0;
			m_collision->y_bottom = 0;
			m_collision->xsize = xsize;
			m_collision->ysize = ysize;

			m_collision->cells.resize(xsize * ysize);
			memcpy(m_collision->cells.data(), collisiondata, 2 * xsize * ysize);

			free(collisiondata);

			fclose(fp);

			return;
		}

		fclose(fp);
	}
	//cells->x_bottom = *g_map_x_bottom;
	//cells->y_bottom = *g_map_y_bottom;
	//cells->xsize = *g_map_xsize;
	//cells->ysize = *g_map_ysize;
	//cells->cell.resize(cells->xsize*cells->ysize);
	//memcpy(cells->cell.data(), g_map_collision_table_raw, 2 * cells->xsize*cells->ysize);
}

void TestCGMap::loadTSPlug()
{
	HRESULT hr = CoInitialize(NULL);
	if FAILED(hr) // SUCCEEDED
		return;

	wchar_t wchPath[MAX_PATH];
	GetModuleFileName(nullptr, wchPath, MAX_PATH);
	std::wstring wszPath(wchPath);
	size_t iIndex = wszPath.rfind(L'\\');

	if (iIndex == std::wstring::npos)
		iIndex = wszPath.rfind(L'/');

	if (iIndex == std::wstring::npos)
		return;
	m_runPath = QString::fromStdWString(wszPath.substr(0, iIndex));
#ifdef _DEBUG
	wszPath = wszPath.substr(0, iIndex) + L"\\TSPlugd.dll";
#else
	wszPath = wszPath.substr(0, iIndex) + L"\\TSPlug.dll";
#endif
	hTSComponentDLL = LoadLibrary(wszPath.c_str());
	if (!tsPlug)
	{
		BOOL(WINAPI*pDllGetClassObject)(REFCLSID, REFIID, LPVOID) =
			(BOOL(WINAPI*)(REFCLSID, REFIID, LPVOID))GetProcAddress(hTSComponentDLL, "DllGetClassObject");

		if (!pDllGetClassObject)
			return;

		IClassFactory* pClassFactory = nullptr;
		HRESULT hr = pDllGetClassObject(__uuidof(TSPlugInterFace), IID_IClassFactory, &pClassFactory);
		if (FAILED(hr))
			return;

		hr = pClassFactory->CreateInstance(NULL, __uuidof(ITSPlugInterFace), (void**)&tsPlug);
		if (FAILED(hr))
		{
			pClassFactory->Release();
			return;
		}
		pClassFactory->Release();
	}
}

void TestCGMap::unLoadTSPlug()
{
	if (tsPlug)
	{
		tsPlug->Release();
		tsPlug = nullptr;
	}

	if (hTSComponentDLL)
	{
		FreeLibrary(hTSComponentDLL);
	}
	CoUninitialize();
}

void TestCGMap::paintEvent(QPaintEvent *event) 
{
	auto widgetSize = size();

	if (m_collision/* && m_objects*/ && m_collision->xsize && m_collision->ysize)
	{
		auto blockSize = 8;
		QPixmap pix((m_collision->xsize + 1) * blockSize, (m_collision->ysize + 1) * blockSize);
		QPainter painter(&pix);

		QBrush brushwh(Qt::white);
		QBrush brushgray(Qt::gray);
		QBrush brushred(Qt::red);
		QBrush brushblue(Qt::blue);
		QBrush brushgreen(Qt::green);
		QBrush brushdy(Qt::darkMagenta);
		painter.fillRect(pix.rect(), brushwh);

		for (int y = 0; y < m_collision->ysize; ++y)
		{
			for (int x = 0; x < m_collision->xsize; ++x)
			{
				auto cell = m_collision->cells.at((size_t)(x + y * m_collision->xsize));
				if (cell == 1)
					painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushgray);
			}
		}

		//for (int y = 0; y < m_objects->ysize; ++y)
		//{
		//	for (int x = 0; x < m_objects->xsize; ++x)
		//	{
		//		auto cell = m_objects->cells.at((size_t)(x + y * m_objects->xsize));
		//		if (cell & 0xff) {
		//			if ((cell & 0xff) == 3)
		//				painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushblue);
		//			else
		//				painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushgreen);

		//			// qDebug("object %X at %d %d", cell, x, y);
		//		}

		//		if (x == m_playerx - m_collision->xbottom && y == m_playery - m_collision->ybottom)
		//			painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushred);
		//	}
		//}

		//for (int i = 0; i < m_units->size(); ++i)
		//{
		//	const auto &unit = m_units->at(i);
		//	if (unit.valid && unit.type == 1 && unit.model_id != 0 && unit.unit_name.length())
		//	{
		//		int x = unit.xpos;
		//		int y = unit.ypos;
		//		painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushdy);
		//	}
		//}

		//if (true)//人物位置
		//{
		//	QPen redpen(Qt::red, blockSize);
		//	painter.setPen(redpen);
		//	painter.drawLine(0, blockSize + blockSize / 2, pix.width(), blockSize + blockSize / 2);
		//	painter.drawLine(blockSize + blockSize / 2, 0, blockSize + blockSize / 2, pix.height());
		//}

		painter.end();
		painter.begin(this);
		painter.drawPixmap(0, 0, widgetSize.width() - 1, widgetSize.height() - 1, pix);
		painter.setPen(Qt::black);
		painter.drawRect(0, 0, widgetSize.width() - 1, widgetSize.height() - 1);
	}
}

void TestCGMap::on_pushButton_clicked()
{
	ITSPlugInterFacePtr pTSPlug = tsPlug;
	if (!pTSPlug)
	{
		//throw Exception(QObject::tr("Component initlization error!"));
		return;
	}
	pTSPlug->SetMouseDelay("dx", 50);
	//              Fdm[i].SetDictPwd("fairy0927");
	pTSPlug->SetPicPwd("fairy0927");
	//                Fdm[i].SetDict(0, strPath + "\\sys\\df.txt");
	//              Fdm[i].SetDict(1, strPath + "di.txt");
	pTSPlug->SetShowErrorMsg(0);
	QString tempPath = m_runPath + "\\sys\\";
	tsPlug->SetPath(tempPath.toStdWString().c_str());
	pTSPlug->SetDict(0, "df.txt");
	pTSPlug->UseDict(0);
//	SysOs = pTSPlug->GetOsType();
	string Pccode = pTSPlug->GetMachineCode();
	long hwnd = pTSPlug->FindWindowByProcess("mspaint.exe", "", "画图");
	pTSPlug->BindWindow(hwnd, "normal", "normal", "normal", 0);
	VARIANT x, y;
	pTSPlug->FindStr(0, 0, 1000, 1000, L"是", "ffffff-101010",1.0, &x, &y);
	string s = pTSPlug->Ocr(0, 0, 2000, 2000, "9f2e3f-000000", 1.0);
	int i = 0;
	i = 1;
}
