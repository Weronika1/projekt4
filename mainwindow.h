#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <qgraphicsitem>
#include <QTimer>

#define ILOSC_ELEMENTOW 4

struct element
{
    QGraphicsSimpleTextItem * napis;
    QGraphicsItem *wsk;
    bool czy_zlapany;
    int masa;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void UstawRamie( QPoint punkt );
    void Odswiez();

    void keyPressEvent( QKeyEvent * zdarzenie );

    void zrob_klocki();
    void zlap();
    bool czy_spadl_na_klocek(element klocek);

public slots:
    void spadanie( void );


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scena;

    QPoint punkt1, punkt2, punkt3;

    QTimer timer;
    element klocki[ILOSC_ELEMENTOW];
    int trzymany;

    QPoint offset;

    QRectF zakazane_pole;
};



#endif // MAINWINDOW_H
