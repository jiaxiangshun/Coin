#include "playscene.h"
#include<QDebug>
#include<QMenuBar>
#include<QPainter>
#include "mypushbutton.h"
#include<QTimer>
#include<QLabel>
#include "mycoin.h"
#include "dataconfig.h"
#include<QPropertyAnimation>
#include<QSound>

//PlayScene::PlayScene(QWidget *parent) : QMainWindow(parent)
//{

//}
PlayScene::PlayScene(int levelNum)
{
    QString str = QString("进入了 %1 关").arg(levelNum);
    qDebug()<<str;
    this->levelIndex = levelNum;
    //初始化游戏场景
    //设置固定大小
    setFixedSize(320,588);

    //设置图标
    setWindowIcon(QIcon(":/res/Coin0001.png"));
    //设置标题
    setWindowTitle("游戏场景");
    //创建菜单栏
    QMenuBar * bar = menuBar();
    setMenuBar(bar);
    //创建开始菜单
    QMenu * startMenu = bar->addMenu("开始");
    //创建退出菜单栏
    QAction * quitAction = startMenu->addAction("退出");
    //点击退出，实现游戏退出
    connect(quitAction,&QAction::triggered,[=](){
        this->close();
    });

    //添加音效资源
    //返回按钮音效
    QSound *backSound = new QSound(":/res/BackButtonSound.wav",this);

    //翻金币音效
    QSound *flipSound = new QSound(":/res/ConFlipSound.wav",this);
    //胜利按钮音效
    QSound *winSound = new QSound(":/res/LevelWinSound.wav",this);

    //返回按钮
    MyPushButton * backBtn  = new MyPushButton(":/res/BackButton.png",":/res/BackButtonSelected.png");
    backBtn->setParent(this);
    backBtn->move(this->width() - backBtn->width(),this->height()-backBtn->height());

    //点击返回
    connect(backBtn,&QPushButton::clicked,[=](){
        qDebug()<<"游戏场景中点击了返回按钮";
        //播放返回音效
        backSound->play();
        //告诉主场景 我返回了，选择关卡场景监听playSence的返回按钮
        QTimer::singleShot(500,this,[=](){
            emit this->playSceneBack();
        });
    });


    //显示当前关卡数
    QLabel *label = new QLabel;
    label->setParent(this);
    QFont font;
    font.setFamily("华文新魏");
    font.setPointSize(15);
    QString str1 = QString("Level: %1 ").arg(this->levelIndex);
    //将字体设置到标签控件中
    label->setFont(font);
    label->setText(str1);
    label->setGeometry(30,this->height() -50,120,50);

    //初始化每个关卡的二维数组
    dataConfig config;
    for(int i=0;i<4;i++)
       {
           for(int j=0;j<4;j++)
           {
              this->gameArray[i][j] = config.mData[this->levelIndex][i][j];
           }
           qDebug()<<"";
       }

    //胜利图片显示
    QLabel * winlabel = new QLabel;
    QPixmap tmpPix;
    tmpPix.load(":/res/LevelCompletedDialogBg.png");
    winlabel->setGeometry(0,0,tmpPix.width(),tmpPix.height());
    winlabel->setPixmap(tmpPix);
    winlabel->setParent(this);
    winlabel->move((this->width() - tmpPix.width())* 0.5 , -tmpPix.height());//y坐标为负，是为了准备从上向下降落


    //显示金币背景图案
    for(int i= 0;i<4;i++)
    {
        for(int j= 0;j<4;j++)
        {
            QPixmap pix = QPixmap(":/res/BoardNode(1).png");
            QLabel * label =new QLabel;
            label->setGeometry(0,0,pix.width(),pix.height());
            label->setPixmap(pix);
            label->setParent(this);
            label->move(57 + i*50,200 + j*50);

            //创建金币
            QString str;
            if(this->gameArray[i][j] == 1)
            {
                //显示金币
                str = ":/res/Coin0001.png";
            }
            else
            {

                //显示银币
                str = ":/res/Coin0008.png";
            }
            MyCoin *coin = new MyCoin(str);
            coin->setParent(this);
            coin->move(59 + i*50,204 + j*50);

            //给金币属性赋值
            coin->posX = i;
            coin->posY = j;
            coin->flag = this->gameArray[i][j];
            //将金币放入到金币的二维数组里 以便后期的维护
            coinBtn[i][j] = coin;

            //点击金币 进行翻转
            connect(coin,&MyCoin::clicked,[=](){
                //播放翻金币音效
                flipSound->play();                //点击按钮时，将所有按钮全部禁用
                for(int i= 0;i<4;i++)
                {
                    for(int j= 0;j<4;j++)
                    {
                       coinBtn[i][j]->isWin = true;
                    }
                }


                coin->changFlag();
                this->gameArray[i][j] = this->gameArray[i][j] == 0 ? 1: 0;

                //翻转周围金币 + 延时翻转
                QTimer::singleShot(300,this,[=](){
                    if(coin->posX + 1 <=3)//周围的右侧金币翻转的条件
                    {
                        coinBtn[coin->posX+1][coin->posY]->changFlag();
                        this->gameArray[coin->posX+1][coin->posY] = this->gameArray[coin->posX+1][coin->posY] == 0 ? 1: 0;

                    }
                    if(coin->posX - 1 >=0)//周围的左侧金币翻转的条件
                    {
                        coinBtn[coin->posX-1][coin->posY]->changFlag();
                        this->gameArray[coin->posX-1][coin->posY] = this->gameArray[coin->posX-1][coin->posY] == 0 ? 1: 0;

                    }
                    if(coin->posY + 1 <=3)//周围的下侧金币翻转的条件
                    {
                        coinBtn[coin->posX][coin->posY+1]->changFlag();
                        this->gameArray[coin->posX][coin->posY+1] = this->gameArray[coin->posX][coin->posY+1] == 0 ? 1: 0;

                    }
                    if(coin->posY - 1>=0)//周围的上侧金币翻转的条件
                    {
                        coinBtn[coin->posX][coin->posY-1]->changFlag();
                        this->gameArray[coin->posX][coin->posY-1] = this->gameArray[coin->posX][coin->posY-1] == 0 ? 1: 0;
                    }
                    //翻完周围金币后，解开禁用操作
                    for(int i= 0;i<4;i++)
                    {
                        for(int j= 0;j<4;j++)
                        {
                           coinBtn[i][j]->isWin = false;
                        }
                    }

                    //判断是否胜利
                    this->isWin = true;
                    for(int i= 0;i<4;i++)
                    {
                        for(int j= 0;j<4;j++)
                        {
                            if(coinBtn[i][j]->flag == false)//只要有一个是反面，那就是失败
                            {
                                this->isWin = false;
                                break;
                            }
                        }
                    }
                    if(this->isWin == true)
                    {
                        //胜利了
                        //播放胜利音效
                        winSound->play();
                        qDebug()<<"游戏胜利了";
                        //将所有按钮的胜利标志改为true;如果再次点击按钮 ，直接return 不做响应
                        for(int i= 0;i<4;i++)
                        {
                            for(int j= 0;j<4;j++)
                            {
                               coinBtn[i][j]->isWin = true;
                            }
                        }
                        //将胜利图片移动下来
                        QPropertyAnimation *animation = new QPropertyAnimation(winlabel,"geometry");
                        //设置时间间隔
                        animation->setDuration(1000);
                        //设置开始位置
                        animation->setStartValue(QRect(winlabel->x(),winlabel->y(),winlabel->width(),winlabel->height()));

                        //设置结束位置
                        animation->setEndValue(QRect(winlabel->x(),winlabel->y()+114,winlabel->width(),winlabel->height()));
                        //设置缓和曲线
                        animation->setEasingCurve(QEasingCurve::OutBounce);
                        //执行动画
                        animation->start();
                    }
                });
            });
        }
    }

}

void PlayScene::paintEvent(QPaintEvent *)
{
    //加载背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/PlayLevelSceneBg.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);


    //加载标题
    pix.load(":/res/Title.png");
    // 图片缩放
    pix = pix.scaled(pix.width() *0.5,pix.height() * 0.5);
    painter.drawPixmap(10,30,pix.width(),pix.height(),pix);

}

