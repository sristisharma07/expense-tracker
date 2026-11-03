# Smart Digital Expense Tracker with Budget Prediction

BCA I Semester (Session 2026-2027) - Lab Sheet 09, COER University, Roorkee

A menu-driven C++ console program that records daily expenses, categorizes
spending, tracks a monthly budget and predicts whether the budget will be exceeded.

## Objective
Build a smart expense management system that lets a student record daily expenses,
view category-wise and monthly reports, set a monthly budget, get warnings and
predict month-end spending.

## Algorithm / Logic
1. Load `users.txt` and `expenses.txt` into arrays of structures at start-up.
2. Show Register / Login menu. Login allows 3 attempts.
3. After login, show the expense menu in a loop until Logout.
4. **Add expense:** validate date, choose category, enter amount and description, append to the array, save to file, then run the budget-warning check.
5. **View history:** copy the user's expense indices, bubble-sort them by date, print.
6. **Search:** convert keyword and record text to lower case, use `string::find`.
7. **Delete:** find the ID, shift later elements left, reduce count, re-save file.
8. **Budget warning:** if spent > budget, show "EXCEEDED"; if spent >= 80% of budget, show an alert.
9. **Prediction:** `daily average = spent so far / today's day number`, then
   `predicted = daily average x days in month`. If predicted > budget, warn and
   suggest a safe daily limit: `(budget - spent) / remaining days`.

## Modules covered
User Registration, Login, Add Daily Expense, View History, Category-wise Expense,
Monthly Report, Budget Setting, Budget Warning, Expense Prediction, Search, Delete, File Saving.

## Concepts used
Variables, loops, arrays, structures, functions, file handling, strings, searching, sorting.

## How to compile and run
```
g++ -std=c++11 -o expense_tracker expense_tracker.cpp
./expense_tracker          # Windows: expense_tracker.exe
```

## Sample input / output
```
Register -> sana / pass1
Login -> Set budget 3000
Add: today, Food, 500, Lunch at canteen
Add: today, Travel, 900, Bus pass
Add: today, Books, 400, C++ book

Category-wise Expense
Food        500.00  (27.8%) #####
Travel      900.00  (50.0%) ##########
Books       400.00  (22.2%) ####
TOTAL      1800.00

Expense Prediction
Spent so far        : Rs. 1800.00
Average per day     : Rs. 600.00
Predicted month end : Rs. 18000.00
Monthly budget      : Rs. 3000.00
RESULT: WARNING! At this rate you will exceed your budget by about Rs. 15000.00.
```
(Figures depend on the date you run it, because prediction uses the system date.)

## Conclusion
The program meets all 12 modules. It uses file handling for permanent storage,
structures and arrays for data, and sorting/searching for reports. Passwords are
stored as plain text here for simplicity; hashing is a future enhancement along with
AI-based prediction, a mobile app, a QR bill scanner and cloud backup.
