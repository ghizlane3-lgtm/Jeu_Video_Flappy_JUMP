#include "database.h"
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QDebug>

static QString hashPassword(const QString &pw) {
    return QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool Database::init() {
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dirPath);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dirPath + "/flappyball.db");

    if (!m_db.open()) {
        qDebug() << "DB Error:" << m_db.lastError().text();
        return false;
    }



    QSqlQuery q(m_db);


    bool ok = q.exec("CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "username TEXT NOT NULL,"
                     "email TEXT UNIQUE NOT NULL,"
                     "password TEXT NOT NULL,"
                     "age INTEGER NOT NULL,"
                     "best_score INTEGER DEFAULT 0,"
                     "saved_score INTEGER DEFAULT 0"
                     ")");

    if (!ok) qDebug() << "Table error:" << q.lastError().text();
    return ok;
}

bool Database::registerUser(const QString &username, const QString &email, const QString &password, int age, int &userId) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (username, email, password, age) VALUES (?, ?, ?, ?)");
    q.addBindValue(username);
    q.addBindValue(email);
    q.addBindValue(hashPassword(password));
    q.addBindValue(age);

    if (!q.exec()) {
        qDebug() << "Register error:" << q.lastError().text();
        return false;
    }

    userId = q.lastInsertId().toInt();
    return true;
}

bool Database::loginUser(const QString &username, const QString &password, int &userId) {
    QSqlQuery q(m_db);
    q.prepare("SELECT id FROM users WHERE username=? AND password=?");
    q.addBindValue(username);
    q.addBindValue(hashPassword(password));

    if (q.exec() && q.next()) {
        userId = q.value(0).toInt();
        return true;
    }
    return false;
}

int Database::getBestScore(int userId) {
    QSqlQuery q(m_db);
    q.prepare("SELECT best_score FROM users WHERE id=?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 0;
}

int Database::getSavedScore(int userId) {
    QSqlQuery q(m_db);
    q.prepare("SELECT saved_score FROM users WHERE id=?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 0;
}

void Database::saveProgress(int userId, int score) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET saved_score=? WHERE id=?");
    q.addBindValue(score);
    q.addBindValue(userId);
    q.exec();
    updateBestScore(userId, score);
}

void Database::updateBestScore(int userId, int score) {
    if (score > getBestScore(userId)) {
        QSqlQuery q(m_db);
        q.prepare("UPDATE users SET best_score=? WHERE id=?");
        q.addBindValue(score);
        q.addBindValue(userId);
        q.exec();
    }
}

QList<QPair<QString, int>> Database::getLeaderboard() {
    QList<QPair<QString, int>> result;
    QSqlQuery q(m_db);
    q.exec("SELECT username, best_score FROM users ORDER BY best_score DESC LIMIT 10");
    while (q.next()) {
        result.append({q.value(0).toString(), q.value(1).toInt()});
    }
    return result;
}