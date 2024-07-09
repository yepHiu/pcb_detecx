#include "jsonresult.h"

jsonresult::jsonresult(const QString& fileName, QTableWidget* tableWidget)
{
    _filename=fileName;
    qtable=tableWidget;
}

jsonresult::~jsonresult(){

}

void jsonresult::loadJsonIntoTable(){
    JsonIntoTable(_filename,qtable);
}



void JsonIntoTable(const QString& fileName, QTableWidget* tableWidget) {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file:" << fileName;
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }

    if (!jsonDoc.isArray()) {
        qWarning() << "JSON is not an array.";
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    // Set the column count and headers
    //QStringList headers = {"Index", "x", "y", "conf", "defect_type"};
    //tableWidget->setColumnCount(headers.size());
    //tableWidget->setHorizontalHeaderLabels(headers);

    // Populate the table with data
    tableWidget->setRowCount(jsonArray.size());
    int row = 0;
    for (const QJsonValue& value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            double x1 = obj["x1"].toDouble();
            double x2 = obj["x2"].toDouble();
            double y1 = obj["y1"].toDouble();
            double y2 = obj["y2"].toDouble();
            double x = (x1 + x2) / 2;
            double y = (y1 + y2) / 2;
            double conf = obj["conf"].toDouble();
            QString defectType = obj["defect_type"].toString();

            tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1))); // Index starts from 1
            tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(x)));
            tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(y)));
            tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(conf)));
            tableWidget->setItem(row, 4, new QTableWidgetItem(defectType));
            row++;
        }
    }
}

