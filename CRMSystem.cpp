// CRMSystem.cpp
#include "CRMSystem.h"
#include "sqlite3.h"
#include <sstream>
#include <algorithm>
#include <iostream>

// ── Static callbacks for sqlite3_exec ────────────────────────────────────────
int CRMSystem::agentLoadCallback(void* userPtr, int /*colCount*/, char** col, char**){
    CRMSystem* self = static_cast<CRMSystem*>(userPtr);
    Agent a;
    a.setId( std::stoi(col[0]) );
    a.setFirstName(col[1] ? col[1] : "");
    a.setLastName( col[2] ? col[2] : "");
    a.setPhone(    col[3] ? col[3] : "");
    a.setEmail(    col[4] ? col[4] : "");
    a.setStartDateFromString(col[5] ? col[5] : "");
    a.setEndDateFromString(  col[6] ? col[6] : "");
    self->agents.push_back(a);
    return 0;
}

int CRMSystem::clientLoadCallback(void* userPtr, int, char** col, char**){
    CRMSystem* self = static_cast<CRMSystem*>(userPtr);
    Client c;
    c.setId( std::stoi(col[0]) );
    c.setFirstName(col[1] ? col[1] : "");
    c.setLastName( col[2] ? col[2] : "");
    c.setPhone(    col[3] ? col[3] : "");
    c.setEmail(    col[4] ? col[4] : "");
    c.setIsMarried(std::stoi(col[5]) != 0);
    c.setBudget(   std::stod(col[6]) );
    c.setBudgetType(col[7] ? col[7] : "");
    self->clients.push_back(c);
    return 0;
}

int CRMSystem::propertyLoadCallback(void* userPtr, int, char** col, char**){
    CRMSystem* self = static_cast<CRMSystem*>(userPtr);
    Property p;
    p.setId(         std::stoi(col[0]) );
    p.setSizeSqm(    std::stod(col[1]) );
    p.setPrice(      std::stod(col[2]) );
    p.setPropertyType(col[3] ? col[3] : "");
    p.setBedrooms(   std::stoi(col[4]) );
    p.setBathrooms(  std::stoi(col[5]) );
    p.setPlace(      col[6] ? col[6] : "");
    p.setAvailability(std::stoi(col[7]) != 0);
    p.setListingType(col[8] ? col[8] : "");
    self->properties.push_back(p);
    return 0;
}

int CRMSystem::contractLoadCallback(void* userPtr, int, char** col, char**){
    CRMSystem* self = static_cast<CRMSystem*>(userPtr);
    Contract c;
    c.setId(          std::stoi(col[0]) );
    c.setPropertyId(  std::stoi(col[1]) );
    c.setClientId(    std::stoi(col[2]) );
    c.setAgentId(     std::stoi(col[3]) );
    c.setPrice(       std::stod(col[4]) );
    c.setStartDateFromString(col[5] ? col[5] : "");
    c.setEndDateFromString(  col[6] ? col[6] : "");
    c.setContractType(col[7] ? col[7] : "");
    c.setIsActive(   std::stoi(col[8]) != 0 );
    self->contracts.push_back(c);
    return 0;
}

// ── Constructor / Destructor ────────────────────────────────────────────────
CRMSystem::CRMSystem()
    : db("real_estate.db")
    , nextAgentId(1)
    , nextClientId(1)
    , nextPropertyId(1)
    , nextContractId(1)
{
    // Create tables if needed
    db.execute("CREATE TABLE IF NOT EXISTS Agents    (ID INTEGER PRIMARY KEY, FirstName TEXT, LastName TEXT, Phone TEXT, Email TEXT, StartDate TEXT, EndDate TEXT);");
    db.execute("CREATE TABLE IF NOT EXISTS Clients   (ID INTEGER PRIMARY KEY, FirstName TEXT, LastName TEXT, Phone TEXT, Email TEXT, IsMarried INTEGER, Budget REAL, BudgetType TEXT);");
    db.execute("CREATE TABLE IF NOT EXISTS Properties(ID INTEGER PRIMARY KEY, SizeSqm REAL, Price REAL, Type TEXT, Bedrooms INTEGER, Bathrooms INTEGER, Place TEXT, Available INTEGER, ListingType TEXT);");
    db.execute("CREATE TABLE IF NOT EXISTS Contracts (ID INTEGER PRIMARY KEY, PropertyId INTEGER, ClientId INTEGER, AgentId INTEGER, Price REAL, StartDate TEXT, EndDate TEXT, ContractType TEXT, IsActive INTEGER);");

    // Load existing data
    loadData();
}

CRMSystem::~CRMSystem() {
    // DatabaseManager’s destructor will close SQLite cleanly
}

// ── loadData: populate in‑memory vectors from SQLite ────────────────────────
void CRMSystem::loadData() {
    agents.clear();
    clients.clear();
    properties.clear();
    contracts.clear();

    char* err = nullptr;

    sqlite3_exec(db.getDbHandle(),
                 "SELECT ID,FirstName,LastName,Phone,Email,StartDate,EndDate FROM Agents;",
                 agentLoadCallback, this, &err);
    if (err) { std::cerr<<"Load Agents error: "<<err<<"\n"; sqlite3_free(err); }
    for (auto &a : agents) nextAgentId = std::max(nextAgentId, a.getId()+1);

    sqlite3_exec(db.getDbHandle(),
                  "SELECT ID,FirstName,LastName,Phone,Email,IsMarried,Budget,BudgetType FROM Clients;",
                  clientLoadCallback, this, &err);
    if (err) { sqlite3_free(err); }
    for (auto &c : clients) nextClientId = std::max(nextClientId, c.getId()+1);

    sqlite3_exec(db.getDbHandle(),
                  "SELECT ID,SizeSqm,Price,Type,Bedrooms,Bathrooms,Place,Available,ListingType FROM Properties;",
                  propertyLoadCallback, this, &err);
    if (err) { sqlite3_free(err); }
    for (auto &p : properties) nextPropertyId = std::max(nextPropertyId, p.getId()+1);

    sqlite3_exec(db.getDbHandle(),
                  "SELECT ID,PropertyId,ClientId,AgentId,Price,StartDate,EndDate,ContractType,IsActive FROM Contracts;",
                  contractLoadCallback, this, &err);
     if (err) { sqlite3_free(err); }
     for (auto &c : contracts) nextContractId = std::max(nextContractId, c.getId()+1);
}

// ── AGENT CRUD ──────────────────────────────────────────────────────────────
void CRMSystem::addAgent(const Agent &agent) {
    Agent a = agent;
    if (a.getId() < 0) a.setId(nextAgentId++);
    if (!a.isValid()) throw ValidationException("Invalid agent data.");

    agents.push_back(a);

    std::stringstream ss;
    ss << "INSERT INTO Agents (ID,FirstName,LastName,Phone,Email,StartDate,EndDate) VALUES ("
       << a.getId() << ",'"
       << a.getFirstName() << "','"
       << a.getLastName()  << "','"
       << a.getPhone()     << "','"
       << a.getEmail()     << "','"
       << a.getStartDateString() << "','"
       << a.getEndDateString()   << "');";

    if (!db.execute(ss.str()))
        throw FileOperationException("real_estate.db", "insert agent");
}

bool CRMSystem::removeAgent(int agentId) {
    auto it = std::remove_if(agents.begin(), agents.end(),
                             [agentId](auto &a){ return a.getId() == agentId; });
    if (it == agents.end()) return false;
    agents.erase(it, agents.end());

    std::stringstream ss;
    ss << "DELETE FROM Agents WHERE ID=" << agentId << ";";
    if (!db.execute(ss.str()))
        throw FileOperationException("real_estate.db", "delete agent");
    return true;
}

Agent CRMSystem::searchAgentById(int agentId) const {
    for (auto &a : agents)
        if (a.getId() == agentId) return a;
    throw AgentNotFoundException(agentId);
}

bool CRMSystem::modifyAgent(const Agent &m) {
    for (auto &a : agents) {
        if (a.getId() == m.getId()) {
            a = m;
            std::stringstream ss;
            ss << "UPDATE Agents SET "
               << "FirstName='" << m.getFirstName() << "', "
               << "LastName='"  << m.getLastName()  << "', "
               << "Phone='"     << m.getPhone()     << "', "
               << "Email='"     << m.getEmail()     << "', "
               << "StartDate='" << m.getStartDateString() << "', "
               << "EndDate='"   << m.getEndDateString()   << "' "
               << "WHERE ID="   << m.getId() << ";";
            if (!db.execute(ss.str()))
                throw FileOperationException("real_estate.db", "update agent");
            return true;
        }
    }
    return false;
}

// ——— CLIENT CRUD ———————————————————————————————————————————————————————

void CRMSystem::addClient(const Client &client) {
    Client c = client;

    // 1) Assign ID if needed
    if (c.getId() < 0) {
        c.setId(nextClientId++);
    }

    // 2) Validate
    if (!c.isValid()) {
        throw ValidationException("Invalid client data.");
    }

    // 3) In‑memory
    clients.push_back(c);

    // 4) Persist to SQLite
    std::stringstream ss;
    ss << "INSERT INTO Clients "
          "(ID,FirstName,LastName,Phone,Email,IsMarried,Budget,BudgetType) "
          "VALUES ("
       << c.getId() << ",'"
       << c.getFirstName() << "','"
       << c.getLastName()  << "','"
       << c.getPhone()     << "','"
       << c.getEmail()     << "',"
       << (c.getIsMarried() ? 1 : 0) << ","
       <<  c.getBudget()   << ",'"
       << c.getBudgetType() << "');";

    if (!db.execute(ss.str())) {
        throw FileOperationException("real_estate.db", "insert client");
    }
}

bool CRMSystem::removeClient(int clientId) {
    // 1) In‑memory
    auto it = std::remove_if(clients.begin(), clients.end(),
                             [clientId](const Client &c){
                                 return c.getId() == clientId;
                             });
    if (it == clients.end()) {
        return false;  // nothing removed
    }
    clients.erase(it, clients.end());

    // 2) Persist deletion
    std::stringstream ss;
    ss << "DELETE FROM Clients WHERE ID=" << clientId << ";";
    if (!db.execute(ss.str())) {
        throw FileOperationException("real_estate.db", "delete client");
    }
    return true;
}

Client CRMSystem::searchClientById(int clientId) const {
    for (const auto &c : clients) {
        if (c.getId() == clientId) {
            return c;
        }
    }
    throw ClientNotFoundException(clientId);
}

bool CRMSystem::modifyClient(const Client &m) {
    // 1) Update in‑memory
    for (auto &c : clients) {
        if (c.getId() == m.getId()) {
            c = m;

            // 2) Persist update
            std::stringstream ss;
            ss << "UPDATE Clients SET "
               << "FirstName='" << m.getFirstName() << "', "
               << "LastName='"  << m.getLastName()  << "', "
               << "Phone='"     << m.getPhone()     << "', "
               << "Email='"     << m.getEmail()     << "', "
               << "IsMarried="  << (m.getIsMarried() ? 1 : 0) << ", "
               << "Budget="     << m.getBudget()    << ", "
               << "BudgetType='"<< m.getBudgetType() << "' "
               << "WHERE ID="   << m.getId() << ";";

            if (!db.execute(ss.str())) {
                throw FileOperationException("real_estate.db", "update client");
            }

            return true;
        }
    }

    // no matching ID
    return false;
}

// ——— PROPERTY CRUD ——————————————————————————————————————————————————

void CRMSystem::addProperty(const Property &property) {
    Property p = property;
    if (p.getId() < 0) p.setId(nextPropertyId++);
    if (!p.isValid()) throw ValidationException("Invalid property data.");

    // in‐memory
    properties.push_back(p);

    // persist
    std::stringstream ss;
    ss << "INSERT INTO Properties "
          "(ID,SizeSqm,Price,Type,Bedrooms,Bathrooms,Place,Available,ListingType) "
          "VALUES ("
       << p.getId() << ","
       << p.getSizeSqm() << ","
       << p.getPrice()   << ",'"
       << p.getPropertyType() << "',"
       << p.getBedrooms() << ","
       << p.getBathrooms()<< ",'"
       << p.getPlace()    << "',"
       << (p.getAvailability()?1:0) << ",'"
       << p.getListingType() << "');";

    if (!db.execute(ss.str()))
        throw FileOperationException("real_estate.db","insert property");
}

bool CRMSystem::removeProperty(int propertyId) {
    auto it = std::remove_if(properties.begin(), properties.end(),
                             [propertyId](auto &p){ return p.getId()==propertyId; });
    if (it==properties.end()) return false;
    properties.erase(it, properties.end());

    std::stringstream ss;
    ss << "DELETE FROM Properties WHERE ID="<<propertyId<<";";
    if (!db.execute(ss.str()))
        throw FileOperationException("real_estate.db","delete property");
    return true;
}

Property CRMSystem::searchPropertyById(int propertyId) const {
    for (auto &p : properties)
        if (p.getId()==propertyId) return p;
    throw PropertyNotFoundException(propertyId);
}

bool CRMSystem::modifyProperty(const Property &m) {
    for (auto &p : properties) {
        if (p.getId()==m.getId()) {
            p = m;
            std::stringstream ss;
            ss << "UPDATE Properties SET "
               << "SizeSqm="    << m.getSizeSqm() << ", "
               << "Price="      << m.getPrice()   << ", "
               << "Type='"      << m.getPropertyType() << "', "
               << "Bedrooms="   << m.getBedrooms() << ", "
               << "Bathrooms="  << m.getBathrooms()<< ", "
               << "Place='"     << m.getPlace()    << "', "
               << "Available="  << (m.getAvailability()?1:0) << ", "
               << "ListingType='"<< m.getListingType() << "' "
               << "WHERE ID="   << m.getId() << ";";
            if (!db.execute(ss.str()))
                throw FileOperationException("real_estate.db","update property");
            return true;
        }
    }
    return false;
}

// ── CONTRACT CRUD ────────────────────────────────────────────────────────────

void CRMSystem::addContract(const Contract &contract) {
    Contract c = contract;
    if (c.getId() < 0)
        c.setId(nextContractId++);
    if (!c.isValid())
        throw ValidationException("Invalid contract data.");

    // in‑memory
    contracts.push_back(c);

    // persist to SQLite
    std::stringstream ss;
    ss << "INSERT INTO Contracts "
          "(ID,PropertyId,ClientId,AgentId,Price,StartDate,EndDate,ContractType,IsActive) "
          "VALUES ("
       << c.getId() << ","
       << c.getPropertyId() << ","
       << c.getClientId()   << ","
       << c.getAgentId()    << ","
       << c.getPrice()      << ",'"
       << c.getStartDateString() << "','"
       << c.getEndDateString()   << "','"
       << c.getContractType()    << "',"
       << (c.getIsActive() ? 1 : 0)
       << ");";

    if (!db.execute(ss.str()))
        throw FileOperationException("real_estate.db", "insert contract");
}

bool CRMSystem::removeContract(int contractId) {
    // in‑memory
    auto it = std::remove_if(
        contracts.begin(), contracts.end(),
        [contractId](auto &c){ return c.getId() == contractId; });
    if (it == contracts.end()) return false;
    contracts.erase(it, contracts.end());

    // in database
    std::stringstream ss;
    ss << "DELETE FROM Contracts WHERE ID=" << contractId << ";";
    if (!db.execute(ss.str()))
        throw FileOperationException("real_estate.db", "delete contract");
    return true;
}

Contract CRMSystem::searchContractById(int contractId) const {
    for (auto &c : contracts) {
        if (c.getId() == contractId)
            return c;
    }
    throw ContractNotFoundException(contractId);
}

bool CRMSystem::modifyContract(const Contract &m) {
    for (auto &c : contracts) {
        if (c.getId() == m.getId()) {
            c = m;
            // persist
            std::stringstream ss;
            ss << "UPDATE Contracts SET "
               << "PropertyId="   << m.getPropertyId()   << ", "
               << "ClientId="     << m.getClientId()     << ", "
               << "AgentId="      << m.getAgentId()      << ", "
               << "Price="        << m.getPrice()        << ", "
               << "StartDate='"   << m.getStartDateString() << "', "
               << "EndDate='"     << m.getEndDateString()   << "', "
               << "ContractType='"<< m.getContractType()    << "', "
               << "IsActive="     << (m.getIsActive() ? 1 : 0)
               << " WHERE ID="    << m.getId() << ";";
            if (!db.execute(ss.str()))
                throw FileOperationException("real_estate.db", "update contract");
            return true;
        }
    }
    return false;
}


// ── Convenience getters for the UI ───────────────────────────────────────────
const std::vector<Agent>&    CRMSystem::getAllAgents()    const { return agents;    }
const std::vector<Client>&   CRMSystem::getAllClients()   const { return clients;   }
const std::vector<Property>& CRMSystem::getAllProperties()const { return properties;}
const std::vector<Contract>& CRMSystem::getAllContracts() const { return contracts; }

// ── createContract simply reuses addContract after validation ───────────────
// void CRMSystem::createContract(int, int pid, int cid, int aid,
//                                double price,
//                                const std::string &sd,
//                                const std::string &ed,
//                                const std::string &type,
//                                bool active)
// {
//     searchAgentById(aid);
//     searchClientById(cid);
//     searchPropertyById(pid);

//     Contract c(-1, pid, cid, aid, price, sd, ed, type, active);
//     if (!c.isValid()) throw ValidationException("Invalid contract data");
//     addContract(c);
// }
