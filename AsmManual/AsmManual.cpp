#include "AsmManual.h"
#include "../include/ITEngine.h"
#include <QMessageBox>
AsmManual::AsmManual(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//ui.comboBox->setStyleSheet("QComboBox{border:1px solid gray;}"
	//	"QComboBox QAbstractItemView::item{height:20;}"); //下拉选项高度               
	//ui.comboBox->setView(new QListView());
	init();
	connect(ui.lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(doAsmFilter(const QString&)));
	connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(doLineEditEnter()));
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(doListWidgetItemClicked(QListWidgetItem*)));
	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void AsmManual::init()
{
	QString szDBPath = QApplication::applicationDirPath() + "//db//asm.db";
	bool bRet = ITEngine::GetInstance().connectToDataBase(szDBPath, "SQLITE", "127.0.0.1", "", "");
	QSqlTableModel* pAsmTypeTable = ITEngine::GetInstance().getTable("asmType");
	QSqlTableModel* pAsmTable = ITEngine::GetInstance().getTable("asm");
	ui.comboBox_asmType->clear();
	m_asmMapItem.clear();

	if (pAsmTypeTable)
	{
		for (int i = 0;i < pAsmTypeTable->rowCount();++i)
		{
			QSqlRecord record = pAsmTypeTable->record(i);
			QString asmTypeName = record.value("name").toString();
			ui.comboBox_asmType->addItem(asmTypeName,record.value("id"));
		}
	}	
	ui.comboBox_asmType->setCurrentIndex(0);

	if (pAsmTable)
	{
		for (int i = 0;i < pAsmTable->rowCount();++i)
		{
			QSqlRecord record = pAsmTable->record(i);
			QString asmFun = record.value("function").toString();
			QString asmName = record.value("name").toString();
			QListWidgetItem* pItem = new QListWidgetItem(asmName);
			pItem->setData(Qt::UserRole, record.value("id"));
			ui.listWidget->addItem(pItem);
			m_asmMapItem.insert(asmName.toLower(), pItem);
		}
	}

}

void AsmManual::on_pushButton_clicked()
{
	//查询
	doLineEditEnter();
}

void AsmManual::on_pushButton_add_clicked()
{
	QSqlTableModel* pTable = ITEngine::GetInstance().getTable("asm");
	if (pTable)
	{
		if (ui.lineEdit_asmName->text().isEmpty())
		{
			QMessageBox::information(this, "提示：", "指令名称不能为空！", QMessageBox::Ok);
			return;
		}
		QSqlRecord record = pTable->record();
//		record.setValue("id", ITObjectID::NewID());//自增不管了
		record.setValue("name", ui.lineEdit_asmName->text());
		record.setValue("type", ui.comboBox_asmType->currentData());
		record.setValue("function", ui.lineEdit_fun->text());
		record.setValue("format", ui.lineEdit_format->text());
		record.setValue("example", ui.textEdit_exam->toPlainText());
		record.setValue("mcode", ui.lineEdit_mcode->text());
		record.setValue("remark", ui.textEdit_remark->toPlainText());
		//	record.setValue("remark", fileInfo);
		pTable->insertRecord(pTable->rowCount(), record);
		QListWidgetItem* pItem = new QListWidgetItem(ui.lineEdit_asmName->text());
		ui.listWidget->addItem(pItem);
		pItem->setData(Qt::UserRole, record.value("id"));
	}
}

void AsmManual::on_pushButton_edit_clicked()
{

}
void AsmManual::on_listWidget_customContextMenuRequested(const QPoint & pos)
{
	QListWidgetItem* curItem = ui.listWidget->itemAt(pos);  //获取当前被点击的节点 
	if (curItem != NULL)
	{
		QMenu menu;
		menu.addAction("删除", this, SLOT(doDelItem()));
		menu.exec(QCursor::pos());
	}
}
void AsmManual::doDelItem()
{
	QListWidgetItem* pItem = ui.listWidget->selectedItems().at(0);
	if (pItem)
	{
		quint64 id = pItem->data(Qt::UserRole).toULongLong();
		ITSqlTable* pTable = ITEngine::GetInstance().getTable("asm");
		if (pTable)
		{

		}
	}
}
void AsmManual::doLineEditEnter()
{
	QString szText = ui.lineEdit->text();
	doAsmFilter(szText);
}

void AsmManual::doAsmFilter(const QString& szFilter)
{
	ui.comboBox_asmType->setCurrentIndex(0);
	QString szLowerFilter = szFilter.toLower();
	if (m_asmMapItem.contains(szLowerFilter))
	{
		ui.listWidget->setCurrentItem(m_asmMapItem.value(szLowerFilter));
		int count = ui.listWidget->count();
		for (int i = 0; i < count; i++)
		{
			QListWidgetItem *item = ui.listWidget->item(i);
			item->setHidden(true);
		}
		m_asmMapItem.value(szLowerFilter)->setHidden(false);
	}
	else
	{
		for (auto it=m_asmMapItem.begin();it!=m_asmMapItem.end();++it)
		{
			if (it.key().contains(szLowerFilter))
			{
				it.value()->setHidden(false);
			}
			else
			{
				it.value()->setHidden(true);
			}
		}
	}
	ui.listWidget->setCurrentRow(0);
	doListWidgetItemClicked(ui.listWidget->item(0));
}

void AsmManual::doListWidgetItemClicked(QListWidgetItem* pItem)
{
	if (pItem == NULL)
		return;
	
	quint64 asmID = pItem->data(Qt::UserRole).toInt();
	ITSqlTable* pAsmTable = ITEngine::GetInstance().getTable("asm");
	if(pAsmTable)
	{
		QSqlRecord record = pAsmTable->findById64(asmID);
		int asmType = record.value("type").toInt();
		QString asmName = record.value("name").toString();
		QString asmFormat = record.value("format").toString();
		QString asmMcode = record.value("mcode").toString();
		QString asmFun = record.value("function").toString();
		QString asmExam = record.value("example").toString();
		QString asmRemark = record.value("remark").toString();
		ui.comboBox_asmType->setCurrentIndex(ui.comboBox_asmType->findData(asmType));
		ui.lineEdit_asmName->setText(asmName);
		ui.lineEdit_format->setText(asmFormat);
		ui.lineEdit_mcode->setText(asmMcode);
		ui.lineEdit_fun->setText(asmFun);
		ui.textEdit_exam->setText(asmExam);
		ui.textEdit_remark->setText(asmRemark);
		//QString displayText = QStringLiteral("\n\n%1 %2\n\n格式：%3\n\n功能：%4").arg(QStringLiteral("指令类型")).arg(pItem->text()).arg("").arg(szFun);
		//ui.textEdit_fun->setText(displayText);
	}
}
