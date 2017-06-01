VideoDisplay::VideoDisplay(QWidget *parent)  
    : QDialog(parent)  
    , bufrgb(NULL)  
{  
    ui.setupUi(this);  
    connect(ui.beginButton, SIGNAL(clicked()), this, SLOT(beginCapture()));  
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(savebmpData()));  
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(reject()));  
  
    timer = new QTimer(this);  
    timer->setInterval(10);  
    connect(timer, SIGNAL(timeout()), this, SLOT(flushBuff()));  
  
    pd.dev_name = "/dev/video0";  
}  
  
VideoDisplay::~VideoDisplay()  
{  
    if (timer->isActive()) {  
        timer->stop();  
    }  
}  
  
void VideoDisplay::beginCapture()  
{  
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
    timer->start();  
    ui.beginButton->setDisabled(TRUE);  
}  
  
void VideoDisplay::flushBuff()  
{  
    read_frame (&pd);  
    if (!bufrgb) {  
        bufrgb = (unsigned char *)malloc(640 * 480* 3);  
    }  
    memset(bufrgb, 0, 640 * 480* 3);  
  
    convert_yuv_to_rgb_buffer((unsigned char *)pd.buffers[pd.buf.index].start,bufrgb,640,480);  
    QImage image(bufrgb,640,480,QImage::Format_RGB888);  
    ui.displayLabel->setPixmap(QPixmap::fromImage(image));  
    return_data(&pd);  
}  
  
void VideoDisplay::savebmpData()  
{  
    FILE *fp;  
    time_t now;  
    struct tm *tm_time;  
    time(&now);  
  
    if (bufrgb > 0 && strlen((char *)bufrgb) > 0) {  
        tm_time = localtime(&now);  
        char filename[30] = {0};  
        sprintf(filename,"%4d-%02d-%02d_%02d.%02d.%02d.png",1900+tm_time->tm_year,1+tm_time->tm_mon,tm_time->tm_mday,  
                tm_time->tm_hour,tm_time->tm_min,tm_time->tm_sec);  
       QImage image(bufrgb,640,480,QImage::Format_RGB888);  
       image.save(filename);  
    }  
}  