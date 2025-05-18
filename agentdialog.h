#ifndef AGENTDIALOG_H
#define AGENTDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "Agent.h"
#include "Date.h"

namespace Ui {
class AgentDialog;
}

class AgentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AgentDialog(QWidget *parent = nullptr);
    ~AgentDialog();

    /**
     * Populate the dialog fields from an existing Agent (for editing).
     */
    void setAgent(const Agent &agent);

    /**
     * Construct an Agent object from the current dialog fields.
     * The returned Agent will have no ID set; caller must assign it if needed.
     */
    Agent getAgent() const;

    void accept() override;
private:
    Ui::AgentDialog *ui;

    // Validators:
    QRegularExpressionValidator *phoneValidator;  // exactly 8 digits
    QRegularExpressionValidator *emailValidator;  // simple email check
    QRegularExpressionValidator *dateValidator;   // YYYY‑MM‑DD
};

#endif // AGENTDIALOG_H
