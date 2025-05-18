#ifndef CRMSYSTEM_H
#define CRMSYSTEM_H

#include <vector>
#include <string>

#include "Agent.h"
#include "Client.h"
#include "Property.h"
#include "Contract.h"
#include "Inspection.h"
#include "Exceptions.h"
#include "Date.h"
#include "DatabaseManager.h"    // ← your SQLite wrapper

class CRMSystem {
public:
    CRMSystem();
    ~CRMSystem();

    // AGENT CRUD
    void addAgent(const Agent &agent);
    bool removeAgent(int agentId);
    Agent searchAgentById(int agentId) const;
    bool modifyAgent(const Agent &modifiedAgent);
    const std::vector<Agent>& getAllAgents() const;

    // CLIENT CRUD
    void addClient(const Client &client);
    bool removeClient(int clientId);
    Client searchClientById(int clientId) const;
    bool modifyClient(const Client &modifiedClient);
    const std::vector<Client>& getAllClients() const;

    // PROPERTY CRUD
    void addProperty(const Property &property);
    bool removeProperty(int propertyId);
    Property searchPropertyById(int propertyId) const;
    bool modifyProperty(const Property &modifiedProperty);
    const std::vector<Property>& getAllProperties() const;

    // CONTRACT CRUD
    void addContract(const Contract &contract);
    bool removeContract(int contractId);
    Contract searchContractById(int contractId) const;
    bool modifyContract(const Contract &modifiedContract);
    const std::vector<Contract>& getAllContracts() const;

    // Create a contract from existing records
    void createContract(int /*ignored*/, int propertyId, int clientId, int agentId,
                        double price,
                        const std::string &startDate,
                        const std::string &endDate,
                        const std::string &contractType,
                        bool isActive);

private:
    // ——— Persistence ———
    DatabaseManager db;          // opens your SQLite DB in ctor

    // ——— In‑memory store ———
    std::vector<Agent>    agents;
    std::vector<Client>   clients;
    std::vector<Property> properties;
    std::vector<Contract> contracts;

    static int agentLoadCallback(void* userPtr, int colCount, char** colValues, char**);
    static int clientLoadCallback(void* userPtr, int colCount, char** colValues, char**);
    static int propertyLoadCallback(void* userPtr, int colCount, char** colValues, char**);
    static int contractLoadCallback(void* userPtr, int colCount, char** colValues, char**);

    // ——— ID counters ———
    int nextAgentId;
    int nextClientId;
    int nextPropertyId;
    int nextContractId;

    // ——— Helpers ———
    // loadVectorFromDB and saveVectorToDB now live in your .cpp,
    // called by ctor/dtor or each CRUD method.
    void loadData();   // run SELECTs to populate agents/clients/etc.
    void saveData();   // (optional) flush any batched changes
};

#endif // CRMSYSTEM_H
