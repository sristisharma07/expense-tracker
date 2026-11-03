/*
 * Smart Digital Expense Tracker with Budget Prediction
 * BCA I Semester - Lab Sheet 09 (COER University, Roorkee)
 *
 * Concepts used: variables, loops, arrays, structures, functions,
 *                file handling, strings, searching, sorting.
 *
 * Data files (created automatically in the program folder):
 *   users.txt     ->  username|password|monthly_budget
 *   expenses.txt  ->  id|username|date(YYYY-MM-DD)|category|amount|description
 */

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <limits>
using namespace std;

// ---------------------------------------------------------------- constants
const int MAX_USERS = 100;
const int MAX_EXPENSES = 2000;
const int NUM_CATEGORIES = 6;
const string CATEGORIES[NUM_CATEGORIES] = {
    "Food", "Travel", "Books", "Entertainment", "Bills", "Other"};
const string USER_FILE = "users.txt";
const string EXPENSE_FILE = "expenses.txt";

// --------------------------------------------------------------- structures
struct User {
    string username;
    string password;
    double monthlyBudget;   // 0 means "not set yet"
};

struct Expense {
    int id;
    string username;
    string date;            // YYYY-MM-DD
    int category;           // index into CATEGORIES
    double amount;
    string description;
};

// ------------------------------------------------------------ global arrays
User users[MAX_USERS];
int userCount = 0;

Expense expenses[MAX_EXPENSES];
int expenseCount = 0;
int nextExpenseId = 1;

int currentUser = -1;       // index of logged-in user in users[]

// ------------------------------------------------------------ small helpers
string toLower(string s) {
    for (size_t i = 0; i < s.length(); i++)
        s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

// Split a line on '|' into parts[]; returns number of parts found.
int splitLine(const string &line, string parts[], int maxParts) {
    int count = 0;
    string current = "";
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == '|' && count < maxParts - 1) {
            parts[count++] = current;
            current = "";
        } else {
            current += line[i];
        }
    }
    parts[count++] = current;
    return count;
}

// Read an integer between lo and hi (inclusive), re-asking until valid.
int readInt(const string &prompt, int lo, int hi) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= lo && value <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid input. Enter a number from " << lo << " to " << hi << ".\n";
    }
}

// Read a positive amount.
double readAmount(const string &prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid amount. Enter a number greater than 0.\n";
    }
}

// Read a non-empty line that does not contain '|' (our file separator).
string readText(const string &prompt) {
    string text;
    while (true) {
        cout << prompt;
        getline(cin, text);
        for (size_t i = 0; i < text.length(); i++)
            if (text[i] == '|') text[i] = '/';
        if (!text.empty()) return text;
        cout << "  Input cannot be empty.\n";
    }
}

// ----------------------------------------------------------- date functions
bool isLeapYear(int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }

int daysInMonth(int month, int year) {
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

string twoDigits(int n) { return (n < 10 ? "0" : "") + to_string(n); }

void getToday(int &d, int &m, int &y) {
    time_t now = time(0);
    tm *t = localtime(&now);
    d = t->tm_mday;
    m = t->tm_mon + 1;
    y = t->tm_year + 1900;
}

string todayString() {
    int d, m, y;
    getToday(d, m, y);
    return to_string(y) + "-" + twoDigits(m) + "-" + twoDigits(d);
}

// Checks the format YYYY-MM-DD and that the date really exists.
bool isValidDate(const string &s) {
    if (s.length() != 10 || s[4] != '-' || s[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (s[i] < '0' || s[i] > '9') return false;
    }
    int y = atoi(s.substr(0, 4).c_str());
    int m = atoi(s.substr(5, 2).c_str());
    int d = atoi(s.substr(8, 2).c_str());
    if (y < 2000 || y > 2100 || m < 1 || m > 12) return false;
    return d >= 1 && d <= daysInMonth(m, y);
}

// ------------------------------------------------------------ file handling
void loadUsers() {
    ifstream fin(USER_FILE.c_str());
    string line, p[3];
    userCount = 0;
    while (getline(fin, line) && userCount < MAX_USERS) {
        if (line.empty()) continue;
        if (splitLine(line, p, 3) == 3) {
            users[userCount].username = p[0];
            users[userCount].password = p[1];
            users[userCount].monthlyBudget = atof(p[2].c_str());
            userCount++;
        }
    }
}

void saveUsers() {
    ofstream fout(USER_FILE.c_str());
    for (int i = 0; i < userCount; i++)
        fout << users[i].username << "|" << users[i].password << "|"
             << users[i].monthlyBudget << "\n";
}

void loadExpenses() {
    ifstream fin(EXPENSE_FILE.c_str());
    string line, p[6];
    expenseCount = 0;
    nextExpenseId = 1;
    while (getline(fin, line) && expenseCount < MAX_EXPENSES) {
        if (line.empty()) continue;
        if (splitLine(line, p, 6) == 6) {
            Expense &e = expenses[expenseCount++];
            e.id = atoi(p[0].c_str());
            e.username = p[1];
            e.date = p[2];
            e.category = atoi(p[3].c_str());
            e.amount = atof(p[4].c_str());
            e.description = p[5];
            if (e.category < 0 || e.category >= NUM_CATEGORIES) e.category = NUM_CATEGORIES - 1;
            if (e.id >= nextExpenseId) nextExpenseId = e.id + 1;
        }
    }
}

void saveExpenses() {
    ofstream fout(EXPENSE_FILE.c_str());
    for (int i = 0; i < expenseCount; i++)
        fout << expenses[i].id << "|" << expenses[i].username << "|"
             << expenses[i].date << "|" << expenses[i].category << "|"
             << expenses[i].amount << "|" << expenses[i].description << "\n";
}

// ------------------------------------------------------------- calculations
// Total spent by the current user in a month ("YYYY-MM").
double monthTotal(const string &month) {
    double total = 0;
    for (int i = 0; i < expenseCount; i++)
        if (expenses[i].username == users[currentUser].username &&
            expenses[i].date.substr(0, 7) == month)
            total += expenses[i].amount;
    return total;
}

void printHeader() {
    cout << left << setw(5) << "ID" << setw(12) << "Date" << setw(15) << "Category"
         << right << setw(11) << "Amount" << "  " << left << "Description\n";
    cout << string(62, '-') << "\n";
}

void printExpense(const Expense &e) {
    cout << left << setw(5) << e.id << setw(12) << e.date << setw(15)
         << CATEGORIES[e.category] << right << setw(11) << fixed << setprecision(2)
         << e.amount << "  " << left << e.description << "\n";
}

// ----------------------------------------------- Module 1: User Registration
void registerUser() {
    cout << "\n--- User Registration ---\n";
    if (userCount >= MAX_USERS) {
        cout << "User limit reached.\n";
        return;
    }
    string name = readText("Choose a username: ");
    for (int i = 0; i < userCount; i++) {
        if (toLower(users[i].username) == toLower(name)) {
            cout << "That username is already taken.\n";
            return;
        }
    }
    string pass = readText("Choose a password: ");
    users[userCount].username = name;
    users[userCount].password = pass;
    users[userCount].monthlyBudget = 0;
    userCount++;
    saveUsers();
    cout << "Registration successful! You can now log in.\n";
}

// ----------------------------------------------------- Module 2: Login System
bool loginUser() {
    cout << "\n--- Login ---\n";
    for (int attempt = 1; attempt <= 3; attempt++) {
        string name = readText("Username: ");
        string pass = readText("Password: ");
        for (int i = 0; i < userCount; i++) {
            if (users[i].username == name && users[i].password == pass) {
                currentUser = i;
                cout << "Welcome, " << name << "!\n";
                return true;
            }
        }
        cout << "Wrong username or password (" << attempt << "/3).\n";
    }
    return false;
}

// ---------------------------------------------------- Module 8: Budget Warning
void checkBudgetWarning(const string &month) {
    double budget = users[currentUser].monthlyBudget;
    if (budget <= 0) return;
    double spent = monthTotal(month);
    double percent = spent / budget * 100;
    if (spent > budget)
        cout << "\n*** WARNING: You have EXCEEDED your budget for " << month << "! (Spent "
             << spent << " of " << budget << ") ***\n";
    else if (percent >= 80)
        cout << "\n** ALERT: You have used " << fixed << setprecision(1) << percent
             << "% of your budget for " << month << ". **\n";
}

// -------------------------------------------------- Module 3: Add Daily Expense
void addExpense() {
    cout << "\n--- Add Daily Expense ---\n";
    if (expenseCount >= MAX_EXPENSES) {
        cout << "Expense storage is full.\n";
        return;
    }
    Expense e;
    e.id = nextExpenseId++;
    e.username = users[currentUser].username;

    string date;
    while (true) {
        date = readText("Date (YYYY-MM-DD, or type 'today'): ");
        if (toLower(date) == "today") date = todayString();
        if (isValidDate(date)) break;
        cout << "  Invalid date. Example: 2026-08-15\n";
    }
    e.date = date;

    for (int i = 0; i < NUM_CATEGORIES; i++)
        cout << "  " << (i + 1) << ". " << CATEGORIES[i] << "\n";
    e.category = readInt("Choose category (1-6): ", 1, NUM_CATEGORIES) - 1;
    e.amount = readAmount("Amount (Rs.): ");
    e.description = readText("Description: ");

    expenses[expenseCount++] = e;
    saveExpenses();   // Module 12: File Saving
    cout << "Expense added and saved (ID " << e.id << ").\n";
    checkBudgetWarning(e.date.substr(0, 7));
}

// ---------------------------------------------- Module 4: View Expense History
// Sorts a list of indices by date (bubble sort), oldest first.
void sortByDate(int idx[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (expenses[idx[j]].date > expenses[idx[j + 1]].date ||
                (expenses[idx[j]].date == expenses[idx[j + 1]].date &&
                 expenses[idx[j]].id > expenses[idx[j + 1]].id)) {
                int tmp = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = tmp;
            }
}

void viewHistory() {
    cout << "\n--- Expense History (sorted by date) ---\n";
    int idx[MAX_EXPENSES], n = 0;
    for (int i = 0; i < expenseCount; i++)
        if (expenses[i].username == users[currentUser].username) idx[n++] = i;
    if (n == 0) {
        cout << "No expenses recorded yet.\n";
        return;
    }
    sortByDate(idx, n);
    printHeader();
    double total = 0;
    for (int i = 0; i < n; i++) {
        printExpense(expenses[idx[i]]);
        total += expenses[idx[i]].amount;
    }
    cout << string(62, '-') << "\nTotal: Rs. " << fixed << setprecision(2) << total << "\n";
}

// -------------------------------------------- Module 5: Category-wise Expense
void categoryReport() {
    cout << "\n--- Category-wise Expense ---\n";
    double totals[NUM_CATEGORIES] = {0};
    double grand = 0;
    for (int i = 0; i < expenseCount; i++)
        if (expenses[i].username == users[currentUser].username) {
            totals[expenses[i].category] += expenses[i].amount;
            grand += expenses[i].amount;
        }
    if (grand == 0) {
        cout << "No expenses recorded yet.\n";
        return;
    }
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        double pct = totals[i] / grand * 100;
        cout << left << setw(15) << CATEGORIES[i] << right << setw(10) << fixed
             << setprecision(2) << totals[i] << "  (" << setprecision(1) << pct << "%) "
             << string((int)(pct / 5), '#') << "\n";
    }
    cout << string(40, '-') << "\n" << left << setw(15) << "TOTAL" << right << setw(10)
         << setprecision(2) << grand << "\n";
}

// ---------------------------------------------- Module 6: Monthly Expense Report
void monthlyReport() {
    cout << "\n--- Monthly Expense Report ---\n";
    string month;
    while (true) {
        month = readText("Enter month (YYYY-MM): ");
        if (month.length() == 7 && isValidDate(month + "-01")) break;
        cout << "  Invalid month. Example: 2026-08\n";
    }
    double catTotals[NUM_CATEGORIES] = {0};
    double total = 0;
    int count = 0;
    printHeader();
    for (int i = 0; i < expenseCount; i++)
        if (expenses[i].username == users[currentUser].username &&
            expenses[i].date.substr(0, 7) == month) {
            printExpense(expenses[i]);
            catTotals[expenses[i].category] += expenses[i].amount;
            total += expenses[i].amount;
            count++;
        }
    if (count == 0) {
        cout << "No expenses found for " << month << ".\n";
        return;
    }
    cout << string(62, '-') << "\nTransactions: " << count << "\nTotal spent : Rs. "
         << fixed << setprecision(2) << total << "\n\nBy category:\n";
    for (int i = 0; i < NUM_CATEGORIES; i++)
        if (catTotals[i] > 0)
            cout << "  " << left << setw(15) << CATEGORIES[i] << right << setw(10)
                 << catTotals[i] << "\n";
    double budget = users[currentUser].monthlyBudget;
    if (budget > 0) {
        cout << "\nBudget      : Rs. " << budget << "\n";
        if (total > budget) cout << "Status      : EXCEEDED by Rs. " << (total - budget) << "\n";
        else cout << "Status      : Within budget, Rs. " << (budget - total) << " left\n";
    }
}

// ------------------------------------------------------ Module 7: Budget Setting
void setBudget() {
    cout << "\n--- Budget Setting ---\n";
    double b = users[currentUser].monthlyBudget;
    if (b > 0) cout << "Current monthly budget: Rs. " << fixed << setprecision(2) << b << "\n";
    users[currentUser].monthlyBudget = readAmount("Enter new monthly budget (Rs.): ");
    saveUsers();
    cout << "Budget saved.\n";
}

// -------------------------------------------------- Module 9: Expense Prediction
void predictExpense() {
    cout << "\n--- Expense Prediction (current month) ---\n";
    double budget = users[currentUser].monthlyBudget;
    if (budget <= 0) {
        cout << "Please set a monthly budget first (menu option 7).\n";
        return;
    }
    int d, m, y;
    getToday(d, m, y);
    string month = to_string(y) + "-" + twoDigits(m);
    int totalDays = daysInMonth(m, y);
    double spent = monthTotal(month);
    double dailyAvg = spent / d;
    double predicted = dailyAvg * totalDays;

    cout << fixed << setprecision(2);
    cout << "Month               : " << month << " (day " << d << " of " << totalDays << ")\n";
    cout << "Spent so far        : Rs. " << spent << "\n";
    cout << "Average per day     : Rs. " << dailyAvg << "\n";
    cout << "Predicted month end : Rs. " << predicted << "\n";
    cout << "Monthly budget      : Rs. " << budget << "\n";

    if (spent > budget) {
        cout << "RESULT: Budget is ALREADY exceeded by Rs. " << (spent - budget) << "!\n";
    } else if (predicted > budget) {
        cout << "RESULT: WARNING! At this rate you will exceed your budget by about Rs. "
             << (predicted - budget) << ".\n";
        if (d < totalDays) {
            double safeDaily = (budget - spent) / (totalDays - d);
            cout << "TIP: Keep spending under Rs. " << safeDaily
                 << " per day for the rest of the month.\n";
        }
    } else {
        cout << "RESULT: Good news! You are on track to stay within budget"
             << " (about Rs. " << (budget - predicted) << " to spare).\n";
    }
}

// ----------------------------------------------------- Module 10: Search Expense
void searchExpense() {
    cout << "\n--- Search Expense ---\n";
    string key = toLower(readText("Enter keyword (description, category or date): "));
    bool found = false;
    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].username != users[currentUser].username) continue;
        string text = toLower(expenses[i].description + " " + CATEGORIES[expenses[i].category] +
                              " " + expenses[i].date);
        if (text.find(key) != string::npos) {
            if (!found) printHeader();
            printExpense(expenses[i]);
            found = true;
        }
    }
    if (!found) cout << "No matching expenses found.\n";
}

// ----------------------------------------------------- Module 11: Delete Expense
void deleteExpense() {
    cout << "\n--- Delete Expense ---\n";
    int id = readInt("Enter the expense ID to delete: ", 1, 1000000);
    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].id == id && expenses[i].username == users[currentUser].username) {
            cout << "Deleting: ";
            cout << expenses[i].date << " | " << CATEGORIES[expenses[i].category] << " | Rs. "
                 << expenses[i].amount << " | " << expenses[i].description << "\n";
            for (int j = i; j < expenseCount - 1; j++) expenses[j] = expenses[j + 1];
            expenseCount--;
            saveExpenses();
            cout << "Expense deleted and file updated.\n";
            return;
        }
    }
    cout << "No expense with that ID found for your account.\n";
}

// ----------------------------------------------------------------- menus
void userMenu() {
    int choice;
    do {
        cout << "\n========== EXPENSE TRACKER MENU ==========\n"
             << " 1. Add Daily Expense\n 2. View Expense History\n 3. Category-wise Expense\n"
             << " 4. Monthly Expense Report\n 5. Search Expense\n 6. Delete Expense\n"
             << " 7. Set Monthly Budget\n 8. Check Budget Status\n 9. Expense Prediction\n"
             << " 0. Logout\n==========================================\n";
        choice = readInt("Enter choice: ", 0, 9);
        switch (choice) {
            case 1: addExpense(); break;
            case 2: viewHistory(); break;
            case 3: categoryReport(); break;
            case 4: monthlyReport(); break;
            case 5: searchExpense(); break;
            case 6: deleteExpense(); break;
            case 7: setBudget(); break;
            case 8: {
                int d, m, y;
                getToday(d, m, y);
                string month = to_string(y) + "-" + twoDigits(m);
                double budget = users[currentUser].monthlyBudget;
                cout << fixed << setprecision(2) << "\nSpent in " << month << ": Rs. "
                     << monthTotal(month) << "\n";
                if (budget <= 0) cout << "No budget set yet (option 7).\n";
                else {
                    cout << "Budget: Rs. " << budget << "\n";
                    if (monthTotal(month) <= budget * 0.8) cout << "You are well within budget.\n";
                    checkBudgetWarning(month);
                }
                break;
            }
            case 9: predictExpense(); break;
            case 0: cout << "Logged out.\n"; break;
        }
    } while (choice != 0);
    currentUser = -1;
}

int main() {
    loadUsers();
    loadExpenses();
    int choice;
    do {
        cout << "\n****** SMART DIGITAL EXPENSE TRACKER ******\n"
             << " 1. Register\n 2. Login\n 0. Exit\n*******************************************\n";
        choice = readInt("Enter choice: ", 0, 2);
        if (choice == 1) registerUser();
        else if (choice == 2) {
            if (loginUser()) userMenu();
            else cout << "Too many failed attempts.\n";
        }
    } while (choice != 0);
    cout << "Goodbye! All data is saved.\n";
    return 0;
}
