#ifndef DATAPERSISTENCE_H
#define DATAPERSISTENCE_H

#include <fstream>
#include <sstream>
#include <string>
#include "Store.h"

class DataPersistence {
private:
    static const std::string USER_FILE;
    static const std::string TRANSACTION_FILE;

public:
    static void loadData() {
        std::ifstream userFile(USER_FILE);
        if (!userFile.is_open()) return;
        std::string line;
        while (std::getline(userFile, line)) {
            if (line.empty()) continue;
            std::vector<std::string> parts;
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, ',')) parts.push_back(token);
            if (parts.size() >= 4) {
                std::string type = parts[0];
                std::string id = parts[1];
                std::string username = parts[2];
                std::string password = parts[3];
                if (type == "BUYER") Store::getInstance().registerUser(username, password, false);
                else if (type == "SELLER") Store::getInstance().registerUser(username, password, true);
            }
        }
    }

    static void saveData() {
        std::ofstream userFile(USER_FILE);
        if (userFile.is_open()) {
            for (const auto &pair : Store::getInstance().getUsers()) {
                userFile << pair.second->toString() << "\n";
            }
            userFile.close();
        }

        std::ofstream transactionFile(TRANSACTION_FILE);
        if (transactionFile.is_open()) {
            for (const auto &pair : Store::getInstance().getStoreTransactions()) {
                transactionFile << pair.second.toString() << "\n";
            }
            transactionFile.close();
        }
    }
};

const std::string DataPersistence::USER_FILE = "users.dat";
const std::string DataPersistence::TRANSACTION_FILE = "transactions.dat";

#endif