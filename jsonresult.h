#ifndef JSONFILE
#define JSONFILE

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QApplication>
#include <QFile>
#include <QDebug>

class jsonresult
{
public:
    jsonresult(const QString& fileName, QTableWidget* tableWidget);
    ~jsonresult();
    void loadJsonIntoTable();

private:
    QString _filename;
    QTableWidget *qtable;
};

void JsonIntoTable(const QString& fileName, QTableWidget* tableWidget);

#endif // JSONFILE
