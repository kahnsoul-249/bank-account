//ID: 22110068
//Name : Huỳnh Minh Tài
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Lớp Transaction lưu thông tin giao dịch
class Transaction {
private:
    double amount; // Số tiền giao dịch
    string type;   // Loại giao dịch (deposit/withdrawal/transfer)
public:
    Transaction(double amt, const string& typ) 
        : amount(amt), type(typ) {}
    double getAmount() const { return amount; }
    string getType() const { return type; }
};

// Lớp cơ sở Account quản lý tài khoản ngân hàng
class Account {
protected:
    string accountNumber;       // Số tài khoản
    double balance;            // Số dư
    string ownerName;          // Tên chủ tài khoản
    vector<Transaction> transactionHistory; // Lịch sử giao dịch

    // Thêm giao dịch với số tiền có dấu
    void addSignedAmount(double signedAmt, const string& type) {
        balance += signedAmt;
        transactionHistory.push_back(Transaction(signedAmt, type));
    }

public:
    Account(const string& accNum, const string& ownName, double initBal = 0.0) 
        : accountNumber(accNum), balance(initBal), ownerName(ownName) {}
    virtual ~Account() {}

    // Nạp tiền vào tài khoản
    void deposit(double amount) {
        if (amount > 0) {
            addSignedAmount(amount, "deposit");
        } else {
            cout << "Deposit amount must be positive." << endl;
        }
    }

    // Rút tiền hoặc chuyển khoản (ảo để lớp con ghi đè)
    virtual bool debit(double amount, const string& type) {
        if (amount <= 0) {
            cout << "Debit amount must be positive." << endl;
            return false;
        }
        if (balance < amount) {
            cout << "Insufficient funds for debit." << endl;
            return false;
        }
        addSignedAmount(-amount, type);
        return true;
    }

    // Ghi có (nạp tiền hoặc nhận chuyển khoản)
    void credit(double amount, const string& type) {
        if (amount > 0) {
            addSignedAmount(amount, type);
        } else {
            cout << "Credit amount must be positive." << endl;
        }
    }

    // Rút tiền từ tài khoản
    bool withdraw(double amount) {
        return debit(amount, "withdrawal");
    }

    // Lấy số dư
    double getBalance() const { return balance; }

    // Hiển thị thông tin tài khoản
    void displayInfo() const {
        cout << "Account Number: " << accountNumber << endl;
        cout << "Owner: " << ownerName << endl;
        cout << "Balance: $" << balance << endl;
        cout << "Transaction History:" << endl;
        for (const auto& trans : transactionHistory) {
            cout << trans.getType() << ": $" << trans.getAmount() << endl;
        }
        cout << endl;
    }

    // Nạp chồng toán tử += để thêm giao dịch
    Account& operator+=(const Transaction& trans) {
        addSignedAmount(trans.getAmount(), trans.getType());
        return *this;
    }

    // Nạp chồng toán tử == để so sánh tài khoản
    bool operator==(const Account& other) const {
        return accountNumber == other.accountNumber;
    }
};

// Lớp SavingsAccount kế thừa từ Account, thêm lãi suất
class SavingsAccount : 
public Account {
private:
    double interestRate; // Lãi suất
public:
    SavingsAccount(const string& accNum, const string& ownName, double initBal = 0.0, double rate = 0.01) 
        : Account(accNum, ownName, initBal), interestRate(rate) {}

    // Ghi đè phương thức debit để thêm phí rút tiền
    bool debit(double amount, const string& type) override {
        if (amount <= 0) {
            cout << "Debit amount must be positive." << endl;
            return false;
        }
        double fee = (type == "withdrawal") ? 2.0 : 0.0;
        if (balance < amount + fee) {
            cout << "Insufficient funds for debit including fee." << endl;
            return false;
        }
        addSignedAmount(-amount, type);
        if (fee > 0) {
            addSignedAmount(-fee, "fee");
        }
        return true;
    }

    // Tính và cộng lãi suất
    void applyInterest() {
        double interest = balance * interestRate;
        if (interest > 0) {
            addSignedAmount(interest, "interest");
        }
    }
};

// Lớp Customer quản lý khách hàng và danh sách tài khoản
class Customer {
private:
    string name;            // Tên khách hàng
    string id;              // ID khách hàng
    vector<Account*> accounts; // Danh sách tài khoản

public:
    Customer(const string& n, const string& i) : name(n), id(i) {}
    ~Customer() {
        for (auto acc : accounts) delete acc; // Giải phóng bộ nhớ
    }

    // Mở tài khoản thường
    Account* openRegularAccount(const string& accNum, double initBal = 0.0) {
        Account* acc = new Account(accNum, name, initBal);
        accounts.push_back(acc);
        return acc;
    }

    // Mở tài khoản tiết kiệm
    SavingsAccount* openSavingsAccount(const string& accNum, double initBal = 0.0, double rate = 0.01) {
        SavingsAccount* acc = new SavingsAccount(accNum, name, initBal, rate);
        accounts.push_back(acc);
        return acc;
    }

    // Tính tổng số dư của tất cả tài khoản
    double getTotalBalance() const {
        double total = 0.0;
        for (const auto& acc : accounts) {
            total += acc->getBalance();
        }
        return total;
    }

    // Hiển thị thông tin khách hàng
    void displayInfo() const {
        cout << "Customer Name: " << name << endl;
        cout << "Customer ID: " << id << endl;
        cout << "Total Balance: $" << getTotalBalance() << endl;
        cout << "Owned Accounts:" << endl;
        for (const auto& acc : accounts) {
            acc->displayInfo();
        }
    }
};

// Hàm chuyển tiền giữa hai tài khoản
bool transfer(Account& from, Account& to, double amount) {
    if (from.debit(amount, "transfer")) {
        to.credit(amount, "transfer");
        return true;
    }
    return false;
}

// Hàm main để kiểm tra hệ thống
int main() {
    Customer cust("TaiHuynh", "C001"); // Tạo khách hàng
    Account* acc1 = cust.openRegularAccount("A001", 1000.0); // Mở tài khoản thường
    SavingsAccount* sav1 = cust.openSavingsAccount("S001", 5000.0, 0.05); // Mở tài khoản tiết kiệm

    acc1->deposit(200.0); // Nạp tiền
    sav1->deposit(300.0);
    acc1->withdraw(500.0); // Rút tiền
    sav1->withdraw(100.0); // Rút tiền (có phí $2)

    sav1->applyInterest(); // Tính lãi suất
    transfer(*acc1, *sav1, 300.0); // Chuyển tiền

    *acc1 += Transaction(50.0, "bonus"); // Dùng toán tử +=

    // So sánh tài khoản
    Account* acc2 = cust.openRegularAccount("A002", 700.0);
    cout << (acc1 == acc2 ? "Accounts A001 and A002 are equal." : 
                            "Accounts A001 and A002 are not equal.") << endl;

    cust.displayInfo(); // Hiển thị thông tin khách hàng
    return 0;
}