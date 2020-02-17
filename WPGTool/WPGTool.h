#ifndef WPGTOOL_H
#define WPGTOOL_H

#include <QtWidgets/QDialog>
#include "ui_WPGTool.h"
#include <windows.h>
class WPGTool : public QDialog
{
	Q_OBJECT

public:
	WPGTool(QWidget *parent = 0);
	~WPGTool();

	void initMode();
	void initHook();
	void initHotKey();
	bool GetProcessList();
	void createStatusBar();
	void clearTableItems();
	void startSync();
	void stopSync();
	static LRESULT CallBackProc(DWORD _dwCode, DWORD _wParam, DWORD _lParam);
	static LRESULT MouseHookProc(DWORD _dwCode, DWORD _wParam, DWORD _lParam);
	static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
	HWND FindMainWindow(unsigned long process_id);
	static BOOL IsMainWindow(HWND handle);

public slots:
	void doBtnClicked();
	void dealHotKeyEvent(const QKeySequence& key);
protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::WPGToolClass ui;
	uint m_KeyStart;
	uint m_KeyStop;
	
};

#endif // WPGTOOL_H
