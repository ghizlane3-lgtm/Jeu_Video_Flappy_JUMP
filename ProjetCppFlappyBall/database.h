#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>
#include <QPair>
#include <QString>

class Database {
public:
    static Database& instance() {
        static Database ins;
        return ins;
    }

    bool init();
    // Modifié : Ajout de l'âge et retour de l'ID utilisateur
    bool registerUser(const QString &username, const QString &email, const QString &password, int age, int &userId);
    bool loginUser(const QString &username, const QString &password, int &userId);

    int getBestScore(int userId);
    int getSavedScore(int userId);
    void saveProgress(int userId, int score);
    void updateBestScore(int userId, int score);
    QList<QPair<QString, int>> getLeaderboard();

private:
    Database() = default;
    ~Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    QSqlDatabase m_db;
};

#endif // DATABASE_H