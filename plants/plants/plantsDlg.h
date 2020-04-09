// plantsDlg.h : 头文件
//

#pragma once


// CplantsDlg 对话框
class CplantsDlg : public CDialog
{
// 构造
public:
	CplantsDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PLANTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_b_cd;
	afx_msg void OnBnClickedCheckCd();
	UINT m_u_money;
	afx_msg void OnEnChangeEditMoney();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnSetfocusEditMoney();
	afx_msg void OnBnClickedButtonEditmoney();
	afx_msg void OnEnKillfocusEditMoney();
	UINT m_uSunlight;
	afx_msg void OnEnChangeEditSunlight();
	afx_msg void OnEnSetfocusEditSunlight();
	afx_msg void OnEnKillfocusEditSunlight();
	afx_msg void OnBnClickedButtonEditsunlight();
	afx_msg void OnBnClickedButtonPlant();
};
