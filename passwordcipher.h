#ifndef PASSWORDCIPHER_H
#define PASSWORDCIPHER_H

#include <QDialog>
#include <QCryptographicHash>

#include "robin_hood.h"

namespace Ui {
class password;
}

class password : public QDialog
{
    Q_OBJECT

public:
    explicit password(QWidget *parent = nullptr);
    ~password();

signals:
    void changeDigits(int hexDigits[16]);

private slots:
    void on_passwordInput_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::password *ui;

    robin_hood::unordered_map<char, int> hexToIntDigit =
    {
        {'0', 0},
        {'1', 1},
        {'2', 2},
        {'3', 3},
        {'4', 4},
        {'5', 5},
        {'6', 6},
        {'7', 7},
        {'8', 8},
        {'9', 9},
        {'a', 10},
        {'b', 11},
        {'c', 12},
        {'d', 13},
        {'e', 14},
        {'f', 15}
    };
};

#endif // PASSWORDCIPHER_H
