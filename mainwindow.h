#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "CRMSystem.h"
#include "agentdialog.h"
#include "clientdialog.h"
#include "propertydialog.h"
#include "contractdialog.h"
#include "Agent.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void loadAgents();
    void refreshAgentTable(const std::vector<Agent>& list);
    void on_addAgent_clicked();
    void on_editAgent_clicked();
    void on_deleteAgent_clicked();
    void on_agentSearchButton_clicked();

    void loadClients();
    void refreshClientsTable(const std::vector<Client>& list);
    void on_addClient_clicked();
    void on_editClient_clicked();
    void on_deleteClient_clicked();
    void on_clientSearchButton_clicked();

    void loadProperties();
    void refreshPropertiesTable(const std::vector<Property>& list);
    void on_addProperty_clicked();
    void on_editProperty_clicked();
    void on_deleteProperty_clicked();
    void on_propertySearchButton_clicked();

    void loadContracts();
    void refreshContractsTable(const std::vector<Contract>& list);
    void on_addContract_clicked();
    void on_editContract_clicked();
    void on_deleteContract_clicked();
    void on_contractSearchButton_clicked();

private:
    Ui::MainWindow *ui;

    /**
     * Backend CRM system instance holding all data.
     */
    CRMSystem crmSystem;
};

#endif // MAINWINDOW_H
