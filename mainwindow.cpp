#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , crmSystem()               // your backend instance
{
    ui->setupUi(this);

    // —— Configure Agents table ——
    ui->agentsTable->setColumnCount(7);
    ui->agentsTable->setHorizontalHeaderLabels({
        "ID", "First Name", "Last Name", "Phone", "Email", "Start Date", "End Start"
    });
    ui->agentsTable->horizontalHeader()->setStretchLastSection(true);

    // —— Connect buttons to slots ——
    connect(ui->addAgent,            &QPushButton::clicked, this, &MainWindow::on_addAgent_clicked);
    connect(ui->editAgent,           &QPushButton::clicked, this, &MainWindow::on_editAgent_clicked);
    connect(ui->deleteAgent,         &QPushButton::clicked, this, &MainWindow::on_deleteAgent_clicked);
    connect(ui->agentSearchButton,   &QPushButton::clicked, this, &MainWindow::on_agentSearchButton_clicked);

    // —— Initial load ——
    loadAgents();

    // — Configure Clients table —
    ui->clientsTable->setColumnCount(8);
    ui->clientsTable->setHorizontalHeaderLabels({
        "ID",
        "First Name",
        "Last Name",
        "Phone",
        "Email",
        "Married",
        "Budget",
        "Budget Type"
    });
    ui->clientsTable->horizontalHeader()->setStretchLastSection(true);

    // — Connect client buttons to slots —
    connect(ui->addClient,           &QPushButton::clicked, this, &MainWindow::on_addClient_clicked);
    connect(ui->editClient,          &QPushButton::clicked, this, &MainWindow::on_editClient_clicked);
    connect(ui->deleteClient,        &QPushButton::clicked, this, &MainWindow::on_deleteClient_clicked);
    connect(ui->clientSearchButton,  &QPushButton::clicked, this, &MainWindow::on_clientSearchButton_clicked);

    // — Initial load of clients —
    loadClients();

    // — Configure Properties table —
    ui->propertiesTable->setColumnCount(8);
    ui->propertiesTable->setHorizontalHeaderLabels({
        "ID", "Size (sqm)", "Price",
        "Type", "Bedrooms", "Bathrooms",
        "Place", "Available", "Listing"
    });
    ui->propertiesTable->horizontalHeader()->setStretchLastSection(true);

    // — Connect property buttons to slots —
    connect(ui->addProperty,          &QPushButton::clicked, this, &MainWindow::on_addProperty_clicked);
    connect(ui->editProperty,         &QPushButton::clicked, this, &MainWindow::on_editProperty_clicked);
    connect(ui->deleteProperty,       &QPushButton::clicked, this, &MainWindow::on_deleteProperty_clicked);
    connect(ui->propertySearchButton, &QPushButton::clicked, this, &MainWindow::on_propertySearchButton_clicked);

    // — Initial load —
    loadProperties();

    // — Configure Contracts table —
    ui->contractsTable->setColumnCount(9);
    ui->contractsTable->setHorizontalHeaderLabels({
        "ID","Property","Client","Agent",
        "Price","Start","End","Type","Active"
    });
    ui->contractsTable->horizontalHeader()->setStretchLastSection(true);

    // — Connect contract buttons —
    connect(ui->addContract,          &QPushButton::clicked, this, &MainWindow::on_addContract_clicked);
    connect(ui->editContract,         &QPushButton::clicked, this, &MainWindow::on_editContract_clicked);
    connect(ui->deleteContract,       &QPushButton::clicked, this, &MainWindow::on_deleteContract_clicked);
    connect(ui->contractSearchButton, &QPushButton::clicked, this, &MainWindow::on_contractSearchButton_clicked);

    // — Initial load —
    loadContracts();


}

MainWindow::~MainWindow()
{
    delete ui;
}


//Agents
void MainWindow::loadAgents()
{
    // Load all agents from backend
    refreshAgentTable(crmSystem.getAllAgents());
}

void MainWindow::refreshAgentTable(const std::vector<Agent>& list)
{
    ui->agentsTable->setRowCount(0);
    for (const auto &a : list) {
        int row = ui->agentsTable->rowCount();
        ui->agentsTable->insertRow(row);
        ui->agentsTable->setItem(row, 0, new QTableWidgetItem(QString::number(a.getId())));
        ui->agentsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(a.getFirstName())));
        ui->agentsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(a.getLastName())));
        ui->agentsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(a.getPhone())));
        ui->agentsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(a.getEmail())));
        ui->agentsTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(a.getStartDateString())));
        ui->agentsTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(a.getEndDateString())));
    }
}

void MainWindow::on_addAgent_clicked()
{
    AgentDialog dlg(this);
    dlg.setWindowTitle("Add Agent");
    if (dlg.exec() == QDialog::Accepted) {
        try {
            crmSystem.addAgent(dlg.getAgent());
            loadAgents();
        } catch (const std::exception &e) {
            QMessageBox::warning(this, "Error Adding Agent", e.what());
        }
    }
}

void MainWindow::on_editAgent_clicked()
{
    int row = ui->agentsTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Edit Agent", "Please select an agent first.");
        return;
    }

    int id = ui->agentsTable->item(row, 0)->text().toInt();
    try {
        Agent existing = crmSystem.searchAgentById(id);
        AgentDialog dlg(this);
        dlg.setWindowTitle("Edit Agent");
        dlg.setAgent(existing);

        if (dlg.exec() == QDialog::Accepted) {
            Agent updated = dlg.getAgent();
            updated.setId(id);
            crmSystem.modifyAgent(updated);
            loadAgents();
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error Editing Agent", e.what());
    }
}

void MainWindow::on_deleteAgent_clicked()
{
    int row = ui->agentsTable->currentRow();
    if (row < 0) return;

    int id = ui->agentsTable->item(row, 0)->text().toInt();
    if (crmSystem.removeAgent(id)) {
        loadAgents();
    } else {
        QMessageBox::warning(this, "Error Deleting Agent", "Could not find or remove that agent.");
    }
}

void MainWindow::on_agentSearchButton_clicked()
{
    QString term = ui->agentSearchEdit->text().trimmed();
    std::vector<Agent> filtered;
    for (const auto &a : crmSystem.getAllAgents()) {
        if (QString::number(a.getId()) == term
            || QString::fromStdString(a.getFirstName()).contains(term, Qt::CaseInsensitive)
            || QString::fromStdString(a.getLastName()).contains(term,  Qt::CaseInsensitive))
        {
            filtered.push_back(a);
        }
    }
    refreshAgentTable(filtered);
}

//Clients
void MainWindow::loadClients() {
    refreshClientsTable(crmSystem.getAllClients());
}

void MainWindow::refreshClientsTable(const std::vector<Client>& list) {
    ui->clientsTable->setRowCount(0);
    for (const auto &c : list) {
        int r = ui->clientsTable->rowCount();
        ui->clientsTable->insertRow(r);
        ui->clientsTable->setItem(r, 0, new QTableWidgetItem(QString::number(c.getId())));
        ui->clientsTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(c.getFirstName())));
        ui->clientsTable->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(c.getLastName())));
        ui->clientsTable->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(c.getPhone())));
        ui->clientsTable->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(c.getEmail())));
        ui->clientsTable->setItem(r, 5, new QTableWidgetItem(c.getIsMarried() ? "Yes" : "No"));
        ui->clientsTable->setItem(r, 6, new QTableWidgetItem(QString::number(c.getBudget())));
        ui->clientsTable->setItem(r, 7, new QTableWidgetItem(QString::fromStdString(c.getBudgetType())));
    }
    ui->clientsTable->resizeColumnsToContents();
}

void MainWindow::on_addClient_clicked() {
    ClientDialog dlg(this);
    dlg.setWindowTitle("Add Client");
    if (dlg.exec() == QDialog::Accepted) {
        try {
            crmSystem.addClient(dlg.getClient());
            loadClients();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MainWindow::on_editClient_clicked() {
    auto sel = ui->clientsTable->currentRow();
    if (sel < 0) return;

    int id = ui->clientsTable->item(sel, 0)->text().toInt();
    try {
        Client c = crmSystem.searchClientById(id);
        ClientDialog dlg(this);
        dlg.setWindowTitle("Edit Client");
        dlg.setClient(c);
        if (dlg.exec() == QDialog::Accepted) {
            crmSystem.modifyClient(dlg.getClient());
            loadClients();
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Not found", e.what());
    }
}

void MainWindow::on_deleteClient_clicked() {
    int sel = ui->clientsTable->currentRow();
    if (sel < 0) return;

    int id = ui->clientsTable->item(sel, 0)->text().toInt();
    if (QMessageBox::question(this, "Delete",
                              "Remove client ID " + QString::number(id) + "?")
        == QMessageBox::Yes)
    {
        crmSystem.removeClient(id);
        loadClients();
    }
}

void MainWindow::on_clientSearchButton_clicked() {
    QString key = ui->clientSearchEdit->text().trimmed().toLower();
    auto all = crmSystem.getAllClients();
    std::vector<Client> filtered;
    for (auto &c : all) {
        if ( QString::fromStdString(c.getFirstName()).toLower().contains(key) ||
            QString::fromStdString(c.getLastName()).toLower().contains(key)  )
        {
            filtered.push_back(c);
        }
    }
    refreshClientsTable(filtered);
}


//Properties
void MainWindow::loadProperties() {
    refreshPropertiesTable(crmSystem.getAllProperties());
}

void MainWindow::refreshPropertiesTable(const std::vector<Property>& list) {
    ui->propertiesTable->setRowCount(0);
    for (auto &p : list) {
        int r = ui->propertiesTable->rowCount();
        ui->propertiesTable->insertRow(r);
        ui->propertiesTable->setItem(r,0,new QTableWidgetItem(QString::number(p.getId())));
        ui->propertiesTable->setItem(r,1,new QTableWidgetItem(QString::number(p.getSizeSqm())));
        ui->propertiesTable->setItem(r,2,new QTableWidgetItem(QString::number(p.getPrice())));
        ui->propertiesTable->setItem(r,3,new QTableWidgetItem(QString::fromStdString(p.getPropertyType())));
        ui->propertiesTable->setItem(r,4,new QTableWidgetItem(QString::number(p.getBedrooms())));
        ui->propertiesTable->setItem(r,5,new QTableWidgetItem(QString::number(p.getBathrooms())));
        ui->propertiesTable->setItem(r,6,new QTableWidgetItem(QString::fromStdString(p.getPlace())));
        ui->propertiesTable->setItem(r,7,new QTableWidgetItem(p.getAvailability()?"Yes":"No"));
        ui->propertiesTable->setItem(r,8,new QTableWidgetItem(QString::fromStdString(p.getListingType())));
    }
    ui->propertiesTable->resizeColumnsToContents();
}

void MainWindow::on_addProperty_clicked() {
    PropertyDialog dlg(this);
    dlg.setWindowTitle("Add Property");
    if (dlg.exec()==QDialog::Accepted) {
        try {
            crmSystem.addProperty(dlg.getProperty());
            loadProperties();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MainWindow::on_editProperty_clicked() {
    int sel = ui->propertiesTable->currentRow();
    if (sel < 0) return;
    int id = ui->propertiesTable->item(sel,0)->text().toInt();
    try {
        Property p = crmSystem.searchPropertyById(id);
        PropertyDialog dlg(this);
        dlg.setWindowTitle("Edit Property");
        dlg.setProperty(p);
        if (dlg.exec()==QDialog::Accepted) {
            crmSystem.modifyProperty(dlg.getProperty());
            loadProperties();
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Not Found", e.what());
    }
}

void MainWindow::on_deleteProperty_clicked() {
    int sel = ui->propertiesTable->currentRow();
    if (sel < 0) return;
    int id = ui->propertiesTable->item(sel,0)->text().toInt();
    if (QMessageBox::question(this,"Delete",
                              "Remove property ID "+QString::number(id)+"?")==QMessageBox::Yes)
    {
        crmSystem.removeProperty(id);
        loadProperties();
    }
}

void MainWindow::on_propertySearchButton_clicked() {
    QString key = ui->propertySearchEdit->text().toLower().trimmed();
    auto all = crmSystem.getAllProperties();
    std::vector<Property> f;
    for (auto &p: all) {
        if ( QString::fromStdString(p.getPropertyType()).toLower().contains(key) ||
            QString::fromStdString(p.getPlace()).toLower().contains(key) )
        {
            f.push_back(p);
        }
    }
    refreshPropertiesTable(f);
}

//Contracts
void MainWindow::loadContracts() {
    refreshContractsTable(crmSystem.getAllContracts());
}

void MainWindow::refreshContractsTable(const std::vector<Contract>& list) {
    ui->contractsTable->setRowCount(0);
    for (auto &c : list) {
        int r = ui->contractsTable->rowCount();
        ui->contractsTable->insertRow(r);
        ui->contractsTable->setItem(r,0,new QTableWidgetItem(QString::number(c.getId())));
        ui->contractsTable->setItem(r,1,new QTableWidgetItem(QString::number(c.getPropertyId())));
        ui->contractsTable->setItem(r,2,new QTableWidgetItem(QString::number(c.getClientId())));
        ui->contractsTable->setItem(r,3,new QTableWidgetItem(QString::number(c.getAgentId())));
        ui->contractsTable->setItem(r,4,new QTableWidgetItem(QString::number(c.getPrice())));
        ui->contractsTable->setItem(r,5,new QTableWidgetItem(QString::fromStdString(c.getStartDateString())));
        ui->contractsTable->setItem(r,6,new QTableWidgetItem(QString::fromStdString(c.getEndDateString())));
        ui->contractsTable->setItem(r,7,new QTableWidgetItem(QString::fromStdString(c.getContractType())));
        ui->contractsTable->setItem(r,8,new QTableWidgetItem(c.getIsActive()?"Yes":"No"));
    }
    ui->contractsTable->resizeColumnsToContents();
}

void MainWindow::on_addContract_clicked() {
    ContractDialog dlg(this);
    dlg.setWindowTitle("Add Contract");
    if (dlg.exec()==QDialog::Accepted) {
        try {
            crmSystem.addContract(dlg.getContract());
            loadContracts();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MainWindow::on_editContract_clicked() {
    int sel = ui->contractsTable->currentRow();
    if (sel<0) return;
    int id = ui->contractsTable->item(sel,0)->text().toInt();
    try {
        Contract c = crmSystem.searchContractById(id);
        ContractDialog dlg(this);
        dlg.setWindowTitle("Edit Contract");
        dlg.setContract(c);
        if (dlg.exec()==QDialog::Accepted) {
            crmSystem.modifyContract(dlg.getContract());
            loadContracts();
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Not Found", e.what());
    }
}

void MainWindow::on_deleteContract_clicked() {
    int sel = ui->contractsTable->currentRow();
    if (sel<0) return;
    int id = ui->contractsTable->item(sel,0)->text().toInt();
    if (QMessageBox::question(this,"Delete",
                              "Remove contract ID "+QString::number(id)+"?")==QMessageBox::Yes)
    {
        crmSystem.removeContract(id);
        loadContracts();
    }
}

void MainWindow::on_contractSearchButton_clicked() {
    QString key = ui->contractSearchEdit->text().toLower().trimmed();
    auto all = crmSystem.getAllContracts();
    std::vector<Contract> filtered;
    for (auto &c : all) {
        // search by property, client or agent ID
        if ( QString::number(c.getPropertyId()).contains(key)
            || QString::number(c.getClientId()).contains(key)
            || QString::number(c.getAgentId()).contains(key) )
        {
            filtered.push_back(c);
        }
    }
    refreshContractsTable(filtered);
}
