#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QList>
#include <QPushButton>
#include <QGraphicsProxyWidget>

struct obstacle {
    QGraphicsEllipseItem *circle;
    bool checked = false;
    bool success = false;
    double tolerance = 30.0;
    obstacle(QGraphicsEllipseItem *c) : circle(c) {}
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    QGraphicsPixmapItem *ball;
    QGraphicsPixmapItem *bg1;
    QGraphicsPixmapItem *bg2;

    QList<obstacle*> obstacles;
    QTimer *timer;
    QTimer *spawnTimer;

    double velocity;
    int score;
    bool gamestart;
    double bottomLimit;

    int m_userId = -1;
    QString m_username;
    int m_bestScore = 0;

    QGraphicsTextItem *scoreText;
    QGraphicsTextItem *bestScoreText;
    QGraphicsTextItem *titleText;
    QGraphicsTextItem *loginMsg;
    QGraphicsRectItem *loginOverlay;


    QList<QGraphicsProxyWidget*> authProxies;
    QList<QGraphicsItem*> authItems;

    bool isPaused;
    QPushButton *btnPause;
    QGraphicsProxyWidget *proxyBtnPause;

    void showLoginScreen();
    void showRegisterScreen();
    void hideAuthScreen();
};

#endif