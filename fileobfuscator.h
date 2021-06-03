#ifndef FILEOBFUSCATOR_H
#define FILEOBFUSCATOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QComboBox>
#include <QMessageBox>
#include <QtConcurrent>
#include <QFuture>
#include <QListWidgetItem>

#include "robin_hood.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FObs; }
QT_END_NAMESPACE

class FObs : public QMainWindow
{
    Q_OBJECT

public:
    FObs(QWidget *parent = nullptr);
    ~FObs();

signals:
    void emitCommandLog(QString message, int displayDuration = 4000, bool logOrNot = true);
    void emitMessage(QString message, QMessageBox::Icon icon = QMessageBox::Critical, bool logOrNot = true);
    void emitProgressBar(int value);
    void emitRepurposeEncode(bool cancelOrNot);
    void emitUIElements(bool flag);

private slots:
    void on_actionOpen_File_triggered();
    void on_actionOpen_Folder_triggered();
    void on_actionExit_triggered();
    void on_actionEncode_triggered();
    void on_actionDecode_triggered();
    void on_paramsButton_clicked();
    void on_actionReverse_file_toggled(bool arg1);
    void on_actionDark_Mode_toggled(bool arg1);
    void on_encodeButton_clicked();
    void on_decodeButton_clicked();

    void on_cipherOption_itemDoubleClicked(QListWidgetItem *item);
    void on_cipherOption_currentRowChanged(int currentRow);
    void on_Combo0_currentIndexChanged(int index);
    void on_Combo1_currentIndexChanged(int index);
    void on_Combo2_currentIndexChanged(int index);
    void on_Combo3_currentIndexChanged(int index);
    void on_Combo4_currentIndexChanged(int index);
    void on_Combo5_currentIndexChanged(int index);
    void on_Combo6_currentIndexChanged(int index);
    void on_Combo7_currentIndexChanged(int index);
    void on_Combo8_currentIndexChanged(int index);
    void on_Combo9_currentIndexChanged(int index);
    void on_Combo10_currentIndexChanged(int index);
    void on_Combo11_currentIndexChanged(int index);
    void on_Combo12_currentIndexChanged(int index);
    void on_Combo13_currentIndexChanged(int index);
    void on_Combo14_currentIndexChanged(int index);
    void on_Combo15_currentIndexChanged(int index);

    void callMessageBox(QString message, QMessageBox::Icon icon = QMessageBox::Critical, bool logOrNot = true);
    void changeDigits(int hexDigits[16]);
    void commandLog(QString message, int displayDuration = 4000, bool logOrNot = true);
    void repurposeEncode(bool cancelOrNot);
    void UIElements(bool flag);

    void on_actionRandomize_Digits_triggered();
private:
    Ui::FObs *ui;

    size_t currentCipher = 0;
    bool brokenOperation = false;
    bool hasParameters = false;
    bool secureParameters = false;
    bool issues = false;
    bool reverseFile = false;
    bool setupFlag = false;
    bool runningOrNot = false;
    QFuture<bool> readFile;
    QFuture<void> tempFolderHandler;

    QString openFileName = "";
    QString saveFileName = "";
    QString openFolderName = "";
    QString saveFolderName = "";

    QFile openFile;
    QFile saveFile;

    class ciphers
    {
    public:
        QString cipherName;
        QString cipherDescription;
        bool cipherParameters;
        bool cipherSecurity;
        int cipherDigits[16];

        ciphers(QString passedName,
                QString passedDescription,
                bool passedParameters = false,
                bool passedSecurity = false,
                int passedDigits[16] = (int []){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15})
        {
            cipherName = passedName;
            cipherDescription = passedDescription;
            cipherParameters = passedParameters;
            cipherSecurity = passedSecurity;
            std::copy(passedDigits, passedDigits + 16, cipherDigits);
        }
    };
    std::vector<ciphers> listOfCiphers;

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
        {10, 'a'},
        {11, 'b'},
        {12, 'c'},
        {13, 'd'},
        {14, 'e'},
        {15, 'f'}
    };

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

    robin_hood::unordered_map<QChar, QChar> currentIndexes;

    void closeEvent(QCloseEvent *closeOrNot);
    void addCipher(QString passedName,
                   QString passedDescription,
                   bool passedParameters = false,
                   bool passedSecurity = false,
                   int passedDigits[16] = (int []){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    void readCiphersFromFile();
    void checkIssues();
    void folderHandler();
    void encodeJSON(bool jsonOrNot);
    void decodeJSON(bool jsonOrNot);
    bool readWriteToFile(QString oFileName, QString sFileName);
    void cancelAllOperations();
};
#endif // FILEOBFUSCATOR_H
