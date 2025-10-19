#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>
#include "Store.h"
#include "DataPersistence.h"
#include "Bank.h"

using UserPtr = std::shared_ptr<User>;
using BuyerPtr = std::shared_ptr<Buyer>;
using SellerPtr = std::shared_ptr<Seller>;

UserPtr current_user = nullptr;

static inline std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace((unsigned char)*start)) start++;
    auto end = s.end();
    do { end--; } while (end != start && std::isspace((unsigned char)*end));
    return std::string(start, end + 1);
}

void clear_input() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int get_int_input(const std::string &prompt) {
    int value;
    std::cout << prompt;
    while (!(std::cin >> value) || value <= 0) {
        std::cout << "Input tidak valid. Masukkan angka positif: ";
        std::cin.clear();
        clear_input();
    }
    clear_input();
    return value;
}

double get_double_input(const std::string &prompt) {
    double value;
    std::cout << prompt;
    while (!(std::cin >> value) || value <= 0.0) {
        std::cout << "Input tidak valid. Masukkan jumlah positif: ";
        std::cin.clear();
        clear_input();
    }
    clear_input();
    return value;
}

void menu_buyer() {
    BuyerPtr buyer = std::dynamic_pointer_cast<Buyer>(current_user);
    if (!buyer) return;
    int choice;
    do {
        std::cout << "\n--- Buyer Menu (" << buyer->getUsername() << ") ---" << std::endl;
        std::cout << "1. Topup Akun Bank" << std::endl;
        std::cout << "2. Withdraw Akun Bank" << std::endl;
        std::cout << "3. Lihat Cash Flow" << std::endl;
        std::cout << "4. Beli Item" << std::endl;
        std::cout << "5. Lihat Semua Pesanan" << std::endl;
        std::cout << "6. Cek Pengeluaran (k Hari Terakhir)" << std::endl;
        std::cout << "7. Logout" << std::endl;
        std::cout << "Pilih Opsi: ";
        if (!(std::cin >> choice)) { choice = 0; std::cin.clear(); }
        clear_input();

        switch (choice) {
            case 1: {
                double amount = get_double_input("Masukkan jumlah Topup: ");
                if (Bank::getInstance().processBankTransaction(buyer->getId(), amount, TransactionType::TOPUP))
                    std::cout << "Topup berhasil. Saldo baru: " << buyer->getAccount()->getBalance() << std::endl;
                else std::cout << "Topup gagal." << std::endl;
                break;
            }
            case 2: {
                double amount = get_double_input("Masukkan jumlah Withdraw: ");
                if (Bank::getInstance().processBankTransaction(buyer->getId(), amount, TransactionType::WITHDRAW))
                    std::cout << "Withdraw berhasil. Saldo baru: " << buyer->getAccount()->getBalance() << std::endl;
                else std::cout << "Withdraw gagal (Saldo tidak cukup)." << std::endl;
                break;
            }
            case 3: {
                int days;
                std::cout << "Pilih rentang waktu (1: Hari Ini, 30: Sebulan): ";
                if (!(std::cin >> days)) { days = 1; std::cin.clear(); }
                clear_input();
                buyer->displayCashFlow(days);
                break;
            }
            case 4: {
                std::string itemId;
                int qty;
                std::cout << "Masukkan Item ID yang ingin dibeli: ";
                std::getline(std::cin, itemId);
                itemId = trim(itemId);
                qty = get_int_input("Masukkan jumlah: ");
                Store::getInstance().purchaseItem(buyer, itemId, qty);
                break;
            }
            case 5: {
                int f;
                std::cout << "Filter Status (1: PAID, 2: COMPLETED, 3: CANCELLED): ";
                if (!(std::cin >> f)) { f = 1; std::cin.clear(); }
                clear_input();
                TransactionStatus filter = TransactionStatus::PAID;
                if (f == 2) filter = TransactionStatus::COMPLETED;
                if (f == 3) filter = TransactionStatus::CANCELLED;
                Store::getInstance().listOrders(buyer->getOrderIds(), filter);
                break;
            }
            case 6: {
                int k = get_int_input("Masukkan jumlah hari (k): ");
                Store::getInstance().checkSpending(buyer, k);
                break;
            }
            case 7:
                current_user = nullptr;
                std::cout << "Anda telah logout." << std::endl;
                break;
            default:
                std::cout << "Pilihan tidak valid." << std::endl;
        }
    } while (choice != 7);
}

void menu_seller() {
    SellerPtr seller = std::dynamic_pointer_cast<Seller>(current_user);
    if (!seller) { menu_buyer(); return; }
    int choice;
    do {
        std::cout << "\n--- Seller Menu (" << seller->getUsername() << ") ---" << std::endl;
        std::cout << "1. Akses Fitur Buyer" << std::endl;
        std::cout << "2. Kelola Item" << std::endl;
        std::cout << "3. Lihat Item Populer Bulanan" << std::endl;
        std::cout << "4. Lihat Pelanggan Loyal Bulanan" << std::endl;
        std::cout << "5. Logout" << std::endl;
        std::cout << "Pilih Opsi: ";
        if (!(std::cin >> choice)) { choice = 0; std::cin.clear(); }
        clear_input();

        switch (choice) {
            case 1: menu_buyer(); break;
            case 2: {
                int sub;
                std::cout << "1. Tambah Item Baru | 2. Tambah Stok | 3. Kurangi Stok: ";
                if (!(std::cin >> sub)) { sub = 0; std::cin.clear(); }
                clear_input();

                if (sub == 1) {
                    std::string itemId, name;
                    double price; int stock;
                    std::cout << "Item ID: "; std::getline(std::cin, itemId); itemId = trim(itemId);
                    std::cout << "Nama Item: "; std::getline(std::cin, name); name = trim(name);
                    price = get_double_input("Harga: ");
                    stock = get_int_input("Stok awal: ");
                    seller->registerNewItem(itemId, name, price, stock);
                    std::cout << "Item '" << name << "' berhasil ditambahkan." << std::endl;
                } else if (sub == 2) {
                    std::string itemId; int qty;
                    std::cout << "Item ID: "; std::getline(std::cin, itemId); itemId = trim(itemId);
                    qty = get_int_input("Jumlah tambahan: ");
                    if (seller->replenishStock(itemId, qty))
                        std::cout << "Stok berhasil diperbarui." << std::endl;
                    else std::cout << "Item tidak ditemukan." << std::endl;
                } else if (sub == 3) {
                    std::string itemId; int qty;
                    std::cout << "Item ID: "; std::getline(std::cin, itemId); itemId = trim(itemId);
                    qty = get_int_input("Jumlah yang dikurangi: ");
                    if (seller->discardStock(itemId, qty))
                        std::cout << "Stok berhasil dikurangi." << std::endl;
                    else std::cout << "Gagal mengurangi stok." << std::endl;
                }
                break;
            }
            case 3: {
                int k = get_int_input("Masukkan jumlah item populer yang ingin dilihat: ");
                Store::getInstance().discoverPopularItems(seller, k);
                break;
            }
            case 4:
                Store::getInstance().discoverLoyalCustomer(seller);
                break;
            case 5:
                current_user = nullptr;
                std::cout << "Anda telah logout." << std::endl;
                break;
            default:
                std::cout << "Pilihan tidak valid." << std::endl;
        }
    } while (choice != 5);
}

void menu_management() {
    int choice;
    do {
        std::cout << "\n--- Menu Management (Store & Bank) ---" << std::endl;
        std::cout << "1. List Transaksi Toko (k Hari)" << std::endl;
        std::cout << "2. List Transaksi Dibayar Belum Selesai" << std::endl;
        std::cout << "3. List Item Paling Sering Terjual" << std::endl;
        std::cout << "4. List Buyer Paling Aktif" << std::endl;
        std::cout << "5. List Seller Paling Aktif" << std::endl;
        std::cout << "6. Bank: Transaksi Seminggu Terakhir" << std::endl;
        std::cout << "7. Bank: Semua Pelanggan" << std::endl;
        std::cout << "8. Bank: Akun Dormant" << std::endl;
        std::cout << "9. Bank: Top N User Hari Ini" << std::endl;
        std::cout << "10. Kembali" << std::endl;
        std::cout << "Pilih Opsi: ";
        if (!(std::cin >> choice)) { choice = 0; std::cin.clear(); }
        clear_input();

        switch (choice) {
            case 1: {
                int k = get_int_input("Masukkan jumlah hari: ");
                Store::getInstance().listTransactionsLastKDays(k);
                break;
            }
            case 2: Store::getInstance().listPaidUncompletedTransactions(); break;
            case 3: {
                int m = get_int_input("Masukkan jumlah item teratas: ");
                Store::getInstance().listMostFrequentItems(m);
                break;
            }
            case 4: {
                int m = get_int_input("Masukkan jumlah buyer teratas: ");
                Store::getInstance().listMostActiveBuyers(m);
                break;
            }
            case 5: {
                int m = get_int_input("Masukkan jumlah seller teratas: ");
                Store::getInstance().listMostActiveSellers(m);
                break;
            }
            case 6: Bank::getInstance().listTransactionsWithinAWeek(); break;
            case 7: Bank::getInstance().listAllCustomers(); break;
            case 8: Bank::getInstance().listDormantAccounts(); break;
            case 9: {
                int n = get_int_input("Masukkan jumlah user teratas: ");
                Bank::getInstance().listTopNUsersToday(n);
                break;
            }
            case 10: return;
            default: std::cout << "Pilihan tidak valid." << std::endl;
        }
    } while (choice != 10);
}

void menu_main() {
    int choice;
    do {
        std::cout << "\n=== SIMULASI BUYER-SELLER ===" << std::endl;
        if (current_user) {
            if (std::dynamic_pointer_cast<Seller>(current_user)) menu_seller();
            else menu_buyer();
            continue;
        }
        std::cout << "1. Register Buyer" << std::endl;
        std::cout << "2. Register Seller" << std::endl;
        std::cout << "3. Login" << std::endl;
        std::cout << "4. Menu Management" << std::endl;
        std::cout << "5. Keluar & Simpan" << std::endl;
        std::cout << "Pilih Opsi: ";
        if (!(std::cin >> choice)) { choice = 0; std::cin.clear(); }
        clear_input();

        switch (choice) {
            case 1: {
                std::string user, pass;
                std::cout << "Username: "; std::getline(std::cin, user); user = trim(user);
                std::cout << "Password: "; std::getline(std::cin, pass); pass = trim(pass);
                Store::getInstance().registerUser(user, pass, false);
                break;
            }
            case 2: {
                std::string user, pass;
                std::cout << "Username: "; std::getline(std::cin, user); user = trim(user);
                std::cout << "Password: "; std::getline(std::cin, pass); pass = trim(pass);
                Store::getInstance().registerUser(user, pass, true);
                break;
            }
            case 3: {
                std::string user, pass;
                std::cout << "Username: "; std::getline(std::cin, user); user = trim(user);
                std::cout << "Password: "; std::getline(std::cin, pass); pass = trim(pass);
                current_user = Store::getInstance().login(user, pass);
                break;
            }
            case 4: menu_management(); break;
            case 5:
                std::cout << "Menyimpan data dan keluar..." << std::endl;
                DataPersistence::saveData();
                return;
            default:
                std::cout << "Pilihan tidak valid." << std::endl;
        }
    } while (true);
}

int main() {
    DataPersistence::loadData();
    menu_main();
    return 0;
}