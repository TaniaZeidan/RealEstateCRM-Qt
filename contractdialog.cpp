#include "contractdialog.h"
#include "ui_contractdialog.h"

#include "Exceptions.h"                // for ValidationException
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QDate>

ContractDialog::ContractDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ContractDialog),
    dateValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(\d{4}-\d{2}-\d{2})")), this))
    // only “sale” or “rent” (case‑insensitive)
    , typeValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^(sale|rent)$)"),
                             QRegularExpression::CaseInsensitiveOption),
          this))
{
    ui->setupUi(this);


    // require positive IDs
    ui->propertyidSpin->setMinimum(1);
    ui->clientidSpin  ->setMinimum(1);
    ui->agentidSpin   ->setMinimum(1);

    // price > 0
    ui->priceSpin     ->setMinimum(1);

    // install validators on the line‑edits
    ui->startDateEdit    ->setValidator(dateValidator);
    ui->endDateEdit      ->setValidator(dateValidator);
    ui->contractTypeEdit ->setValidator(typeValidator);
}

ContractDialog::~ContractDialog()
{
    delete ui;
}

void ContractDialog::setContract(const Contract &c)
{
    ui->propertyidSpin   ->setValue(c.getPropertyId());
    ui->clientidSpin     ->setValue(c.getClientId());
    ui->agentidSpin      ->setValue(c.getAgentId());
    ui->priceSpin        ->setValue(c.getPrice());
    ui->startDateEdit    ->setText(QString::fromStdString(c.getStartDateString()));
    if (c.getEndDateString().empty()) {
        ui->endDateEdit->clear();
    } else {
        ui->endDateEdit->setText(QString::fromStdString(c.getEndDateString()));
    }
    ui->contractTypeEdit ->setText(QString::fromStdString(c.getContractType()));
    ui->activeCheck      ->setChecked(c.getIsActive());
}

Contract ContractDialog::getContract() const
{
    Contract c;
    c.setPropertyId(ui->propertyidSpin->value());
    c.setClientId  (ui->clientidSpin  ->value());
    c.setAgentId   (ui->agentidSpin   ->value());
    c.setPrice     (ui->priceSpin     ->value());

    // Parse start date (throws if format is wrong)
    const QString sd = ui->startDateEdit->text().trimmed();
    c.setStartDate(Date(sd.toStdString()));

    // Parse end date (allow blank → emptyDate)
    const QString ed = ui->endDateEdit->text().trimmed();
    if (ed.isEmpty()) {
        c.setEndDate(Date::emptyDate());
    } else {
        c.setEndDate(Date(ed.toStdString()));
    }

    // Read remaining fields
    c.setContractType(ui->contractTypeEdit->text().trimmed().toStdString());
    c.setIsActive     (ui->activeCheck->isChecked());

    // Final sanity check
    if (!c.isValid())
        throw ValidationException("Contract data is invalid");

    return c;
}

void ContractDialog::accept() {
    // 1) Dates must match or be blank
    if (!ui->startDateEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Start date must be YYYY‑MM‑DD.");
        return;
    }
    QString endTxt = ui->endDateEdit->text().trimmed();
    if (!endTxt.isEmpty() && !ui->endDateEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "End date must be YYYY‑MM‑DD or left blank.");
        return;
    }

    // 2) Logical: end ≥ start if given
    Date sd(ui->startDateEdit->text().toStdString());
    if (!endTxt.isEmpty()) {
        Date ed(endTxt.toStdString());
        if (ed < sd) {
            QMessageBox::warning(this, "Validation Error", "End date must not be before start date.");
            return;
        }
    }

    // 3) Contract type
    if (!ui->contractTypeEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Contract type must be 'sale' or 'rent'.");
        return;
    }

    QDialog::accept();
}
