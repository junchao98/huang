#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern "C" {

#include "v4l2.h"

}



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
     pass_data pd;
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
     void show_img();
     void flushBuff();
private:
    Ui::MainWindow *ui;

    void init_cam(int in_fd);
    int init_mmap(int cam_fd);
    //int read_frame(int cam_fd);
    int process_image(void *addr,int length) ;

    unsigned char *bufrgb;

    QTimer *timer;

};

#endif // MAINWINDOW_H
