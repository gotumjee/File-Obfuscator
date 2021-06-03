#include "fileobfuscator.h"
#include "ui_fileobfuscator.h"

FObs::FObs(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FObs)
{
    ui->setupUi(this);

    // themes the window
    on_actionDark_Mode_toggled(true);

    // disabled window resizing
    this->statusBar()->setSizeGripEnabled(false);

    // initial setup
    addCipher("Subsitution Cipher", "Replace a hexadecimal digit with another.");
    readCiphersFromFile();

    // sets the default row for startup
    ui->cipherOption->setCurrentRow(0);

    // connects the command log for within thread signals
    QObject::connect(this, &FObs::emitCommandLog, this, &FObs::commandLog);

    // connects the message box for within thread signals
    QObject::connect(this, &FObs::emitMessage, this, &FObs::callMessageBox);

    // connects the progress bar for within thread signals
    QObject::connect(this, SIGNAL(emitProgressBar(int)), ui->progressBar, SLOT(setValue(int)));

    // connects the encode button for within thread signals
    QObject::connect(this, &FObs::emitRepurposeEncode, this, &FObs::repurposeEncode);

    // allows for disabling UI elements from within thread signals
    QObject::connect(this, &FObs::emitUIElements, this, &FObs::UIElements);
}

FObs::~FObs()
{
    delete ui;
}

// function to create an object to store a new cipher's name and description
void FObs::addCipher(QString passedName, QString passedDescription, bool passedParameters, bool passedSecurity, int passedDigits[16])
{
    // creates a class object to store the cipher's details
    ciphers cipher(passedName, passedDescription, passedParameters, passedSecurity, passedDigits);

    // stores the cipher in a list
    listOfCiphers.push_back(cipher);

    // adds the cipher to the list of opions
    ui->cipherOption->addItem(passedName);
}

void FObs::on_actionExit_triggered()
{
    // attemps to quit
    QCoreApplication::quit();
}

// runs when X(close) button is pressed
// (Sundram, 2016)
void FObs::closeEvent(QCloseEvent *closeOrNot)
{
    // if an operation is running, ask the user if they want to quit
    if(runningOrNot)
    {
        QMessageBox::StandardButton exitOption;
        exitOption = QMessageBox::question(this, "Exit program", "An operation is still running. Are you sure you want to exit?", QMessageBox::Yes|QMessageBox::No);

        if(exitOption == QMessageBox::Yes)
        {
            // accepts the close event, i.e., closes
            cancelAllOperations();
            closeOrNot->accept();
        }
        else
        {
            // ignores otherwise
            closeOrNot->ignore();
        }
    }
    else
    {
        cancelAllOperations();
        closeOrNot->accept();
    }
}

// called when cipher is changed
void FObs::on_cipherOption_currentRowChanged(int currentRow)
{
    // sets the number and security parameters of the current cipher for all class members to access
    // needed as some members do not have access to all UI class members
    currentCipher = currentRow;
    hasParameters =  listOfCiphers[currentCipher].cipherParameters;
    secureParameters = listOfCiphers[currentCipher].cipherSecurity;

    if(!secureParameters)
        ui->actionRandomize_Digits->setEnabled(true);
    else
        ui->actionRandomize_Digits->setEnabled(false);

    // changes description, parameter availability and default indexes appropriately
    ui->cipherDescription->setText(listOfCiphers[currentCipher].cipherDescription);
    ui->paramsButton->setEnabled(hasParameters);
    changeDigits(listOfCiphers[currentCipher].cipherDigits);
}

// used to create a message box
// can be used by functions on other threads
void FObs::callMessageBox(QString message, QMessageBox::Icon icon, bool logOrNot)
{
    if(logOrNot)
        commandLog(message);

    QMessageBox msgBox;
    msgBox.setWindowTitle("File Obfuscator");
    msgBox.setIcon(icon);
    msgBox.setWindowIcon(QIcon(":/fobsicons/fobs.ico"));
    msgBox.setText(message);
    msgBox.exec();
}

// function used to set indexes and create JSON when encoding files
void FObs::encodeJSON(bool jsonOrNot)
{
    // map used to iterate through each hex digit combo box entry
    robin_hood::unordered_map<int, QComboBox *> comboIndexes {
            {0, ui->Combo0},
            {1, ui->Combo1},
            {2, ui->Combo2},
            {3, ui->Combo3},
            {4, ui->Combo4},
            {5, ui->Combo5},
            {6, ui->Combo6},
            {7, ui->Combo7},
            {8, ui->Combo8},
            {9, ui->Combo9},
            {10, ui->Combo10},
            {11, ui->Combo11},
            {12, ui->Combo12},
            {13, ui->Combo13},
            {14, ui->Combo14},
            {15, ui->Combo15},
        };

    // gets index for all digits and stores in a map
    for(size_t i = 0; i < 16; ++i)
    {
        currentIndexes[intToHexDigit[i]] = intToHexDigit[comboIndexes[i]->currentIndex()];
    }

    if(secureParameters)
    {
        for(size_t i = 0; i < 16; ++i)
        {
            comboIndexes[i]->setCurrentIndex(0);
        }
        commandLog("Password has been concealed, do not panic.", 4000);
    }

    else if(jsonOrNot)
    {
        QString saveJSONname = QFileDialog::getSaveFileName(this,
                                                          tr("Save file"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                          tr("JSON file (*.json)"));

        if(saveJSONname == "")
        {
            callMessageBox("No file or folder has been selected.");
            issues = true;
            return;
        }

        else if(saveJSONname == openFileName or saveJSONname == saveFileName)
        {
            callMessageBox("The JSON file cannot be the same as the file selected.");
            issues = true;
            return;
        }

        // creates a JSON object
        QJsonObject JSONobject;

        // saves hex digits to JSON object
        for(size_t i = 0; i < 16; ++i)
        {
            JSONobject.insert(QString(intToHexDigit[i]), QString(currentIndexes[intToHexDigit[i]]));
        }

        // creates a JSON document using the JSON object
        QJsonDocument JSONfile(JSONobject);

        // opens the file to which JSON has to be saved
        QFile saveJSON(saveJSONname);
        if(!saveJSON.open(QIODevice::WriteOnly))
        {
            if(!QFileInfo(saveJSON).isWritable())
            {
                callMessageBox("This file cannot be written to. Please ensure you have write permissions.");
            }
            else
            {
                callMessageBox("This file cannot be written to.");
            }

            issues = true;
            return;
        }

        // writes the file and closes it
        saveJSON.write(JSONfile.toJson());
        saveJSON.close();
    }
}

void FObs::decodeJSON(bool jsonOrNot)
{
    robin_hood::unordered_map<int, QComboBox *> comboIndexes {
            {0, ui->Combo0},
            {1, ui->Combo1},
            {2, ui->Combo2},
            {3, ui->Combo3},
            {4, ui->Combo4},
            {5, ui->Combo5},
            {6, ui->Combo6},
            {7, ui->Combo7},
            {8, ui->Combo8},
            {9, ui->Combo9},
            {10, ui->Combo10},
            {11, ui->Combo11},
            {12, ui->Combo12},
            {13, ui->Combo13},
            {14, ui->Combo14},
            {15, ui->Combo15},
        };

    if(jsonOrNot)
    {
        QString openJSONname = QFileDialog::getOpenFileName(this,
                                                            tr("Open file"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                            tr("JSON file (*.json)"));

        if(openJSONname == "")
        {
            callMessageBox("No file or folder has been selected.");
            issues = true;
            return;
        }

        QFile openJSON(openJSONname);
        if(!openJSON.open(QIODevice::ReadOnly))
        {
            if(!QFileInfo(openJSONname).isReadable())
            {
                callMessageBox("This file cannot be read. Please ensure you have read permissions.");
            }

            else
            {
                callMessageBox("This file cannot be read.");
            }

            issues = true;
            return;
        }

        // catch any errors with reading the JSON file
        try
        {
            QJsonDocument JSONfile = QJsonDocument::fromJson(openJSON.readAll());
            openJSON.close();

            if(JSONfile.isNull())
            {
                callMessageBox("The loaded JSON file is not valid.");
                issues = true;
                return;
            }

            QJsonObject JSONobject = JSONfile.object();

            // reads the JSON object's keys and values and alters the combo boxes accordingly
            for(auto i = JSONobject.begin(); i != JSONobject.end(); ++i)
            {
                comboIndexes[hexToIntDigit[i.key().toStdString().c_str()[0]]]->setCurrentIndex(hexToIntDigit[QString(i.value().toString()).toStdString().c_str()[0]]);
            }

            if(issues)
            {
                callMessageBox("The loaded JSON file contains duplicates or invalid characters. Please check the file.");
                return;
            }
        }
        catch(...)
        {
            issues = true;
            callMessageBox("The laoded JSON file is not valid.");
            return;
        }
    }

    // gets index for all digits and stores in a map
    // the opposite is used when decoding to make it more intuitive for users
    // for instance, if A->0 when encoding, when decoding, 0->A will be used when A->0 will be displayed
    for(size_t i = 0; i < 16; ++i)
    {
        currentIndexes[intToHexDigit[comboIndexes[i]->currentIndex()]] = intToHexDigit[i];
    }

    if(secureParameters)
    {
        for(size_t i = 0; i < 16; ++i)
        {
            comboIndexes[i]->setCurrentIndex(0);
        }
        commandLog("Password has been concealed, do not panic.", 8000);
    }
}

// function used to enable/disable all UI elements
void FObs::UIElements(bool flag)
{
    // map used to iterate through each hex digit combo box entry
    robin_hood::unordered_map<int, QComboBox *> comboIndexes {
            {0, ui->Combo0},
            {1, ui->Combo1},
            {2, ui->Combo2},
            {3, ui->Combo3},
            {4, ui->Combo4},
            {5, ui->Combo5},
            {6, ui->Combo6},
            {7, ui->Combo7},
            {8, ui->Combo8},
            {9, ui->Combo9},
            {10, ui->Combo10},
            {11, ui->Combo11},
            {12, ui->Combo12},
            {13, ui->Combo13},
            {14, ui->Combo14},
            {15, ui->Combo15},
        };

    for(size_t i = 0; i < 16; ++i)
    {
        comboIndexes[i]->setEnabled(flag);
    }

    ui->encodeButton->setEnabled(flag);
    ui->decodeButton->setEnabled(flag);
    ui->actionOpen_File->setEnabled(flag);
    ui->actionOpen_Folder->setEnabled(flag);
    ui->actionReverse_file->setEnabled(flag);
    ui->cipherOption->setEnabled(flag);
    ui->actionEncode->setEnabled(flag);
    ui->actionDecode->setEnabled(flag);
    if(hasParameters)
        ui->paramsButton->setEnabled(flag);
}

void FObs::on_actionEncode_triggered()
{
    on_encodeButton_clicked();
}

void FObs::on_actionDecode_triggered()
{
    on_decodeButton_clicked();
}

void FObs::on_actionReverse_file_toggled(bool arg1)
{
    reverseFile = arg1;

    if(arg1)
        callMessageBox("Please note, the reverse file mode TANKS the performance.", QMessageBox::Warning, false);
}

// opens parameters when cipher is double-clicked
void FObs::on_cipherOption_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    if(hasParameters)
        on_paramsButton_clicked();
}

// used to load the stylesheet
// facilitates switchable dark mode
void FObs::on_actionDark_Mode_toggled(bool arg1)
{
    if(arg1)
    {
        QFile QSS(":qdarkstyle/dark/dark.qss");
        QSS.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(QSS.readAll());
    }
    else
    {
        QFile QSS(":qdarkstyle/light/light.qss");
        QSS.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(QSS.readAll());
    }
}

// used to facilitate the logging, called when a message box or status bar message is called
void FObs::commandLog(QString message, int displayDuration, bool logOrNot)
{
    statusBar()->showMessage(message, displayDuration);

    if(logOrNot)
        ui->commandLog->appendPlainText('[' + QTime::currentTime().toString("hh:mm:ss") + "] " + message);
}

void FObs::cancelAllOperations()
{
    // used to break the folder iteration thread
    brokenOperation = true;

    // cancels the QFuture instance waiting for the thread reading the file's result
    readFile.cancel();

    // close the QFuture instance used to iterate through all files in a folder
    // ensures that no more files will be run as part of the iteration
    tempFolderHandler.cancel();

    // closes the file pointers
    openFile.close();
    saveFile.close();

    // indicates that no operations are taking place
    runningOrNot = false;

    // resets UI elements
    emit emitRepurposeEncode(false);
    emit emitUIElements(true);
    emit emitProgressBar(0);
}

void FObs::on_actionRandomize_Digits_triggered()
{
    // vector used to store the indexes possible for the hexadecimal digits
    QVector<int> hexDigits = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    robin_hood::unordered_map<int, QComboBox *> comboIndexes {
            {0, ui->Combo0},
            {1, ui->Combo1},
            {2, ui->Combo2},
            {3, ui->Combo3},
            {4, ui->Combo4},
            {5, ui->Combo5},
            {6, ui->Combo6},
            {7, ui->Combo7},
            {8, ui->Combo8},
            {9, ui->Combo9},
            {10, ui->Combo10},
            {11, ui->Combo11},
            {12, ui->Combo12},
            {13, ui->Combo13},
            {14, ui->Combo14},
            {15, ui->Combo15},
        };

    // integer used to store the index to be deleted from the vector
    size_t storeIndex;

    for(size_t i = 0; i < 16; ++i)
    {
        // gets a random number from the elements in the vector
        storeIndex = QRandomGenerator::global()->generate() % hexDigits.size();

        // sets the index using the randomization's result
        comboIndexes[i]->setCurrentIndex(hexDigits[storeIndex]);

        // removes the index from the vector to ensure no duplicates
        hexDigits.remove(storeIndex);
    }
}

void FObs::repurposeEncode(bool cancelOrNot)
{
    if(cancelOrNot)
    {
        ui->encodeButton->setText("Cancel");
        ui->encodeButton->setEnabled(true);
    }
    else
    {
        ui->encodeButton->setText("Encode");
    }
}
