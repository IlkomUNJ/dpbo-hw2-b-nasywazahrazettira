#ifndef SELLER_H
#define SELLER_H

#include "Buyer.h"
#include "Item.h"
#include <map>
#include <string>

class Seller : public Buyer {
private:
    std::map<std::string, Item> items;

public:
    Seller() = default;
    Seller(const std::string& id, const std::string& user, const std::string& pass)
        : Buyer(id, user, pass) {}

    void registerNewItem(const std::string& itemId, const std::string& name, double price, int stock) {
        items.emplace(itemId, Item(itemId, name, price, stock));
    }

    Item* getItem(const std::string& itemId) {
        if (items.count(itemId)) return &items.at(itemId);
        return nullptr;
    }

    bool replenishStock(const std::string& itemId, int quantity) {
        Item* item = getItem(itemId);
        if (item) {
            item->setStock(item->getStock() + quantity);
            return true;
        }
        return false;
    }

    bool discardStock(const std::string& itemId, int quantity) {
        Item* item = getItem(itemId);
        if (item && item->getStock() >= quantity) {
            item->setStock(item->getStock() - quantity);
            return true;
        }
        return false;
    }

    const std::map<std::string, Item>& getAllItems() const { return items; }

    std::string toString() const override {
        std::string base = Buyer::toString();
        std::string itemList;
        for (const auto& pair : items) itemList += pair.second.toString() + ";";
        if (!itemList.empty()) itemList.pop_back();
        base.replace(0, 5, "SELLER");
        return base + "|" + itemList;
    }
};

#endif