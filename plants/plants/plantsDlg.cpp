// plantsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "plants.h"
#include "plantsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CplantsDlg 对话框




CplantsDlg::CplantsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CplantsDlg::IDD, pParent)
	, m_b_cd(FALSE)
	, m_u_money(0)
	, m_uSunlight(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CplantsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_CD, m_b_cd);
	DDX_Text(pDX, IDC_EDIT_MONEY, m_u_money);
	DDX_Text(pDX, IDC_EDIT_SUNLIGHT, m_uSunlight);
}

BEGIN_MESSAGE_MAP(CplantsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_CD, &CplantsDlg::OnBnClickedCheckCd)
	ON_EN_CHANGE(IDC_EDIT_MONEY, &CplantsDlg::OnEnChangeEditMoney)
	ON_WM_TIMER()
	ON_EN_SETFOCUS(IDC_EDIT_MONEY, &CplantsDlg::OnEnSetfocusEditMoney)
	ON_BN_CLICKED(IDC_BUTTON_EDITMONEY, &CplantsDlg::OnBnClickedButtonEditmoney)
	ON_EN_KILLFOCUS(IDC_EDIT_MONEY, &CplantsDlg::OnEnKillfocusEditMoney)
	ON_EN_CHANGE(IDC_EDIT_SUNLIGHT, &CplantsDlg::OnEnChangeEditSunlight)
	ON_EN_SETFOCUS(IDC_EDIT_SUNLIGHT, &CplantsDlg::OnEnSetfocusEditSunlight)
	ON_EN_KILLFOCUS(IDC_EDIT_SUNLIGHT, &CplantsDlg::OnEnKillfocusEditSunlight)
	ON_BN_CLICKED(IDC_BUTTON_EDITSUNLIGHT, &CplantsDlg::OnBnClickedButtonEditsunlight)
	ON_BN_CLICKED(IDC_BUTTON_PLANT, &CplantsDlg::OnBnClickedButtonPlant)
END_MESSAGE_MAP()


// CplantsDlg 消息处理程序

BOOL CplantsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    SetTimer(101,1000,NULL);//金钱
	 SetTimer(102,1000,NULL);//阳光
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CplantsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CplantsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CplantsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//提升进程访问权限   
bool EnableDebugPriv()  
{  
	HANDLE hToken;  
	LUID sedebugnameValue;  
	TOKEN_PRIVILEGES tkp;  

	if (!OpenProcessToken(GetCurrentProcess(),  
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {  
			return false;  
	}  
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) 
	{  
		CloseHandle(hToken);  
		return false;  
	}  
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Luid = sedebugnameValue;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) 
	{  
		CloseHandle(hToken);  
		return false;  
	}  
	return true;  
}  
HANDLE GetGameProcessHanlde()
{
	//HWND h=::FindWindow(NULL,L"植物大战僵尸中文版");
	//if (h==0)
	//{
	//	TRACE("游戏未打开");
	//	return NULL;
	//}
	////1005194
	//DWORD pid;
	//GetWindowThreadProcessId(h,&pid);
	//GetWindowThreadProcessId
	//OpenProcss
	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	HANDLE hp=OpenProcess(PROCESS_ALL_ACCESS,false,5688);
	return hp;
	//
}
void CplantsDlg::OnBnClickedCheckCd()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);//更新窗口状态至变量
	UCHAR buf[2];
	DWORD bywrite;
    HANDLE hp=GetGameProcessHanlde();
	if (hp==NULL)
	{
		::MessageBox(0,L"打开进程出错",0,MB_OK);
		return;
	}
	//读取数据 
	
	if (m_b_cd)
	{   //禁用掉冷却时间
		//00487296 - 7e 14                      - jle 004872ac //nop nop  //去掉冷却时间
		//	90 90
		buf[0]=0x90;
		buf[1]=0x90;
	}else
	{   //启用冷却时间
		//0487296 - 7e 14  
		buf[0]=0x7e;
		buf[1]=0x14;
	}
	//把buf内数据写入 0x0487296
	WriteProcessMemory(hp,(LPVOID)0x0487296,buf,sizeof(buf),&bywrite);
	CloseHandle(hp);//释放掉句柄
}

void CplantsDlg::OnEnChangeEditMoney()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CplantsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	CString s;
   switch (nIDEvent)
   {
   case 101: //更新金币数量至 IDC_EDIT_MONEY
	   
      //dd  [[6A9EC0]+82c]+28 
	   //6A9EC0
	   
	   //[[6A9EC0]+82c]+28 
	   ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	   ////[6A9EC0]+82c]
	   ReadProcessMemory(hp,(PVOID)(buf+0x82c),&buf,sizeof(buf),&byread);
	   ReadProcessMemory(hp,(PVOID)(buf+0x28),&buf,sizeof(buf),&byread);
	   m_u_money=buf;
	   
	   s.Format(L"%d",buf);
	   GetDlgItem(IDC_EDIT_MONEY)->SetWindowText(s);
	   break;
   case 102:
	   //更新阳光数组至窗口
	  // dd  [[[[6A9EC0]+768]+144]+2c]+5560 
ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x768),&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x144),&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x2c),&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x5560),&buf,sizeof(buf),&byread);
        m_uSunlight=buf;	
		
		s.Format(L"%d",buf);
GetDlgItem(IDC_EDIT_SUNLIGHT)->SetWindowText(s);
	   
	   break;
   }
     // UpdateData(false);//把数值更新显示到窗口
   CloseHandle(hp);
	CDialog::OnTimer(nIDEvent);
}

void CplantsDlg::OnEnSetfocusEditMoney()
{
	// TODO: 在此添加控件通知处理程序代码
	//当编辑框 获取焦点时 禁用定时器
	KillTimer(101);
}

void CplantsDlg::OnBnClickedButtonEditmoney()
{
	// TODO: 在此添加控件通知处理程序代码
	//修改金币
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	//dd  [[6A9EC0]+82c]+28 
	//6A9EC0

	//[[6A9EC0]+82c]+28 
	ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	////[6A9EC0]+82c]
	ReadProcessMemory(hp,(PVOID)(buf+0x82c),&buf,sizeof(buf),&byread);
	UpdateData(TRUE);//把窗口数据更新至变量
	 
	WriteProcessMemory(hp,(PVOID)(buf+0x28),&m_u_money,sizeof(buf),&byread);
	
	//UpdateData(false);//把数值更新显示到窗口

	SetTimer(101,1000,NULL);
}

void CplantsDlg::OnEnKillfocusEditMoney()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(101,1000,NULL);
}

void CplantsDlg::OnEnChangeEditSunlight()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CplantsDlg::OnEnSetfocusEditSunlight()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(102);
}

void CplantsDlg::OnEnKillfocusEditSunlight()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(102,1000,NULL);
}

void CplantsDlg::OnBnClickedButtonEditsunlight()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	ReadProcessMemory(hp,(PVOID)(buf+0x768),&buf,sizeof(buf),&byread);
	ReadProcessMemory(hp,(PVOID)(buf+0x144),&buf,sizeof(buf),&byread);
	ReadProcessMemory(hp,(PVOID)(buf+0x2c),&buf,sizeof(buf),&byread);
	
	WriteProcessMemory(hp,(PVOID)(buf+0x5560),&m_uSunlight,sizeof(buf),&byread);
	CloseHandle(hp);
}
__declspec(naked) void plant1(void)
{
	__asm
	{
		mov eax, 0
		mov ecx, 0
		mov edx, 0x00462EF8
		call edx
		ret			
	}
}
void CplantsDlg::OnBnClickedButtonPlant()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD byWrite;
	//游戏进程句柄
	HANDLE hp=GetGameProcessHanlde();
	//在目标进程分配内存空间 以方便写入要执行的代码
	PVOID FarCall=VirtualAllocEx(hp,NULL,0x8FFF,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	//向目标进程的 目标地址写入我们要执行的代码 
	WriteProcessMemory(hp,FarCall,plant1,0x8FFF,&byWrite);
	//在目标进程 指定地址 执行代码
	TRACE("\n addr=%x \n",FarCall);
	CreateRemoteThread(hp,NULL,NULL,( LPTHREAD_START_ROUTINE)FarCall,NULL,NULL,NULL);


}
