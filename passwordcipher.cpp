#include "passwordcipher.h"
#include "ui_passwordcipher.h"

password::password(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::password)
{
    ui->setupUi(this);
}

password::~password()
{
    delete ui;
}

void password::on_passwordInput_returnPressed()
{
    // hashes the password using SHA3-512
    std::string hexString = QString(QCryptographicHash::hash((ui->passwordInput->text().toUtf8()),QCryptographicHash::Sha3_512).toHex()).toStdString();

    // removes duplicates
    for(size_t i = 0, j; i < hexString.size(); i++)
    {
        while((j = hexString.find_last_of(hexString[i])) and (j != i))
        {
            hexString.erase(j, 1);
        }
    }

    int hexDigits[16];

    // converts hex digits to their indexes to send
    for(size_t i = 0; i < 16; ++i)
    {
        hexDigits[i] = hexToIntDigit[hexString[i]];
    }

    // send signal to change digits in the main UI window
    emit changeDigits(hexDigits);

    close();
}

void password::on_pushButton_clicked()
{
    on_passwordInput_returnPressed();
}

