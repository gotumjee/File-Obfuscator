#include "caesarcipher.h"
#include "ui_caesarcipher.h"

CaesarCipher::CaesarCipher(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaesarCipher)
{
    ui->setupUi(this);
}

CaesarCipher::~CaesarCipher()
{
    delete ui;
}

// called when Caesar Cipher parameter slider is moved
void CaesarCipher::on_digitSlider_valueChanged(int value)
{
    // changes the label to demonstrate the changes that will occur
    ui->zeroTo->setText("0 -> " + QString(QChar(intToHexDigit[value])));
}

void CaesarCipher::on_pushButton_clicked()
{
    int hexDigits[16];

    // if value goes above 15, loop back to 0
    for(size_t i = 0; i < 16; ++i)
    {
        hexDigits[i] = i + ui->digitSlider->value();
        if(hexDigits[i] >= 16)
            hexDigits[i] -= 16;
    }

    // send signal to change digits in the main UI window
    emit changeDigits(hexDigits);

    close();
}
