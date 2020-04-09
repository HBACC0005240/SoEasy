// plantsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "plants.h"
#include "plantsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CplantsDlg �Ի���




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


// CplantsDlg ��Ϣ�������

BOOL CplantsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    SetTimer(101,1000,NULL);//��Ǯ
	 SetTimer(102,1000,NULL);//����
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CplantsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CplantsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�������̷���Ȩ��   
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
	//HWND h=::FindWindow(NULL,L"ֲ���ս��ʬ���İ�");
	//if (h==0)
	//{
	//	TRACE("��Ϸδ��");
	//	return NULL;
	//}
	////1005194
	//DWORD pid;
	//GetWindowThreadProcessId(h,&pid);
	//GetWindowThreadProcessId
	//OpenProcss
	EnableDebugPriv();//��������Ȩ�� VS2008�Ժ�İ汾����Ҫ
	HANDLE hp=OpenProcess(PROCESS_ALL_ACCESS,false,5688);
	return hp;
	//
}
void CplantsDlg::OnBnClickedCheckCd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);//���´���״̬������
	UCHAR buf[2];
	DWORD bywrite;
    HANDLE hp=GetGameProcessHanlde();
	if (hp==NULL)
	{
		::MessageBox(0,L"�򿪽��̳���",0,MB_OK);
		return;
	}
	//��ȡ���� 
	
	if (m_b_cd)
	{   //���õ���ȴʱ��
		//00487296 - 7e 14                      - jle 004872ac //nop nop  //ȥ����ȴʱ��
		//	90 90
		buf[0]=0x90;
		buf[1]=0x90;
	}else
	{   //������ȴʱ��
		//0487296 - 7e 14  
		buf[0]=0x7e;
		buf[1]=0x14;
	}
	//��buf������д�� 0x0487296
	WriteProcessMemory(hp,(LPVOID)0x0487296,buf,sizeof(buf),&bywrite);
	CloseHandle(hp);//�ͷŵ����
}

void CplantsDlg::OnEnChangeEditMoney()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CplantsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	CString s;
   switch (nIDEvent)
   {
   case 101: //���½�������� IDC_EDIT_MONEY
	   
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
	   //������������������
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
     // UpdateData(false);//����ֵ������ʾ������
   CloseHandle(hp);
	CDialog::OnTimer(nIDEvent);
}

void CplantsDlg::OnEnSetfocusEditMoney()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���༭�� ��ȡ����ʱ ���ö�ʱ��
	KillTimer(101);
}

void CplantsDlg::OnBnClickedButtonEditmoney()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�޸Ľ��
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	//dd  [[6A9EC0]+82c]+28 
	//6A9EC0

	//[[6A9EC0]+82c]+28 
	ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	////[6A9EC0]+82c]
	ReadProcessMemory(hp,(PVOID)(buf+0x82c),&buf,sizeof(buf),&byread);
	UpdateData(TRUE);//�Ѵ������ݸ���������
	 
	WriteProcessMemory(hp,(PVOID)(buf+0x28),&m_u_money,sizeof(buf),&byread);
	
	//UpdateData(false);//����ֵ������ʾ������

	SetTimer(101,1000,NULL);
}

void CplantsDlg::OnEnKillfocusEditMoney()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetTimer(101,1000,NULL);
}

void CplantsDlg::OnEnChangeEditSunlight()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CplantsDlg::OnEnSetfocusEditSunlight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(102);
}

void CplantsDlg::OnEnKillfocusEditSunlight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetTimer(102,1000,NULL);
}

void CplantsDlg::OnBnClickedButtonEditsunlight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD byWrite;
	//��Ϸ���̾��
	HANDLE hp=GetGameProcessHanlde();
	//��Ŀ����̷����ڴ�ռ� �Է���д��Ҫִ�еĴ���
	PVOID FarCall=VirtualAllocEx(hp,NULL,0x8FFF,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	//��Ŀ����̵� Ŀ���ַд������Ҫִ�еĴ��� 
	WriteProcessMemory(hp,FarCall,plant1,0x8FFF,&byWrite);
	//��Ŀ����� ָ����ַ ִ�д���
	TRACE("\n addr=%x \n",FarCall);
	CreateRemoteThread(hp,NULL,NULL,( LPTHREAD_START_ROUTINE)FarCall,NULL,NULL,NULL);


}
