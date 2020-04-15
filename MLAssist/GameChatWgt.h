#pragma once

#include <QWidget>
#include "ui_GameChatWgt.h"

class GameChatWgt : public QWidget
{
	Q_OBJECT

public:
	GameChatWgt(QWidget *parent = Q_NULLPTR);
	~GameChatWgt();

	void addOneChatData(const QString& chatData);
public slots:
	void doUpdateGameChat();
private:
	Ui::GameChatWgt ui;
	int m_nLastIndex;	//�ϴμ�¼
};