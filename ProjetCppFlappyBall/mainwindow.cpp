#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QPen>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Database::instance().init();

    velocity  = 0.0;
    score     = 0;
    gamestart = false;
    isPaused  = false;

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPixmap px(":/imgs/Combined background - Blue.png");
    int h = 600;
    int w = px.width() * h / px.height();
    bg1 = scene->addPixmap(px.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    bg2 = scene->addPixmap(px.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    bg1->setPos(0, 0);  bg2->setPos(w, 0);
    bg1->setZValue(-1); bg2->setZValue(-1);

    QPixmap ballimg(":/imgs/basketball.png");
    ball = scene->addPixmap(ballimg.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ball->setPos(60, 60);
    ball->setZValue(1);

    scoreText = scene->addText("Score: 0");
    scoreText->setDefaultTextColor(Qt::black);
    scoreText->setFont(QFont("Arial", 16, QFont::Bold));
    scoreText->setPos(10, 10);
    scoreText->setZValue(1);

    bestScoreText = scene->addText("Best: 0");
    bestScoreText->setDefaultTextColor(Qt::darkGreen);
    bestScoreText->setFont(QFont("Arial", 14, QFont::Bold));
    bestScoreText->setPos(10, 35);
    bestScoreText->setZValue(1);

    btnPause = new QPushButton("⏸️");
    btnPause->setFixedSize(40, 40);
    btnPause->setFocusPolicy(Qt::NoFocus);

    btnPause->setStyleSheet(
        "background: transparent; color: white; font-size: 16px;"
        "border: 2px solid orange; border-radius: 20px; font-weight: bold;");

    proxyBtnPause = scene->addWidget(btnPause);
    proxyBtnPause->setZValue(100);
    proxyBtnPause->hide();

    connect(btnPause, &QPushButton::clicked, [this]() {
        if (!gamestart) return;

        if (!isPaused) {
            isPaused = true;
            timer->stop();
            spawnTimer->stop();
            btnPause->setText("▶️");
        } else {
            isPaused = false;
            timer->start(10);
            spawnTimer->start(1500);
            btnPause->setText("⏸️");
        }
        this->setFocus();
    });

    timer      = new QTimer(this);
    spawnTimer = new QTimer(this);

    auto triggerGameOver = [this]() {
        if (!gamestart) return;
        gamestart = false;
        isPaused = false;

        timer->stop();
        spawnTimer->stop();
        proxyBtnPause->hide();
        Database::instance().saveProgress(m_userId, score);

        double sw = scene->sceneRect().width();
        double sh = scene->sceneRect().height();

        QGraphicsRectItem *goOverlay = scene->addRect(sw/2 - 160, sh/2 - 170, 320, 330,
                                                      QPen(QColor(255,165,0), 2),
                                                      QBrush(QColor(0, 0, 0, 210)));
        goOverlay->setZValue(10);
        goOverlay->setData(0, "gameover_element");

        QGraphicsTextItem *goTitle = scene->addText("❌ GAME OVER");
        goTitle->setDefaultTextColor(Qt::red);
        goTitle->setFont(QFont("Arial", 18, QFont::Bold));
        goTitle->setPos(sw/2 - 95, sh/2 - 150);
        goTitle->setZValue(11);
        goTitle->setData(0, "gameover_element");

        QGraphicsTextItem *currentUserScore = scene->addText("Joueur: " + m_username + "\nScore: " + QString::number(score));
        currentUserScore->setDefaultTextColor(Qt::white);
        currentUserScore->setFont(QFont("Arial", 11, QFont::Bold));
        currentUserScore->setPos(sw/2 - 130, sh/2 - 105);
        currentUserScore->setZValue(11);
        currentUserScore->setData(0, "gameover_element");

        auto board = Database::instance().getLeaderboard();
        QString boardStr = "🏆 TOP 3 MEILLEURS\n\n";
        int count = 0;
        for (auto &[name, sc] : board) {
            if (count >= 3) break;
            QString medaille = (count == 0) ? "🥇 " : (count == 1) ? "🥈 " : "🥉 ";
            boardStr += medaille + name + "  -  " + QString::number(sc) + " pts\n";
            count++;
        }

        QGraphicsTextItem *goBoard = scene->addText(boardStr);
        goBoard->setDefaultTextColor(QColor(255, 200, 100));
        goBoard->setFont(QFont("Courier New", 11, QFont::Bold));
        goBoard->setPos(sw/2 - 130, sh/2 - 55);
        goBoard->setZValue(11);
        goBoard->setData(0, "gameover_element");

        QPushButton *btnPlayAgain = new QPushButton("Play Again");
        btnPlayAgain->setFixedSize(125, 36);
        btnPlayAgain->setStyleSheet("background: #2ecc71; color: white; font-weight: bold; border-radius: 6px;");
        QGraphicsProxyWidget *proxyBtnAgain = scene->addWidget(btnPlayAgain);
        proxyBtnAgain->setPos(sw/2 - 135, sh/2 + 105);
        proxyBtnAgain->setZValue(11);
        proxyBtnAgain->setData(0, "gameover_element_proxy");

        QPushButton *btnLogOut = new QPushButton("Log Out");
        btnLogOut->setFixedSize(125, 36);
        btnLogOut->setStyleSheet("background: #c0392b; color: white; font-weight: bold; border-radius: 6px;");
        QGraphicsProxyWidget *proxyBtnLogOut = scene->addWidget(btnLogOut);
        proxyBtnLogOut->setPos(sw/2 + 10, sh/2 + 105);
        proxyBtnLogOut->setZValue(11);
        proxyBtnLogOut->setData(0, "gameover_element_proxy");

        connect(btnPlayAgain, &QPushButton::clicked, [this, proxyBtnAgain, proxyBtnLogOut]() {
            for (QGraphicsItem *item : scene->items()) {
                if (item->data(0).toString() == "hint_text") { scene->removeItem(item); delete item; }
            }
            QList<QGraphicsItem*> toDelete;
            for (QGraphicsItem *item : scene->items()) {
                if (item->data(0).toString() == "gameover_element") toDelete.append(item);
            }
            for (QGraphicsItem *item : toDelete) { scene->removeItem(item); delete item; }

            scene->removeItem(proxyBtnAgain);  proxyBtnAgain->deleteLater();
            scene->removeItem(proxyBtnLogOut); proxyBtnLogOut->deleteLater();

            for (obstacle *obs : obstacles) { scene->removeItem(obs->circle); delete obs->circle; delete obs; }
            obstacles.clear();

            score = 0; velocity = 0.0; gamestart = false; isPaused = false;
            btnPause->setText("⏸️"); scoreText->setPlainText("Score: 0"); ball->setPos(60, 60);
            bg1->setPos(0, 0); bg2->setPos(bg1->pixmap().width(), 0);

            QGraphicsTextItem *hint = scene->addText("Appuie sur ESPACE pour jouer !");
            hint->setDefaultTextColor(Qt::black); hint->setFont(QFont("Arial", 11, QFont::Bold));
            hint->setZValue(3); hint->setData(0, "hint_text");
            hint->setPos(scene->sceneRect().width() / 2 - hint->boundingRect().width() / 2,
                         scene->sceneRect().height() / 2 - hint->boundingRect().height() / 2);

            proxyBtnPause->setPos(230, 10);
            proxyBtnPause->show();
            this->setFocus();
            timer->start(10);
        });

        connect(btnLogOut, &QPushButton::clicked, [this, proxyBtnAgain, proxyBtnLogOut]() {
            QList<QGraphicsItem*> toDelete;
            for (QGraphicsItem *item : scene->items()) {
                if (item->data(0).toString() == "gameover_element") toDelete.append(item);
            }
            for (QGraphicsItem *item : toDelete) { scene->removeItem(item); delete item; }
            scene->removeItem(proxyBtnAgain); proxyBtnAgain->deleteLater();
            scene->removeItem(proxyBtnLogOut); proxyBtnLogOut->deleteLater();

            for (obstacle *obs : obstacles) { scene->removeItem(obs->circle); delete obs->circle; delete obs; }
            obstacles.clear();

            m_userId = -1; m_username = ""; m_bestScore = 0; score = 0; velocity = 0.0; gamestart = false; isPaused = false;
            scoreText->setPlainText("Score: 0"); bestScoreText->setPlainText("Best: 0"); ball->setPos(60, 60);
            bg1->setPos(0, 0); bg2->setPos(bg1->pixmap().width(), 0);

            showLoginScreen();
        });
    };

    connect(timer, &QTimer::timeout, [this, triggerGameOver]() {
        if (!gamestart) return;
        double gameSpeed = 2.0 + (score / 100.0);
        double gravity = 0.09 + ((score / 100.0) * 0.015);

        bottomLimit = ui->graphicsView->height() - 45;
        velocity += gravity;
        ball->moveBy(0, velocity);

        if (ball->y() >= bottomLimit) { ball->setY(bottomLimit); velocity = 0.0; triggerGameOver(); return; }
        if (ball->y() < 0) { ball->setY(0); velocity = 0.0; }

        for (int i = 0; i < obstacles.size(); i++) {
            obstacle *obs = obstacles[i];
            obs->circle->moveBy(-gameSpeed, 0);
            QRectF ballRect = ball->sceneBoundingRect();
            QRectF circleRect = obs->circle->sceneBoundingRect();

            if (ballRect.center().x() > circleRect.left() && ballRect.center().x() < circleRect.right() &&
                qAbs(ballRect.center().y() - circleRect.center().y()) < obs->tolerance) {
                obs->success = true;
            }

            if (!obs->checked && circleRect.right() < ballRect.left()) {
                obs->checked = true;
                if (!obs->success) { triggerGameOver(); return; }
                score += 10;
                scoreText->setPlainText("Score: " + QString::number(score));
                Database::instance().saveProgress(m_userId, score);
                if (score > m_bestScore) { m_bestScore = score; bestScoreText->setPlainText("Best: " + QString::number(m_bestScore)); }
            }

            if (obs->circle->x() < -500) { scene->removeItem(obs->circle); delete obs->circle; delete obs; obstacles.removeAt(i); i--; }
        }

        int bw = bg1->pixmap().width();
        bg1->moveBy(-gameSpeed, 0); bg2->moveBy(-gameSpeed, 0);
        if (bg1->x() <= -bw) bg1->setX(bg2->x() + bw);
        if (bg2->x() <= -bw) bg2->setX(bg1->x() + bw);
    });

    connect(spawnTimer, &QTimer::timeout, [this]() {
        if (!gamestart) return;
        auto ellipse = scene->addEllipse(scene->sceneRect().width(), 100 + rand() % 300, 100, 40, QPen(Qt::red, 4), Qt::NoBrush);
        ellipse->setZValue(10);
        obstacles.append(new obstacle(ellipse));
    });

    timer->start(10);
    showLoginScreen();
}

void MainWindow::showLoginScreen()
{
    hideAuthScreen();
    double sw = scene->sceneRect().width();
    double sh = scene->sceneRect().height();

    loginOverlay = scene->addRect(sw/2 - 160, sh/2 - 140, 320, 270, QPen(QColor(255,165,0), 2), QBrush(QColor(0,0,0,190)));
    loginOverlay->setZValue(5); authItems.append(loginOverlay);

    titleText = scene->addText("🔐 CONNEXION");
    titleText->setDefaultTextColor(QColor(255, 165, 0)); titleText->setFont(QFont("Arial", 16, QFont::Bold));
    titleText->setPos(sw/2 - 90, sh/2 - 125); titleText->setZValue(6); authItems.append(titleText);

    loginMsg = scene->addText("");
    loginMsg->setDefaultTextColor(Qt::red); loginMsg->setFont(QFont("Arial", 10));
    loginMsg->setPos(sw/2 - 140, sh/2 + 85); loginMsg->setZValue(6); authItems.append(loginMsg);

    QString styleFields = "background: rgba(255,255,255,220); border: 2px solid orange; border-radius: 6px; padding: 4px; font-size: 14px;";

    auto *edtUser = new QLineEdit(); edtUser->setPlaceholderText("Nom d'utilisateur"); edtUser->setFixedSize(240, 36); edtUser->setStyleSheet(styleFields);
    auto *pUser = scene->addWidget(edtUser); pUser->setPos(sw/2 - 120, sh/2 - 80); pUser->setZValue(6); authProxies.append(pUser);

    auto *edtPass = new QLineEdit(); edtPass->setPlaceholderText("Mot de passe"); edtPass->setEchoMode(QLineEdit::Password); edtPass->setFixedSize(240, 36); edtPass->setStyleSheet(styleFields);
    auto *pPass = scene->addWidget(edtPass); pPass->setPos(sw/2 - 120, sh/2 - 30); pPass->setZValue(6); authProxies.append(pPass);

    auto *btnLogin = new QPushButton("Se connecter"); btnLogin->setFixedSize(115, 36);
    btnLogin->setStyleSheet("background: orange; color: white; font-weight: bold; border-radius: 6px;");
    auto *pBtnLogin = scene->addWidget(btnLogin); pBtnLogin->setPos(sw/2 - 120, sh/2 + 30); pBtnLogin->setZValue(6); authProxies.append(pBtnLogin);

    auto *btnGoReg = new QPushButton("Créer un compte"); btnGoReg->setFixedSize(115, 36);
    btnGoReg->setStyleSheet("background: #444; color: white; font-weight: bold; border-radius: 6px;");
    auto *pBtnGoReg = scene->addWidget(btnGoReg); pBtnGoReg->setPos(sw/2 + 5, sh/2 + 30); pBtnGoReg->setZValue(6); authProxies.append(pBtnGoReg);

    connect(btnLogin, &QPushButton::clicked, [this, edtUser, edtPass]() {
        QString user = edtUser->text().trimmed(); QString pass = edtPass->text();
        if (user.isEmpty() || pass.isEmpty()) { loginMsg->setPlainText("⚠️ Remplissez tous les champs"); return; }

        int uid = -1;
        if (Database::instance().loginUser(user, pass, uid)) {
            m_userId = uid; m_username = user;
            m_bestScore = Database::instance().getBestScore(uid);
            bestScoreText->setPlainText("Best: " + QString::number(m_bestScore));
            hideAuthScreen();
        } else {
            loginMsg->setPlainText("❌ Identifiants incorrects");
        }
    });

    connect(btnGoReg, &QPushButton::clicked, [this]() { showRegisterScreen(); });
}

void MainWindow::showRegisterScreen()
{
    hideAuthScreen();
    double sw = scene->sceneRect().width();
    double sh = scene->sceneRect().height();

    loginOverlay = scene->addRect(sw/2 - 160, sh/2 - 190, 320, 370, QPen(QColor(255,165,0), 2), QBrush(QColor(0,0,0,190)));
    loginOverlay->setZValue(5); authItems.append(loginOverlay);

    titleText = scene->addText("📝 INSCRIPTION");
    titleText->setDefaultTextColor(QColor(255, 165, 0)); titleText->setFont(QFont("Arial", 16, QFont::Bold));
    titleText->setPos(sw/2 - 90, sh/2 - 175); titleText->setZValue(6); authItems.append(titleText);

    loginMsg = scene->addText("");
    loginMsg->setDefaultTextColor(Qt::red); loginMsg->setFont(QFont("Arial", 10));
    loginMsg->setPos(sw/2 - 140, sh/2 + 135); loginMsg->setZValue(6); authItems.append(loginMsg);

    QString styleFields = "background: rgba(255,255,255,220); border: 2px solid orange; border-radius: 6px; padding: 4px; font-size: 14px;";

    auto *edtUser = new QLineEdit(); edtUser->setPlaceholderText("Nom d'utilisateur"); edtUser->setFixedSize(240, 36); edtUser->setStyleSheet(styleFields);
    auto *pUser = scene->addWidget(edtUser); pUser->setPos(sw/2 - 120, sh/2 - 130); pUser->setZValue(6); authProxies.append(pUser);

    auto *edtEmail = new QLineEdit(); edtEmail->setPlaceholderText("Adresse Email (Unique)"); edtEmail->setFixedSize(240, 36); edtEmail->setStyleSheet(styleFields);
    auto *pEmail = scene->addWidget(edtEmail); pEmail->setPos(sw/2 - 120, sh/2 - 80); pEmail->setZValue(6); authProxies.append(pEmail);

    auto *edtPass = new QLineEdit(); edtPass->setPlaceholderText("Mot de passe"); edtPass->setEchoMode(QLineEdit::Password); edtPass->setFixedSize(240, 36); edtPass->setStyleSheet(styleFields);
    auto *pPass = scene->addWidget(edtPass); pPass->setPos(sw/2 - 120, sh/2 - 30); pPass->setZValue(6); authProxies.append(pPass);

    auto *edtAge = new QLineEdit(); edtAge->setPlaceholderText("Votre Âge"); edtAge->setFixedSize(240, 36); edtAge->setStyleSheet(styleFields);
    auto *pAge = scene->addWidget(edtAge); pAge->setPos(sw/2 - 120, sh/2 + 20); pAge->setZValue(6); authProxies.append(pAge);

    auto *btnRegSubmit = new QPushButton("S'inscrire & Jouer"); btnRegSubmit->setFixedSize(115, 36);
    btnRegSubmit->setStyleSheet("background: #2ecc71; color: white; font-weight: bold; border-radius: 6px;");
    auto *pBtnReg = scene->addWidget(btnRegSubmit); pBtnReg->setPos(sw/2 - 120, sh/2 + 80); pBtnReg->setZValue(6); authProxies.append(pBtnReg);

    auto *btnBack = new QPushButton("Retour"); btnBack->setFixedSize(115, 36);
    btnBack->setStyleSheet("background: #7f8c8d; color: white; font-weight: bold; border-radius: 6px;");
    auto *pBtnBack = scene->addWidget(btnBack); pBtnBack->setPos(sw/2 + 5, sh/2 + 80); pBtnBack->setZValue(6); authProxies.append(pBtnBack);

    connect(btnRegSubmit, &QPushButton::clicked, [this, edtUser, edtEmail, edtPass, edtAge]() {
        QString user = edtUser->text().trimmed();
        QString email = edtEmail->text().trimmed();
        QString pass = edtPass->text();
        int age = edtAge->text().toInt();

        if (user.isEmpty() || email.isEmpty() || pass.isEmpty() || edtAge->text().isEmpty()) {
            loginMsg->setPlainText("⚠️ Remplissez tous les champs"); return;
        }

        int uid = -1;
        if (Database::instance().registerUser(user, email, pass, age, uid)) {
            m_userId = uid; m_username = user; m_bestScore = 0;
            bestScoreText->setPlainText("Best: 0");
            hideAuthScreen();
        } else {
            loginMsg->setPlainText("❌ Cet Email est déjà utilisé");
        }
    });

    connect(btnBack, &QPushButton::clicked, [this]() { showLoginScreen(); });
}

void MainWindow::hideAuthScreen()
{
    for (QGraphicsProxyWidget *proxy : authProxies) {
        if (proxy) { scene->removeItem(proxy); proxy->deleteLater(); }
    }
    authProxies.clear();

    for (QGraphicsItem *item : authItems) {
        if (item) { scene->removeItem(item); delete item; }
    }
    authItems.clear();

    loginOverlay = nullptr; titleText = nullptr; loginMsg = nullptr;

    if (m_userId == -1) return;

    velocity = 0.0; gamestart = false; isPaused = false; btnPause->setText("⏸️"); ball->setPos(60, 60);

    QGraphicsTextItem *hint = scene->addText("Appuie sur ESPACE pour jouer !");
    hint->setDefaultTextColor(Qt::black); hint->setFont(QFont("Arial", 11, QFont::Bold));
    hint->setZValue(3); hint->setData(0, "hint_text");
    hint->setPos(scene->sceneRect().width() / 2 - hint->boundingRect().width() / 2,
                 scene->sceneRect().height() / 2 - hint->boundingRect().height() / 2);

    proxyBtnPause->setPos(230, 10);
    proxyBtnPause->show();
    this->setFocus();
    timer->start(10);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (!gamestart) return;

        if (!isPaused) {
            isPaused = true;
            timer->stop();
            spawnTimer->stop();
            btnPause->setText("▶️");
        } else {
            isPaused = false;
            timer->start(10);
            spawnTimer->start(1500);
            btnPause->setText("⏸️");
        }
        return;
    }

    if (event->key() == Qt::Key_Space) {
        if (m_userId == -1) return;
        if (isPaused) return;

        if (!gamestart) {
            gamestart = true;
            velocity = -3.0;
            spawnTimer->start(1500);

            for (QGraphicsItem *item : scene->items()) {
                if (item->data(0).toString() == "hint_text") {
                    scene->removeItem(item);
                    delete item;
                    break;
                }
            }
        } else {
            velocity = -3.0 - ((score / 100.0) * 0.15);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}