#ifndef CONTRACTDIALOG_H
#define CONTRACTDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "Contract.h"
#include "Date.h"

namespace Ui {
class ContractDialog;
}

class ContractDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContractDialog(QWidget *parent = nullptr);
    ~ContractDialog();

    void     setContract(const Contract &c);
    Contract getContract() const;

     void accept() override;

private:
    Ui::ContractDialog *ui;

    // Validators
    QRegularExpressionValidator *dateValidator;  // YYYY‑MM‑DD
    QRegularExpressionValidator *typeValidator;  // sale|rent
};

#endif // CONTRACTDIALOG_H
