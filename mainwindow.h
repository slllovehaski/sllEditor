#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QTreeWidget;
class QMenu;
class QTextEdit;
class QLineEdit;
class TextFilter;
class QTreeWidgetItem;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void newLetter();
    void save();
    void saveCfg();
    void undo();
    void open();
    void openXml();
    void treeStateChanged(QTreeWidgetItem* item,int index);
    void prepareMenu(const QPoint & pos);
    void newChildFolder();
    void newChildTitle();
    void newBroFolder();
    void newTitle();
    void delTitle();
    void delFolder();
private:
    void createActions();
    void createStatusBar();
    void createDockWindows();
    void readSettings();
    void openFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void setitemCheckState(QTreeWidgetItem* item, Qt::CheckState state);
    void updateFindText();
    void getSelText(QTreeWidgetItem* item, QStringList& selText);

    TextFilter *textEdit;
    QTreeWidget *searchItemTree;
    QLineEdit *searchItemFilter;
    QMenu *viewMenu;

    QString curFile;
    bool isUntitled;
};
//! [0]

#endif
