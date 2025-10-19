#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include "BankAccount.h"

using BankAccountPtr = std::shared_ptr<BankAccount>;

class Bank {
private:
    std::map<std::string, BankAccountPtr> accounts;
    std::map<std::string, std::string> customerMap;
    std::vector<Transaction> allTransactions;

    Bank() = default;
    Bank(const Bank&) = delete;
    Bank& operator=(const Bank&) = delete;

public:
    static Bank& getInstance() {
        static Bank instance;
        return instance;
    }

    BankAccountPtr createAccount(const std::string &userId) {
        if (customerMap.count(userId)) {
            return accounts.at(customerMap.at(userId));
        }
        std::string accountId = "BA_" + userId;
        auto newAccount = std::make_shared<BankAccount>(accountId, userId);
        accounts[accountId] = newAccount;
        customerMap[userId] = accountId;
        return newAccount;
    }

    BankAccountPtr getAccount(const std::string &userId) {
        if (customerMap.count(userId)) {
            return accounts.at(customerMap.at(userId));
        }
        return nullptr;
    }

    bool processBankTransaction(const std::string &userId, double amount, TransactionType type) {
        BankAccountPtr account = getAccount(userId);
        if (!account) return false;
        std::string tId = "T" + std::to_string(allTransactions.size() + 1);
        bool success = false;
        if (type == TransactionType::TOPUP) success = account->topup(amount, tId);
        else if (type == TransactionType::WITHDRAW) success = account->withdraw(amount, tId);
        if (success) allTransactions.emplace_back(tId, userId, amount, type);
        return success;
    }

    bool transfer(const std::string &buyerId, const std::string &sellerId, double amount, const std::string &tId) {
        BankAccountPtr buyerAcc = getAccount(buyerId);
        BankAccountPtr sellerAcc = getAccount(sellerId);
        if (!buyerAcc || !sellerAcc) return false;
        if (!buyerAcc->debit(amount, tId)) return false;
        sellerAcc->credit(amount, tId);
        return true;
    }

    void listTransactionsWithinAWeek() const {
        time_t oneWeekAgo = DateUtility::getPastDays(7);
        std::cout << "\n--- Transaksi Bank (Topup/Withdraw) dalam Seminggu Terakhir ---" << std::endl;
        for (const auto &accPair : accounts) {
            const auto &account = accPair.second;
            for (const auto &t : account->getCashFlow()) {
                if (t.getDate() >= oneWeekAgo && (t.getType() == TransactionType::TOPUP || t.getType() == TransactionType::WITHDRAW)) {
                    std::cout << DateUtility::timeToString(t.getDate())
                              << " | Akun: " << account->getId()
                              << " | Tipe: " << (t.getType() == TransactionType::TOPUP ? "TOPUP" : "WITHDRAW")
                              << " | Jumlah: " << (t.getAmount() > 0 ? "+" : "") << t.getAmount() << std::endl;
                }
            }
        }
    }

    void listAllCustomers() const {
        std::cout << "\n--- Daftar Semua Pelanggan Bank ---" << std::endl;
        for (const auto &pair : customerMap) {
            std::cout << "User ID: " << pair.first << " | Account ID: " << pair.second << std::endl;
        }
    }

    void listDormantAccounts() const {
        std::cout << "\n--- Daftar Akun Dormant (Tidak ada transaksi dalam Sebulan) ---" << std::endl;
        time_t oneMonthAgo = DateUtility::getPastMonth();
        int count = 0;
        for (const auto &accPair : accounts) {
            const auto &account = accPair.second;
            const auto &cashFlow = account->getCashFlow();
            if (cashFlow.empty() || cashFlow.back().getDate() < oneMonthAgo) {
                std::cout << "Akun ID: " << account->getId() << " | Pemilik: " << account->getOwnerId() << std::endl;
                count++;
            }
        }
        if (count == 0) std::cout << "Tidak ada akun dormant." << std::endl;
    }

    void listTopNUsersToday(int n) const {
        std::map<std::string, int> userTransactionCount;
        time_t now = DateUtility::getCurrentTime();
        time_t startOfToday = now - (now % 86400);
        for (const auto &accPair : accounts) {
            const auto &account = accPair.second;
            for (const auto &t : account->getCashFlow()) {
                if (t.getDate() >= startOfToday) userTransactionCount[account->getOwnerId()]++;
            }
        }
        std::vector<std::pair<int, std::string>> sortedUsers;
        for (const auto &pair : userTransactionCount) sortedUsers.push_back({pair.second, pair.first});
        std::sort(sortedUsers.rbegin(), sortedUsers.rend());
        std::cout << "\n--- Top " << n << " Pengguna Paling Aktif Hari Ini ---" << std::endl;
        for (int i = 0; i < std::min((int)sortedUsers.size(), n); ++i) {
            std::cout << (i + 1) << ". User ID: " << sortedUsers[i].second
                      << " | Jumlah Transaksi: " << sortedUsers[i].first << std::endl;
        }
    }
};

#endif