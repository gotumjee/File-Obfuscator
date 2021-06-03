#include "fileobfuscator.h"
#include "ui_fileobfuscator.h"

void FObs::on_actionOpen_File_triggered()
{
    QString tempFileName;

    // gets file name from user
    tempFileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("All files (*.*)"));

    // if file name is not invalid, store it
    if(tempFileName != "")
    {
        openFileName = tempFileName;
    }

    if(openFileName != "" and QFileInfo::exists(openFileName) and QFileInfo(openFileName).isFile())
    {
        if(!QFileInfo(openFileName).isReadable())
        {
            callMessageBox("This file cannot be read. Please check the file's permissions.");
            return;
        }
        openFolderName = "";

        commandLog(openFileName + " has been opened!", 4000);

        // once file has been opened, restore original functionality to encode and decode buttons
        setupFlag = true;
        ui->encodeButton->setText("Encode");
        ui->decodeButton->setText("Decode");

        UIElements(true);
        emit emitProgressBar(0);
    }
}

void FObs::on_actionOpen_Folder_triggered()
{
    QString tempFolderName;

    tempFolderName = QFileDialog::getExistingDirectory(this,
                                                     tr("Open folder"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(tempFolderName != "")
    {
        openFolderName = tempFolderName;
    }

    if(openFolderName != "" and QDir(openFolderName).exists())
    {
        if(!QDir(openFolderName).isReadable())
        {
            callMessageBox("This folder cannot be read. Please check the folder's permissions.");
            return;
        }
        openFileName = "";

        commandLog(openFolderName + " has been opened!", 4000);

        setupFlag = true;
        ui->encodeButton->setText("Encode");
        ui->decodeButton->setText("Decode");

        UIElements(true);
        emit emitProgressBar(0);
    }
}

void FObs::on_encodeButton_clicked()
{
    // used to make the buttons multi-purpose
    // if initial set-up hasn't been completed, the button is used to open files
    if(!setupFlag)
    {
        on_actionOpen_File_triggered();
        return;
    }

    // if the program is running, this button is used as the cancel button
    if(runningOrNot)
    {
        cancelAllOperations();
        commandLog("The operation has been cancelled.", 8000);
        return;
    }

    // resets the issues flag and checks for any issues
    checkIssues();

    // if there are duplicate hex digits, prevent the program from running
    if(issues)
    {
        callMessageBox("There are duplicate hex digits. Please ensure there are no duplicate digits (marked in red) have been resolved.");
        return;
    }

    // if no files or folders have been selected, prevent the program from running
    if(openFileName == "" and openFolderName == "")
    {
        callMessageBox("No file or folder has been selected.");
        return;
    }

    if(openFileName != "" and QFileInfo::exists(openFileName) and QFileInfo(openFileName).isFile())
    {
        if(!QFileInfo(openFileName).isReadable())
        {
            callMessageBox("This file cannot be read. Please check the file's permissions.");
            return;
        }

        saveFileName = QFileDialog::getSaveFileName(this,
                                                          tr("Save file"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                          tr("All files (*.*)"));

        if(saveFileName == "")
        {
            callMessageBox("No file has been selected.");
            return;
        }

        else if(saveFileName == openFileName)
        {
            callMessageBox("The file to save cannot be the same as the file opened.");
            return;
        }

        // if the program doesn't need security, prompt user to create JSON to store the hex digits and their translations
        if(!secureParameters)
        {
            QMessageBox::StandardButton JSONOption;
            JSONOption = QMessageBox::question(this, "Create JSON", "Would you like to create a JSON file to store the hex digit translation?", QMessageBox::Yes|QMessageBox::No);

            if(JSONOption == QMessageBox::Yes)
            {
                encodeJSON(true);
            }
            else
            {
                encodeJSON(false);
            }
        }
        else
        {
            encodeJSON(false);
        }

        if(issues and !secureParameters)
        {
            return;
        }

        // assigns false as default to let the files be read unless cancelled
        brokenOperation = false;

        // starts new thread to read and write to file
        readFile = QtConcurrent::run(&FObs::readWriteToFile, this, openFileName, saveFileName);
    }

    if(openFolderName != "" and QDir(openFolderName).exists())
    {
        if(!QDir(openFolderName).isReadable())
        {
            callMessageBox("This folder cannot be read. Please check the folder's permissions.");
            return;
        }

        saveFolderName = QFileDialog::getExistingDirectory(this,
                                                        tr("Save to folder"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if(saveFolderName == "")
        {
            callMessageBox("No folder has been selected.");
            return;
        }

        if(!secureParameters)
        {
            QMessageBox::StandardButton JSONOption;
            JSONOption = QMessageBox::question(this, "Create JSON", "Would you like to create a JSON file to store the hex digit translation?", QMessageBox::Yes|QMessageBox::No);

            if(JSONOption == QMessageBox::Yes)
            {
                encodeJSON(true);
            }
            else
            {
                encodeJSON(false);
            }
        }
        else
        {
            encodeJSON(false);
        }

        if(issues and !secureParameters)
        {
            return;
        }

        brokenOperation = false;

        // starts new thread to iterate through files in a directory and read and write to file
        tempFolderHandler = QtConcurrent::run(&FObs::folderHandler, this);
    }
}

void FObs::on_decodeButton_clicked()
{
    if(!setupFlag)
    {
        on_actionOpen_Folder_triggered();
        return;
    }

    checkIssues();
    if(issues)
    {
        callMessageBox("There are duplicate hex digits. Please ensure there are no duplicate digits (marked in red) have been resolved.");
        return;
    }

    if(openFileName == "" and openFolderName == "")
    {
        callMessageBox("No file or folder has been selected.");
        return;
    }

    if(openFileName != "" and QFileInfo::exists(openFileName) and QFileInfo(openFileName).isFile())
    {
        if(!QFileInfo(openFileName).isReadable())
        {
            callMessageBox("This file cannot be read. Please check the file's permissions.");
            return;
        }

        saveFileName = QFileDialog::getSaveFileName(this,
                                                          tr("Save file"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                          tr("All files (*.*)"));

        if(saveFileName == "")
        {
            callMessageBox("No file has been selected.");
            return;
        }

        else if(saveFileName == openFileName)
        {
            callMessageBox("The file to save cannot be the same as the file opened.");
            return;
        }

        if(!secureParameters)
        {
            QMessageBox::StandardButton JSONOption;
            JSONOption = QMessageBox::question(this, "Load JSON", "Would you like to load a JSON file to retrieve the hex digit translation?", QMessageBox::Yes|QMessageBox::No);

            if(JSONOption == QMessageBox::Yes)
            {
                decodeJSON(true);
            }
            else
            {
                decodeJSON(false);
            }
        }
        else
        {
            decodeJSON(false);
        }

        if(issues and !secureParameters)
        {
            return;
        }

        brokenOperation = false;
        readFile = QtConcurrent::run(&FObs::readWriteToFile, this, openFileName, saveFileName);
    }

    if(openFolderName != "" and QDir(openFolderName).exists())
    {
        if(!QDir(openFolderName).isReadable())
        {
            callMessageBox("This folder cannot be read. Please check the folder's permissions.");
            return;
        }

        saveFolderName = QFileDialog::getExistingDirectory(this,
                                                        tr("Save to folder"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if(saveFolderName == "")
        {
            callMessageBox("No folder has been selected.");
            return;
        }

        if(!secureParameters)
        {
            QMessageBox::StandardButton JSONOption;
            JSONOption = QMessageBox::question(this, "Load JSON", "Would you like to load a JSON file to retrieve the hex digit translation?", QMessageBox::Yes|QMessageBox::No);

            if(JSONOption == QMessageBox::Yes)
            {
                decodeJSON(true);
            }
            else
            {
                decodeJSON(false);
            }
        }
        else
        {
            decodeJSON(false);
        }

        if(issues and !secureParameters)
        {
            return;
        }

        brokenOperation = false;
        tempFolderHandler = QtConcurrent::run(&FObs::folderHandler, this);
    }
}

void FObs::folderHandler()
{
    // used iterate through all files in a folder
    QDirIterator folderIterator(openFolderName, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QString tempFileName;

    // cycles through all files in a directory
    while(folderIterator.hasNext())
    {
        tempFileName = folderIterator.next();

        // gets the file name without the path and adds a custom extension
        // saves to the folder specified by the user
        readFile = QtConcurrent::run(&FObs::readWriteToFile,
                                     this,
                                     tempFileName,
                                     saveFolderName + '/' + QFileInfo(tempFileName).fileName() + ".FObs");

        readFile.waitForFinished();

        if(brokenOperation)
            return;
    }
}

bool FObs::readWriteToFile(QString oFileName, QString sFileName)
{
    openFile.setFileName(oFileName);
    if(!openFile.open(QIODevice::ReadOnly))
    {
        emit emitMessage("Opened file cannot be read.");
        brokenOperation = true;
        return false;
    }

    saveFile.setFileName(sFileName);
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        emit emitMessage("File to save cannot be written to.");
        brokenOperation = true;
        return false;
    }

    // sets the flag to indicate that the program is running
    runningOrNot = true;

    // disables most UI elements
    emit emitUIElements(false);

    // repurposes the encode button to be used to cancel the program
    emit emitRepurposeEncode(true);

    // resets progress bar
    emit emitProgressBar(0);

    // logs the operation beginning
    emit emitCommandLog("Operation has started for " + oFileName);

    // get file size and declare string to store file data
    qint64 length = openFile.size();
    QString hexDigits;

    // used to catch any errors that may occur when the operation has been cancelled
    try
    {
        // if file reverse has not been enabled
        if(!reverseFile)
        {
            while(!openFile.atEnd())
            {
                // reads 9999 bytes from a file, converts to hex, and stores in the file
                // found to be the sweet spot between writing to cache and writing to the disk
                // determined experimentally, not very representative
                hexDigits = QString::fromUtf8(openFile.read(9999).toHex());

                // modified each hex digit individually using the map
                for(qint64 i = 0; i < hexDigits.size(); ++i)
                {
                    hexDigits[i] = currentIndexes[hexDigits[i]];
                }

                // writes the new modified hex digits to the file
                saveFile.write(QByteArray::fromHex(hexDigits.toUtf8()));

                // updates the progress bar periodically
                emit emitProgressBar(int(openFile.pos() * 100 / length));
            }
        }
        else
        {
            // seeks near the end of the file (not the end as the program has to read one byte)
            openFile.seek(length - 1);

            // SUPER slow as program has to ready byte-by-byte backwards
            while(openFile.pos() != 0)
            {
                hexDigits = QString::fromUtf8(openFile.read(1).toHex());

                for(qint64 i = 0; i < hexDigits.size(); ++i)
                {
                    hexDigits[i] = currentIndexes[hexDigits[i]];
                }

                saveFile.write(QByteArray::fromHex(hexDigits.toUtf8()));

                openFile.seek(openFile.pos() - 2);

                emit emitProgressBar(int((length - openFile.pos()) * 100 / length));
            }

            // manually runs the loop again to read the last (technically first) hex digit
            {
                hexDigits = QString::fromUtf8(openFile.read(1).toHex());

                for(qint64 i = 0; i < hexDigits.size(); ++i)
                {
                    hexDigits[i] = currentIndexes[hexDigits[i]];
                }

                saveFile.write(QByteArray::fromHex(hexDigits.toUtf8()));

                emit emitProgressBar(100);
            }
        }
    }
    catch(...)
    {
        cancelAllOperations();
        emit emitMessage("An unkown error occured.");

        return false;
    }

    // closes file pointers
    openFile.close();
    saveFile.close();

    // restores original text for the encode button
    emit emitRepurposeEncode(false);

    // enables most UI elements
    emit emitUIElements(true);

    // sets the running flag to be false
    runningOrNot = false;

    // logs operation completion if the operation has not been cancelled
    if(!brokenOperation)
        emit emitCommandLog("The operation has completed successfully!");

    return true;
}
