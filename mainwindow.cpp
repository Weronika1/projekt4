#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <ctime>
#include <cstdlib>

#define L1 250
#define L2 250

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scena = new QGraphicsScene(0,0, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->setScene(scena);

    punkt1 = QPoint(scena->width()/2, scena->height()/2);
    punkt3 = punkt1 + QPoint(50,-50);

    connect(&timer, SIGNAL(timeout()), this, SLOT(spadanie()));
    timer.start(3);

    zakazane_pole = QRect(100, 100, 200, 200);
    trzymany = -1;

    zrob_klocki();
    UstawRamie(punkt3); // wylicza punkt2
    Odswiez();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete scena;
}

void MainWindow::UstawRamie(QPoint punkt)
{
    if(zakazane_pole.contains(punkt)) return;

    int x = punkt.x(), y = punkt.y();
    x -= punkt1.x();
    y = punkt1.y() - y;
    double odleglosc1_3 = sqrt(x*x + y*y);

    if(odleglosc1_3 > L1+L2) return;
    double coskat1 = (L1*L1 + L2*L2 - odleglosc1_3*odleglosc1_3)/(2*L1*L2); // cos kata miedzy ramionami
    double kat1 = acos(coskat1);
    double kat2 = atan2(y, x);
    double sinkat3 = (sin(kat1)*L1)/odleglosc1_3;
    double kat3 = asin(sinkat3);
    double kat23 = kat2+kat3;

    punkt2.setX(L1*cos(kat23));
    punkt2.setY(L1*sin(kat23));

    punkt2.setX(punkt2.x()+punkt1.x());
    punkt2.setY(punkt1.y()-punkt2.y());

    punkt3 = punkt;

    if(trzymany > -1)
    {
        klocki[trzymany].wsk->setPos(QPointF(punkt3) + offset);
        klocki[trzymany].napis->setPos(klocki[trzymany].wsk->pos() + QPointF(10, 10));
    }
}

void MainWindow::Odswiez()
{
    for(int i = 0; i < ILOSC_ELEMENTOW; i++)
    {
        scena->removeItem(klocki[i].wsk);
        scena->removeItem(klocki[i].napis);
    }
    scena->clear();

    for(int i = 0; i < ILOSC_ELEMENTOW; i++)
    {
        scena->addItem(klocki[i].wsk);
        scena->addItem(klocki[i].napis);
    }
    scena->addLine(punkt1.x(),punkt1.y(),punkt2.x(),punkt2.y(), QPen(Qt::red));
    scena->addLine(punkt3.x(),punkt3.y(),punkt2.x(),punkt2.y(), QPen(Qt::red));
    scena->addRect(zakazane_pole);
}

void MainWindow::keyPressEvent(QKeyEvent *zdarzenie)
{
    int p=ui->predkosc->value();
    switch(zdarzenie->key())
    {
        case Qt::Key_Up:
            UstawRamie(punkt3 + QPoint(0, -p));
            break;

        case Qt::Key_Down:
            UstawRamie(punkt3 + QPoint(0, p));
            break;

        case Qt::Key_Left:
            UstawRamie(punkt3 + QPoint(-p, 0));
            break;

        case Qt::Key_Right:
            UstawRamie(punkt3 + QPoint(p, 0));
            break;

        case Qt::Key_Return:
            zlap();
            break;
    }
    Odswiez();
}

void MainWindow::zrob_klocki()
{
    srand(time(NULL));
    QPixmap obrazek(":/klocek.png");
    for(int i = 0; i < ILOSC_ELEMENTOW; i++)
    {
        klocki[i].czy_zlapany = 0;
        do
        {
            klocki[i].masa = rand() % 10 + 1;
        }while(czy_taka_sama_masa(i, klocki[i].masa));

        klocki[i].wsk = scena->addPixmap(obrazek);
        klocki[i].wsk->setPos(100 + 100*i, 500);

        klocki[i].napis=scena->addSimpleText(QString::number( klocki[i].masa));
        klocki[i].napis->setPos(klocki[i].wsk->pos() + QPointF(10, 10));
    }
}

void MainWindow::zlap()
{
    QGraphicsItem *wsk = scena->itemAt(QPointF(punkt3)+QPointF(5, 5), QTransform());
    if(trzymany > -1)
    {
        klocki[trzymany].czy_zlapany = 0;
        trzymany = -1;
        Odswiez();
        return;
    }

    if(wsk != NULL)
    {
        for(int i = 0; i < ILOSC_ELEMENTOW; i++)
        {
            if(wsk == klocki[i].wsk || wsk == klocki[i].napis)
            {
                if(i != trzymany)
                {
                    offset = wsk->pos().toPoint()-punkt3;
                    klocki[i].czy_zlapany = 1;
                    trzymany = i;
                    Odswiez();
                    return;
                }
            }
        }
    }

}

bool MainWindow::czy_spadl_na_klocek(element klocek)
{
    QRectF kl(klocek.wsk->pos()+QPointF(0,1), klocek.wsk->boundingRect().size());

    for(int i = 0; i < ILOSC_ELEMENTOW; i++)
    {
        if(klocek.wsk == klocki[i].wsk) continue;

        QRectF ograniczenie(klocki[i].wsk->pos(), klocki[i].wsk->boundingRect().size());

        if( ograniczenie.contains(kl.topLeft()) ||
            ograniczenie.contains(kl.topRight()) ||
            ograniczenie.contains(kl.bottomLeft()) ||
            ograniczenie.contains(kl.bottomRight()))
            return 1;
    }
    return 0;
}

bool MainWindow::czy_taka_sama_masa(int nr_klocka, int masa)
{
    int waga;
    if (nr_klocka == 0) return 0;
    for(int i = 0; i < nr_klocka; i++)
    {
        if(klocki[i].masa == masa) return 1;
    }
    return 0;
}

void MainWindow::spadanie()
{
    this->setFocus(Qt::ActiveWindowFocusReason);

    for(int i = 0; i < ILOSC_ELEMENTOW; i++)
    {
        if(klocki[i].czy_zlapany) continue;

        if(scena->height() > klocki[i].wsk->boundingRect().height() + klocki[i].wsk->pos().y()) //sprawdza czy element lezy na podlodze
        {
            if(czy_spadl_na_klocek(klocki[i])) continue;
            klocki[i].wsk->moveBy(0, 1);
            klocki[i].napis->setPos(klocki[i].wsk->pos() + QPointF(10, 10));
        }
    }
    Odswiez();
}
