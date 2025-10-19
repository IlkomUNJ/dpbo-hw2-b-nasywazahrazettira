#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include "DateUtility.h"

enum class TransactionStatus {
    PAID,
    COMPLETED,
    CANCELLED
};

enum class TransactionType {
    PURCHASE,
    TOPUP,
    WITHDRAW
};

class Transaction {
private:
    std::string transactionId;
    std::string itemId;
    std::string buyerId;
    std::string sellerId;
    double amount;
    int quantity;
    time_t date;
    TransactionStatus status;
    TransactionType type;

public:
    Transaction() = default;

    Transaction(const std::string &tId, const std::string &iId, const std::string &bId, const std::string &sId,
                double amt, int qty)
        : transactionId(tId), itemId(iId), buyerId(bId), sellerId(sId),
          amount(amt), quantity(qty), date(DateUtility::getCurrentTime()),
          status(TransactionStatus::PAID), type(TransactionType::PURCHASE) {}

    Transaction(const std::string &tId, const std::string &uId, double amt, TransactionType t)
        : transactionId(tId), itemId("N/A"), buyerId(uId), sellerId("N/A"),
          amount(amt), quantity(1), date(DateUtility::getCurrentTime()),
          status(TransactionStatus::COMPLETED), type(t) {}

    std::string getId() const { return transactionId; }
    std::string getItemId() const { return itemId; }
    std::string getBuyerId() const { return buyerId; }
    std::string getSellerId() const { return sellerId; }
    double getAmount() const { return amount; }
    time_t getDate() const { return date; }
    TransactionStatus getStatus() const { return status; }
    TransactionType getType() const { return type; }
    int getQuantity() const { return quantity; }

    void setStatus(TransactionStatus s) { status = s; }

    std::string toString() const {
        return transactionId + "," + itemId + "," + buyerId + "," + sellerId + "," +
               std::to_string(amount) + "," + std::to_string(quantity) + "," +
               std::to_string(static_cast<long long>(date)) + "," +
               std::to_string(static_cast<int>(status)) + "," +
               std::to_string(static_cast<int>(type));
    }
};

#endif // TRANSACTION_H