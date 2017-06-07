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

#include <QDateTime>

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
//static int n_buffer = 0;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);


       QPixmap image_init;
    image_init.load("/work/init.png");
     ui->label_show->setPixmap(image_init);
        cmd_pro = new QProcess;
     //system("/work/clean.sh");
       // cmd_pro->start("/work/clean.sh");


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
        timer ->start(66);


        timer_up = new QTimer(this);
        connect(timer_up,SIGNAL(timeout()),this,SLOT(up_image()));
        timer_up ->start(1000*60*10);  //10min

        save_flag = 0;
        fp = NULL;


}

void MainWindow::flushBuff()
{
    printf("read_frame \n");

    read_frame (&pd);

    if (!bufrgb) {
        //bufrgb = (unsigned char *)malloc(320*240*3);
    }
    //memset(bufrgb,0,320*240*3);



    image_my.loadFromData((const uchar *)pd.buffers[pd.buf.index].start,pd.buffers[pd.buf.index].length);
    ui->label_show->setPixmap(QPixmap::fromImage(image_my));



   if(save_flag){

        save_flag = 0;
        date = QDateTime::currentDateTime();
        QString str = date.toString("dd_MM_yyyy_h_m_s");

        str+=".jpg";

        QByteArray ba = str.toLatin1();
        char * jname=NULL;
        char * ch_cmd = NULL;
        QString str_cmd;

        jname = ba.data();
        fp = fopen(jname,"w");
        fwrite((unsigned char *)pd.buffers[pd.buf.index].start,1,pd.buffers[pd.buf.index].length,fp);

        sync();

        fclose(fp);


        str_cmd = "tftp -l " +str +" -p 10.19.3.0";
        ba = str_cmd.toLatin1();
        ch_cmd = ba.data();
        cmd_pro->start(ch_cmd);



        str.insert(0,"save as ");

         //QMessageBox::information(this,tr("Tip"),str);
   }


    return_data(&pd);
}




void MainWindow::show_img()
{



}

 void MainWindow::up_image(void)
 {

   save_flag = 1;


 }



void MainWindow::on_pushButton_save_clicked()
{

    save_flag = 1;

}
