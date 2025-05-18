#include "propertydialog.h"
#include "ui_propertydialog.h"
#include "Exceptions.h"                // for ValidationException

#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QMessageBox>

PropertyDialog::PropertyDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyDialog),
    placeValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^[^\d]+$)")), this))
    // only these three types (case‑insensitive)
    , typeValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^(land|house|apartment)$)"),
                             QRegularExpression::CaseInsensitiveOption),
          this))
    // sale or rent
    , listingValidator(new QRegularExpressionValidator(
          QRegularExpression(QStringLiteral(R"(^(sale|rent)$)"),
                             QRegularExpression::CaseInsensitiveOption),
          this))
{
    ui->setupUi(this);

    // enforce positive size/price
    ui->sizeSpin ->setMinimum(0.01);
    ui->priceSpin->setMinimum(0.01);

    // bedrooms & bathrooms >= 0
    ui->bedsSpin ->setMinimum(0);
    ui->bathsSpin->setMinimum(0);

    // install line‑edit validators
    ui->placeEdit        ->setValidator(placeValidator);
    ui->propertyTypeEdit ->setValidator(typeValidator);
    ui->listingTypeEdit  ->setValidator(listingValidator);
}

PropertyDialog::~PropertyDialog(){
    delete ui;
}

void PropertyDialog::setProperty(const Property &p) {
    ui->sizeSpin         ->setValue(p.getSizeSqm());
    ui->priceSpin        ->setValue(p.getPrice());
    ui->propertyTypeEdit ->setText(QString::fromStdString(p.getPropertyType()));
    ui->bedsSpin         ->setValue(p.getBedrooms());
    ui->bathsSpin        ->setValue(p.getBathrooms());
    ui->placeEdit        ->setText(QString::fromStdString(p.getPlace()));
    ui->availCheck       ->setChecked(p.getAvailability());
    ui->listingTypeEdit  ->setText(QString::fromStdString(p.getListingType()));
}

Property PropertyDialog::getProperty() const {
    Property p;
    p.setSizeSqm     ( ui->sizeSpin  ->value() );
    p.setPrice       ( ui->priceSpin ->value() );
    p.setPropertyType( ui->propertyTypeEdit->text().toStdString() );
    p.setBedrooms    ( ui->bedsSpin  ->value() );
    p.setBathrooms   ( ui->bathsSpin ->value() );
    p.setPlace       ( ui->placeEdit ->text().toStdString() );
    p.setAvailability( ui->availCheck->isChecked() );
    p.setListingType ( ui->listingTypeEdit->text().toStdString() );

    // final validation
    if (!p.isValid())
        throw ValidationException("Property data is invalid");

    return p;
}

void PropertyDialog::accept() {
    // 1) Must be > 0
    if (ui->sizeSpin->value() <= 0) {
        QMessageBox::warning(this, "Validation Error", "Size must be greater than 0.");
        return;
    }
    if (ui->priceSpin->value() <= 0) {
        QMessageBox::warning(this, "Validation Error", "Price must be greater than 0.");
        return;
    }

    // 2) Non‑negative counts
    if (ui->bedsSpin->value() < 0 || ui->bathsSpin->value() < 0) {
        QMessageBox::warning(this, "Validation Error", "Bedrooms and bathrooms cannot be negative.");
        return;
    }

    // 3) Place not empty & matches regex
    if (ui->placeEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Place cannot be empty.");
        return;
    }
    if (!ui->placeEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Place may not contain digits.");
        return;
    }

    // 4) Property type
    if (!ui->propertyTypeEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error",
                             "Property type must be one of: land, house, apartment.");
        return;
    }

    // 5) Listing type
    if (!ui->listingTypeEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Validation Error", "Listing type must be 'sale' or 'rent'.");
        return;
    }

    QDialog::accept();
}
