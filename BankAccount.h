#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include <string>
#include <vector>
#include "Transaction.h"
#include "DateUtility.h"

class BankAccount {
private:
    std::string accountId;
    std::string ownerId;
    double balance;
    std::vector<Transaction> cashFlow;

public:
    BankAccount() = default;
    BankAccount(const std::string& accId, const std::string& ownId)
        : accountId(accId), ownerId(ownId), balance(0.0) {}

    std::string getId() const { return accountId; }
    std::string getOwnerId() const { return ownerId; }
    double getBalance() const { return balance; }
    const std::vector<Transaction>& getCashFlow() const { return cashFlow; }

    bool topup(double amount, const std::string& tId) {
        if (amount > 0) {
            balance += amount;
            cashFlow.emplace_back(tId, ownerId, amount, TransactionType::TOPUP);
            return true;
        }
        return false;
    }

    bool withdraw(double amount, const std::string& tId) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            cashFlow.emplace_back(tId, ownerId, -amount, TransactionType::WITHDRAW);
            return true;
        }
        return false;
    }

    bool debit(double amount, const std::string& tId) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            cashFlow.emplace_back(tId, ownerId, -amount, TransactionType::PURCHASE);
            return true;
        }
        return false;
    }

    bool credit(double amount, const std::string& tId) {
        if (amount > 0) {
            balance += amount;
            cashFlow.emplace_back(tId, ownerId, amount, TransactionType::PURCHASE);
            return true;
        }
        return false;
    }

    std::vector<Transaction> getCashFlowSince(time_t threshold) const {
        std::vector<Transaction> filtered;
        for (const auto& t : cashFlow) {
            if (t.getDate() >= threshold) filtered.push_back(t);
        }
        return filtered;
    }

    std::string toString() const {
        return accountId + "," + ownerId + "," + std::to_string(balance);
    }

    bool isDormant() const {
        if (cashFlow.empty()) return true;
        time_t oneMonthAgo = DateUtility::getPastMonth();
        return cashFlow.back().getDate() < oneMonthAgo;
    }
};

#endif