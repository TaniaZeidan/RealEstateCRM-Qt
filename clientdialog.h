#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <QRegularExpressionValidator>
#include "Client.h"

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog {
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr);
    ~ClientDialog();

    void    setClient(const Client &c);
    Client  getClient() const;

    void accept() override;

private:
    Ui::ClientDialog *ui;

    // Input validators
    QRegularExpressionValidator *nameValidator;       // no digits in names
    QRegularExpressionValidator *phoneValidator;      // exactly 8 digits
    QRegularExpressionValidator *emailValidator;      // simple email format
    QRegularExpressionValidator *budgetTypeValidator; // "rent" or "buy"
};

#endif // CLIENTDIALOG_H
