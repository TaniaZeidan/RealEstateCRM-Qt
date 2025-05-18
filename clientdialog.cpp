#include "clientdialog.h"
#include "ui_clientdialog.h"
#include "Exceptions.h"
#include <QMessageBox>

ClientDialog::ClientDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ClientDialog),
    nameValidator(new QRegularExpressionValidator(
        QRegularExpression(QStringLiteral(R"(^\D+$)")), this))
    , phoneValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^\d{8}$)")), this))
    , emailValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)")), this))
    , budgetTypeValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^(rent|buy)$)"),
                             QRegularExpression::CaseInsensitiveOption),
          this))
{
    ui->setupUi(this);

    // install validators
    ui->firstNameEdit ->setValidator(nameValidator);
    ui->lastNameEdit  ->setValidator(nameValidator);
    ui->phoneEdit     ->setValidator(phoneValidator);
    ui->emailEdit     ->setValidator(emailValidator);
    ui->budgetTypeEdit->setValidator(budgetTypeValidator);

    // ensure budgetSpin >= 0
    ui->budgetSpin->setMinimum(0);
}

ClientDialog::~ClientDialog(){
    delete ui;
}

void ClientDialog::setClient(const Client &c) {
    ui->firstNameEdit  ->setText(QString::fromStdString(c.getFirstName()));
    ui->lastNameEdit   ->setText(QString::fromStdString(c.getLastName()));
    ui->phoneEdit      ->setText(QString::fromStdString(c.getPhone()));
    ui->emailEdit      ->setText(QString::fromStdString(c.getEmail()));
    ui->marriedCheck   ->setChecked(c.getIsMarried());
    ui->budgetSpin     ->setValue(c.getBudget());
    ui->budgetTypeEdit ->setText(QString::fromStdString(c.getBudgetType()));
}

Client ClientDialog::getClient() const {
    Client c;

    c.setFirstName(ui->firstNameEdit->text().toStdString());
    c.setLastName( ui->lastNameEdit->text().toStdString());
    c.setPhone(    ui->phoneEdit->text().toStdString());
    c.setEmail(    ui->emailEdit->text().toStdString());
    c.setIsMarried(ui->marriedCheck->isChecked());
    c.setBudget(   ui->budgetSpin->value());
    c.setBudgetType(ui->budgetTypeEdit->text().toStdString());

    // final sanity check against your existing logic
    if (!c.isValid())
        throw ValidationException("Client data is invalid");

    return c;
}

void ClientDialog::accept() {
    // 1) Required & basic checks
    if (ui->firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "First name cannot be empty.");
        return;
    }
    if (!ui->firstNameEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "First name cannot contain digits.");
        return;
    }

    if (ui->lastNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Last name cannot be empty.");
        return;
    }
    if (!ui->lastNameEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Last name cannot contain digits.");
        return;
    }

    if (!ui->phoneEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Phone must be exactly 8 digits.");
        return;
    }
    if (!ui->emailEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Invalid email format.");
        return;
    }

    if (ui->budgetSpin->value() < 0) {
        QMessageBox::warning(this, "Validation Error", "Budget must be non‑negative.");
        return;
    }
    if (!ui->budgetTypeEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Budget type must be 'rent' or 'buy'.");
        return;
    }

    // all good…
    QDialog::accept();
}
