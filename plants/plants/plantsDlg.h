// plantsDlg.h : ͷ�ļ�
//

#pragma once


// CplantsDlg �Ի���
class CplantsDlg : public CDialog
{
// ����
public:
	CplantsDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PLANTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
