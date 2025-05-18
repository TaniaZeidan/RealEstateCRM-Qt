#include "agentdialog.h"
#include "ui_agentdialog.h"
#include "ui_agentdialog.h"
#include <QMessageBox>

AgentDialog::AgentDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AgentDialog),
    phoneValidator(new QRegularExpressionValidator(QRegularExpression(R"(\d{8})"), this)),
    emailValidator(new QRegularExpressionValidator(
        QRegularExpression(R"([^@\s]+@[^@\s]+\.[^@\s]+)"), this)),
    dateValidator(new QRegularExpressionValidator(
        QRegularExpression(R"(\d{4}-\d{2}-\d{2})"), this))
{
    ui->setupUi(this);

    // Install validators
    ui->phoneEdit->setValidator(phoneValidator);
    ui->emailEdit->setValidator(emailValidator);
    ui->startDateEdit->setValidator(dateValidator);
    ui->endDateEdit  ->setValidator(dateValidator);

}

AgentDialog::~AgentDialog(){
    delete ui;
}

void AgentDialog::setAgent(const Agent &a) {
    ui->firstNameEdit->setText(QString::fromStdString(a.getFirstName()));
    ui->lastNameEdit ->setText(QString::fromStdString(a.getLastName()));
    ui->phoneEdit    ->setText(QString::fromStdString(a.getPhone()));
    ui->emailEdit    ->setText(QString::fromStdString(a.getEmail()));
    ui->startDateEdit->setText(
        QString::fromStdString(a.getStartDateString())
        );
    if (a.getEndDateString().empty()) {
        ui->endDateEdit->clear();
    } else {
        ui->endDateEdit->setText(
            QString::fromStdString(a.getEndDateString())
            );
    }
}

Agent AgentDialog::getAgent() const {
    Agent a;
    a.setFirstName(ui->firstNameEdit->text().toStdString());
    a.setLastName( ui->lastNameEdit->text().toStdString());
    a.setPhone(    ui->phoneEdit->text().toStdString());
    a.setEmail(    ui->emailEdit->text().toStdString());

    // Parse start date (must not be empty)

    QString sd = ui->startDateEdit->text().trimmed();
    a.setStartDate( Date(sd.toStdString()) );


    // Parse end date, allowing blank → emptyDate

    QString ed = ui->endDateEdit->text().trimmed();
    if (ed.isEmpty()) {
        a.setEndDate( Date::emptyDate() );
    } else {
        a.setEndDate( Date(ed.toStdString()) );
    }


    return a;
}


void AgentDialog::accept()
{
    // 1) Required fields
    if (ui->firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "First name cannot be empty.");
        return;
    }
    if (ui->lastNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Last name cannot be empty.");
        return;
    }

    // 2) Validator checks (they’ll show red text on failure, but we also catch here)
    if (!ui->phoneEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Phone must be exactly 8 digits.");
        return;
    }
    if (!ui->emailEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Email format is invalid.");
        return;
    }
    if (!ui->startDateEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Start date must be YYYY‑MM‑DD.");
        return;
    }
    // End date may be blank or must match
    if (!ui->endDateEdit->text().trimmed().isEmpty()
        && !ui->endDateEdit->hasAcceptableInput())
    {
        QMessageBox::warning(this, "Validation Error", "End date must be YYYY‑MM‑DD or blank.");
        return;
    }

    // 3) Logical check: start ≤ end
    Date sd(ui->startDateEdit->text().toStdString());
    QString edTxt = ui->endDateEdit->text().trimmed();
    if (!edTxt.isEmpty()) {
        Date ed(edTxt.toStdString());
        if (ed < sd) {
            QMessageBox::warning(this, "Validation Error", "End date must not be before start date.");
            return;
        }
    }

    QDialog::accept();
}
