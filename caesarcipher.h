#ifndef CAESARCIPHER_H
#define CAESARCIPHER_H

#include <QDialog>

#include "robin_hood.h"

namespace Ui {
class CaesarCipher;
}

class CaesarCipher : public QDialog
{
    Q_OBJECT

public:
    explicit CaesarCipher(QWidget *parent = nullptr);
    ~CaesarCipher();

signals:
    void changeDigits(int hexDigits[16]);

private slots:
    void on_digitSlider_valueChanged(int value);

    void on_pushButton_clicked();

private:
    Ui::CaesarCipher *ui;

    // map used to get the relevant character from an integer index
    robin_hood::unordered_map<int, char> intToHexDigit =
    {
        {0, '0'},
        {1, '1'},
        {2, '2'},
        {3, '3'},
        {4, '4'},
        {5, '5'},
        {6, '6'},
        {7, '7'},
        {8, '8'},
        {9, '9'},
        {10, 'A'},
        {11, 'B'},
        {12, 'C'},
        {13, 'D'},
        {14, 'E'},
        {15, 'F'}
    };
};

#endif // CAESARCIPHER_H
