#include "fileobfuscator.h"

// add header files for ciphers here
#include "caesarcipher.h"
#include "passwordcipher.h"

// function used to add ciphers
void FObs::readCiphersFromFile()
{
    // cipher number 1
    int caesarArray[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    addCipher("Caesar Cipher", "Shift all hexadecimal digits forward.", true, false, caesarArray);

    // cipher number 2
    addCipher("Password Cipher", "\"Encrypts/Decrypts\" a file with the supplied password.", true, true);

    // new ciphers should be added here
    // format is addCipher(<cipher name>, <cipher description>, <parameters>(optional), <security>(optional), <default digits>(optional))
}

void FObs::on_paramsButton_clicked()
{
    switch(currentCipher)
    {
    // this syntax MUST be followed for all ciphers
    case 1  :   {
                    CaesarCipher parametersUI;

                    QObject::connect(&parametersUI, &CaesarCipher::changeDigits, this, &FObs::changeDigits);
                    parametersUI.exec();
                    break;
                }

    case 2  :   {
                    password parametersUI;

                    QObject::connect(&parametersUI, &password::changeDigits, this, &FObs::changeDigits);
                    parametersUI.exec();
                    break;
                }

    // new ciphers should be added here

    /* format is
     <cipher class name> parametersUI;

     QObject::connect(&parametersUI, &<cipher class name>::changeDigits, this, &FObs::changeDigits);
     parametersUI.exec();
     break;
    */

    default :   break;
    }
}
