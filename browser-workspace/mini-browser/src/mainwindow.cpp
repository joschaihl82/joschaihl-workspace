#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QToolBar>
#include <QWebEngineView>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    webView = new QWebEngineView(this);
    setCentralWidget(webView);


    searchLineEdit = new QLineEdit(this);
    searchButton = new QPushButton("Search", this);


    engineComboBox = new QComboBox(this);
    engineComboBox->addItem("Google");
    engineComboBox->addItem("Bing");
    engineComboBox->addItem("DuckDuckGo");


    QToolBar *toolBar = addToolBar("Navigation");
    toolBar->addWidget(engineComboBox);
    toolBar->addWidget(searchLineEdit);
    toolBar->addWidget(searchButton);

    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);

    onLoadHomePage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSearchClicked()
{
    QString searchText = searchLineEdit->text();
    QString selectedEngine = engineComboBox->currentText();
    QString searchUrl;

    if (selectedEngine == "Google") {
        searchUrl = "https://www.google.com/search?q=" + searchText;
    } else if (selectedEngine == "Bing") {
        searchUrl = "https://www.bing.com/search?q=" + searchText;
    } else if (selectedEngine == "DuckDuckGo") {
        searchUrl = "https://duckduckgo.com/?q=" + searchText;
    }

    webView->load(QUrl(searchUrl));
}

void MainWindow::onLoadHomePage()
{
    webView->load(QUrl("https://www.google.com"));
}
