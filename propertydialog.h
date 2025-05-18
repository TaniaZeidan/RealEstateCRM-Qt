#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "Property.h"


namespace Ui {
class PropertyDialog;
}

class PropertyDialog : public QDialog {
    Q_OBJECT

public:
    explicit PropertyDialog(QWidget *parent = nullptr);
    ~PropertyDialog();

    /// Fill the form from an existing Property
    void     setProperty(const Property &p);
    /// Read the form back into a Property
    Property getProperty() const;

    void accept() override;


private:
    Ui::PropertyDialog *ui;

    // Validators for QLineEdits
    QRegularExpressionValidator *placeValidator;    // no digits
    QRegularExpressionValidator *typeValidator;     // land|house|apartment
    QRegularExpressionValidator *listingValidator;  // sale|rent
};

#endif // PROPERTYDIALOG_H
