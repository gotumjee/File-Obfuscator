#include "fileobfuscator.h"
#include "ui_fileobfuscator.h"

// calls the checkduplicate function when any of the digits are changed
void FObs::on_Combo0_currentIndexChanged(int index)
{
    // used to prevent "unused value" warnings
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo1_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo2_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo3_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo4_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo5_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo6_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo7_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo8_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo9_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo10_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo11_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo12_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo13_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo14_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::on_Combo15_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkIssues();
}

void FObs::checkIssues()
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

    // map used to store index values
    std::multimap <size_t, size_t> duplicateDigits;

    // assumes no duplicate hex digits at first
    issues = false;

    // stores all digit box indexes in a map and clears the status of all hex digits
    for(size_t i = 0; i < 16; ++i)
    {
        duplicateDigits.insert(std::pair <int, int> (comboIndexes[i]->currentIndex(), i));
        comboIndexes[i]->setStyleSheet("");
    }

    for(size_t i = 0; i < 16; ++i)
    {
        // checks if multiple entries exist for the same digits
        if(duplicateDigits.count(i) > 1)
        {
            // (GeeksforGeeks, 2019)
            for(auto j = duplicateDigits.begin(); j != duplicateDigits.end(); ++j)
            {
                // color duplicate digits and set the duplicates flag
                if(j -> first == i)
                {
                    comboIndexes[j -> second]->setStyleSheet("QComboBox{color: red;}");
                    issues = true;
                }
            }
        }
    }
}

// used to change the values from the signal
void FObs::changeDigits(int hexDigits[16])
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

    for(size_t i = 0; i < 16; ++i)
    {
        comboIndexes[i]->setCurrentIndex(hexDigits[i]);
    }

    commandLog("Digits have been set!", 4000, false);
}
