#ifndef USER_H
#define USER_H

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "BankAccount.h"
#include "Transaction.h"
#include "DateUtility.h"

class User {
protected:
    std::string userId;
    std::string username;
    std::string password;
    std::string bankAccountId;
    std::shared_ptr<BankAccount> account;

public:
    User() = default;
    User(const std::string &id, const std::string &user, const std::string &pass)
        : userId(id), username(user), password(pass) {
        bankAccountId = "ACC_" + id;
        account = nullptr;
    }

    std::string getId() const { return userId; }
    std::string getUsername() const { return username; }
    std::string getBankAccountId() const { return bankAccountId; }
    std::shared_ptr<BankAccount> getAccount() const { return account; }

    void setAccount(std::shared_ptr<BankAccount> acc) { account = acc; }

    bool topup(double amount, const std::string &tId) {
        if (account) return account->topup(amount, tId);
        return false;
    }

    bool withdraw(double amount, const std::string &tId) {
        if (account) return account->withdraw(amount, tId);
        return false;
    }

    void displayCashFlow(int days) const {
        if (!account) {
            std::cout << "Akun bank belum terdaftar." << std::endl;
            return;
        }
        time_t threshold = DateUtility::getPastDays(days);
        std::vector<Transaction> filtered = account->getCashFlowSince(threshold);
        std::cout << "\n--- Cash Flow " << (days == 30 ? "Sebulan" : "Hari Ini") << " ---" << std::endl;
        for (const auto &t : filtered) {
            std::string typeStr = (t.getType() == TransactionType::TOPUP ? "TOPUP" : t.getType() == TransactionType::WITHDRAW ? "WITHDRAW" : "PURCHASE");
            std::cout << DateUtility::timeToString(t.getDate()) << " | Tipe: " << typeStr
                      << " | Jumlah: " << (t.getAmount() > 0 ? "+" : "") << t.getAmount() << std::endl;
        }
        std::cout << "Saldo Saat Ini: " << account->getBalance() << std::endl;
    }

    bool verifyPassword(const std::string &p) const { return password == p; }

    virtual std::string toString() const = 0;
    virtual ~User() = default;
};

#endif