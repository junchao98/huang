#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QMessageBox>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <assert.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

extern "C" {

#include "v4l2.h"
#include "jpeglib.h"
}


//照相机延时等待
#define TimeOut 5
//拍照 个数
#define CapNum 10
//设置照片宽度 高度
#define CapWidth 160
#define CapHeight 120
//申请Buf个数
#define ReqButNum 4
//使用前置或者后置Camera 前置设0，后置设1
#define IsRearCamera 0
//设置帧率
#define  FPS 10
//设置格式
#define PIXELFMT V4L2_PIX_FMT_YUYV

#define CapDelay 0*500*1000

#define CLEAR(x)    memset(&(x), 0, sizeof(x))

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;

typedef struct
{
    void *start;
    int length;
}BUFTYPE;


BUFTYPE *user_buf;
static int n_buffer = 0;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{


    pd.dev_name = "/dev/video0";

    int flag = init_dev(&pd);

    if (flag == -1) {
            QMessageBox::information(this,tr("Tip"),tr("no device"));
            exit(1);
        }
        else if (flag == -2) {
            QMessageBox::information(this,tr("Tip"),tr("device is wrong"));
            exit(2);
        }
        else if (flag == -3) {
            QMessageBox::information(this,tr("Tip"),tr("can not open device"));
            exit(3);
        }


        timer = new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(flushBuff()));
        timer ->start(2000);


       // QPixmap image_end;
        QImage image;
       ui->label_show->clear();

        //image.load("/work/e.png");
       //image_end.load("/work/e.jpg");

        //ui->label_show->setPixmap(QPixmap::fromImage(image));

        //ui->label_show->show();

        printf("butt");

}

void MainWindow::flushBuff()
{
    printf("read_frame \n");

    read_frame (&pd);

    if (!bufrgb) {
        bufrgb = (unsigned char *)malloc(320*240*3);
    }
    memset(bufrgb,0,320*240*3);

    convert_yuv_to_rgb_buffer((unsigned char *)pd.buffers[pd.buf.index].start,bufrgb,320,240);

    QImage image_my(bufrgb,320,340,QImage::Format_RGB888);
    ui->label_show->setPixmap(QPixmap::fromImage(image_my));

/*
    QImage image_my;
    image_my.loadFromData((const uchar *)pd.buffers[pd.buf.index].start,pd.buffers[pd.buf.index].length,"JPG");
    ui->label_show->setPixmap(QPixmap::fromImage(image_my));
*/


    FILE *fp = NULL;
    //static int tt = 1;
/*
   if(tt){
        tt--;
        fp = fopen("e.jpg","w");
        fwrite((unsigned char *)pd.buffers[pd.buf.index].start,1,pd.buffers[pd.buf.index].length,fp);

        sync();

        fclose(fp);
   }
   */





    return_data(&pd);
}




void MainWindow::show_img()
{



}


