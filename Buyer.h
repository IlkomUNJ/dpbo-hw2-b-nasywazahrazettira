#ifndef BUYER_H
#define BUYER_H

#include "User.h"
#include <vector>
#include <string>

class Buyer : public User {
private:
    std::vector<std::string> orderIds;

public:
    Buyer() = default;
    Buyer(const std::string& id, const std::string& user, const std::string& pass)
        : User(id, user, pass) {}

    void addOrderId(const std::string& orderId) { orderIds.push_back(orderId); }
    const std::vector<std::string>& getOrderIds() const { return orderIds; }

    std::string toString() const override {
        std::string orderList;
        for (const auto& id : orderIds) orderList += id + "|";
        if (!orderList.empty()) orderList.pop_back();
        return "BUYER," + userId + "," + username + "," + password + "," + orderList;
    }
};

#endif