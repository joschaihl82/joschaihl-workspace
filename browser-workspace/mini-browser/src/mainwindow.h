#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLineEdit;
class QPushButton;
class QComboBox;
class QWebEngineView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchClicked();
    void onLoadHomePage();

private:
    Ui::MainWindow *ui;
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QComboBox *engineComboBox;
    QWebEngineView *webView;
};
#endif // MAINWINDOW_H
