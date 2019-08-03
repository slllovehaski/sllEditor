//! [0]
#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QtPrintSupport>
#endif
#endif

#include "mainwindow.h"
#include <QLineEdit>
#include "codeeditor.h"
#include "textfilter.h"
#include "xbelreader.h"
#include "xbelwriter.h"
//! [0]

//! [1]
MainWindow::MainWindow()
    : textEdit(new TextFilter(this))
{
    setCentralWidget(textEdit);

    createActions();
    createStatusBar();
    createDockWindows();
    readSettings();
    setWindowTitle(tr("Dock Widgets"));

    newLetter();
}
//! [1]

//! [2]
void MainWindow::newLetter()
{
    //textEdit->clear();
}
//! [2]
//! [4]
void MainWindow::save()
{
    QMimeDatabase mimeDatabase;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a file name"), ".",
                                                    mimeDatabase.mimeTypeForName("text").filterString());
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Dock Widgets"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->ui_textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}
void MainWindow::saveCfg()
{
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save Cfg File"),
                                         QDir::currentPath(),
                                         tr("XBEL Files (*.xbel *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    XbelWriter writer(searchItemTree);
    if (writer.writeFile(&file))
        statusBar()->showMessage(tr("Cfg File saved"), 2000);
}
//! [4]

//! [5]
void MainWindow::undo()
{
    QTextDocument *document;// = textEdit->document();
    document->undo();
}
//! [5]

//! [6]
void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}
void MainWindow::openXml()
{
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Bookmark File"),
                                         QDir::currentPath(),
                                         tr("XBEL Files (*.xbel *.xml)"));
    if (fileName.isEmpty())
        return;

    searchItemTree->clear();
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    XbelReader reader(searchItemTree);
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
                             tr("Parse error in file %1:\n\n%2")
                             .arg(fileName)
                             .arg(reader.errorString()));
    } else {
        statusBar()->showMessage(tr("File loaded"), 2000);
    }
}
void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("document%1.txt").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    //textEdit->ui_textEdit->document()->setModified(false);
    setWindowModified(false);

    setWindowFilePath(curFile);
}
void MainWindow::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //textEdit->ui_textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
void MainWindow::openFile(const QString &fileName)
{
    //if (textEdit->ui_textEdit->document()->isEmpty() && !isWindowModified()) {
        loadFile(fileName);
        return;
   // }
}
void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width()*3/ 4, availableGeometry.height()*3/4);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}
void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    //QToolBar *fileToolBar = addToolBar(tr("File"));
    QAction *openAct = new QAction(tr("&OpenLog..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    QAction *openXmlAct = new QAction(tr("&OpenCfg..."), this);
    openXmlAct->setStatusTip(tr("Open an existing file"));
    connect(openXmlAct, &QAction::triggered, this, &MainWindow::openXml);
    fileMenu->addAction(openXmlAct);
    //fileToolBar->addAction(openAct);

    QAction *saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current form letter"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    fileMenu->addAction(saveAct);

    QAction *saveCfgAct = new QAction(tr("&Save Cfg..."), this);
    saveCfgAct->setStatusTip(tr("Save the current form letter"));
    connect(saveCfgAct, &QAction::triggered, this, &MainWindow::saveCfg);
    fileMenu->addAction(saveCfgAct);

    fileMenu->addSeparator();

    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    //QToolBar *editToolBar = addToolBar(tr("Edit"));
#if 0
    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/images/undo.png"));
    QAction *undoAct = new QAction(undoIcon, tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last editing action"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAct);
#endif
    //editToolBar->addAction(undoAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();
}

//! [8]
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
//! [8]

//! [9]
void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("key word pool"), this);
//dock->setMinimumWidth(300);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QWidget* my_search_ui = new QWidget();
    //my_search_ui->setMinimumWidth(150);
    QVBoxLayout* vlay = new QVBoxLayout(dock);
    //searchItemFilter = new QLineEdit(dock);
    vlay->sizeHint();
    //xml Treewidget
    QStringList labels;
    labels << tr("Key Word List");

    searchItemTree = new QTreeWidget(dock);
    searchItemTree->header()->setSectionResizeMode(QHeaderView::Stretch);
    searchItemTree->setHeaderLabels(labels);
    searchItemTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(searchItemTree,&QTreeWidget::customContextMenuRequested,this,&MainWindow::prepareMenu);
    connect(searchItemTree,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(treeStateChanged(QTreeWidgetItem*,int)));

    //vlay->addWidget(searchItemFilter);
    vlay->addWidget(searchItemTree);
    my_search_ui->setLayout(vlay);
    dock->setWidget(my_search_ui);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
    //connect(searchItemTree, &QTreeWidget::currentTextChanged,
        //    this, &MainWindow::insertCustomer);
}
void MainWindow::newChildFolder()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
       return;
    QPoint pos = action->property("pos").toPoint();
    QTreeWidgetItem *item = searchItemTree->itemAt(pos);
    QTreeWidgetItem *childItem;
    childItem = new QTreeWidgetItem(item);
    childItem->setData(0, Qt::UserRole, "folder");
    childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    childItem->setCheckState(0, Qt::Unchecked);
}
void MainWindow::newChildTitle()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
       return;
    QPoint pos = action->property("pos").toPoint();
    QTreeWidgetItem *item = searchItemTree->itemAt(pos);
    QTreeWidgetItem *childItem;
    childItem = new QTreeWidgetItem(item);
    childItem->setData(0, Qt::UserRole, "title");
    childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    childItem->setCheckState(0, Qt::Unchecked);
}
void MainWindow::newBroFolder()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
       return;
    QPoint pos = action->property("pos").toPoint();
    QTreeWidgetItem *item = searchItemTree->itemAt(pos);
    QTreeWidgetItem *parent = item->parent();
    QTreeWidgetItem* childItem = new QTreeWidgetItem(parent);
    childItem->setData(0, Qt::UserRole, "Folder");
    childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    childItem->setCheckState(0, Qt::Unchecked);
}
void MainWindow::newTitle()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
       return;
    QPoint pos = action->property("pos").toPoint();
    QTreeWidgetItem *item = searchItemTree->itemAt(pos);
    QTreeWidgetItem *parent = item->parent();
    QTreeWidgetItem* childItem = new QTreeWidgetItem(parent);
    childItem->setData(0, Qt::UserRole, "title");
    childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    childItem->setCheckState(0, Qt::Unchecked);
}
void MainWindow::delTitle()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
       return;
    QPoint pos = action->property("pos").toPoint();
    QTreeWidgetItem *item = searchItemTree->itemAt(pos);
    qDebug()<<item->text(0);
    delete item;
}
void MainWindow::delFolder()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
       return;
    QPoint pos = action->property("pos").toPoint();
    QTreeWidgetItem *item = searchItemTree->itemAt(pos);
    delete item;
}
void MainWindow::prepareMenu( const QPoint & pos )
{
    QTreeWidgetItem *nd = searchItemTree->itemAt(pos);
    qDebug()<<pos<<nd->text(0);
    QString tagName = nd->data(0, Qt::UserRole).toString();
    QMenu menu(this);
    if(tagName == "folder")
    {
        QAction *newChildFolder = new QAction(tr("&Add Child: folder"), this);
        newChildFolder->setProperty("pos", pos);
        newChildFolder->setStatusTip(tr("new folder"));
        connect(newChildFolder, SIGNAL(triggered()), this, SLOT(newChildFolder()));
        menu.addAction(newChildFolder);

        QAction *newChildTitle = new QAction(tr("&Add Child: title"), this);
        newChildTitle->setProperty("pos", pos);
        newChildTitle->setStatusTip(tr("new folder"));
        connect(newChildTitle, SIGNAL(triggered()), this, SLOT(newChildTitle()));
        menu.addAction(newChildTitle);

        QAction *newBroFolder = new QAction(tr("&Add Brother: folder"), this);
        newBroFolder->setProperty("pos", pos);
        newBroFolder->setStatusTip(tr("new bro folder"));
        connect(newBroFolder, SIGNAL(triggered()), this, SLOT(newBroFolder()));
        menu.addAction(newBroFolder);

        QAction *newTitle = new QAction(tr("&Add title"), this);
        newTitle->setProperty("pos", pos);
        newTitle->setStatusTip(tr("new title"));
        connect(newTitle, SIGNAL(triggered()), this, SLOT(newTitle()));
        menu.addAction(newTitle);

        QAction *delfolder = new QAction(tr("&Delete folder"), this);
        delfolder->setProperty("pos", pos);
        delfolder->setStatusTip(tr("del folder"));
        connect(delfolder, SIGNAL(triggered()), this, SLOT(delFolder()));
        menu.addAction(delfolder);
    }else if(tagName == "title")
    {
        QAction *newTitle = new QAction(tr("&Add title"), this);
        newTitle->setProperty("pos", pos);
        newTitle->setStatusTip(tr("new title"));
        connect(newTitle, SIGNAL(triggered()), this, SLOT(newTitle()));
        menu.addAction(newTitle);

        QAction *delTitle = new QAction(tr("&Delete title"), this);
        delTitle->setProperty("pos", pos);
        delTitle->setStatusTip(tr("del title"));
        connect(delTitle, SIGNAL(triggered()), this, SLOT(delTitle()));
        menu.addAction(delTitle);
    }
    QPoint pt(pos);
    menu.exec(searchItemTree->mapToGlobal(pos) );
}
//! [9]
void MainWindow::treeStateChanged(QTreeWidgetItem* item,int index)
{
    qDebug()<<item->text(0)<<"index"<<index;
    QString tagName = item->data(0, Qt::UserRole).toString();
    if(tagName == "folder")
    {
        if(item->checkState(0) == Qt::Checked)
        {
            for (int i = 0; i < item->childCount(); ++i)
            {
                setitemCheckState(item->child(i), Qt::Checked);
            }
        }else
        {
            for (int i = 0; i < item->childCount(); ++i)
            {
                setitemCheckState(item->child(i), Qt::Unchecked);
            }
        }
    }
    updateFindText();
}
void MainWindow::setitemCheckState(QTreeWidgetItem* item, Qt::CheckState state)
{
    item->setCheckState(0, state);
    for (int i = 0; i < item->childCount(); ++i)
    {
        setitemCheckState(item->child(i), state);
    }
}
void MainWindow::updateFindText()
{
    QStringList selectedText;
    for (int i = 0; i < searchItemTree->topLevelItemCount(); ++i)
    {
        getSelText(searchItemTree->topLevelItem(i), selectedText);
    }
    textEdit->ui_lineEdit->clear();
    textEdit->keyword_list.clear();
    for(int i=0; i<selectedText.length();i++)
    {
        textEdit->ui_lineEdit->insert(selectedText[i]);
        textEdit->keyword_list.push_back(selectedText[i]);
        textEdit->ui_lineEdit->insert(QString(";"));
    }
}

void MainWindow::getSelText(QTreeWidgetItem* item, QStringList& selText)
{
    QString tagName = item->data(0, Qt::UserRole).toString();
    if (tagName == "folder") {
        for (int i = 0; i < item->childCount(); ++i)
            getSelText(item->child(i), selText);
    } else if (tagName == "title") {
        if(item->checkState(0) == Qt::Checked)
        {
            selText.append(item->text(0));
        }
    }
}
