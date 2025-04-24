#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#ifdef _WIN32
#include <conio.h>
#endif
using namespace std;

// Expense structure to represent each expense entry
struct Expense {
    string date;
    string category;
    string description;
    double amount;
    Expense* next;

    Expense(string d, string c, string desc, double amt)
        : date(d), category(c), description(desc), amount(amt), next(nullptr) {}
};

// ExpenseTracker class to handle all operations
class ExpenseTracker {
private:
    Expense* head;
    unordered_map<string, vector<Expense*>> categoryMap;
    const string filename = "expenses.csv";

public:
    ExpenseTracker() : head(nullptr) {
        loadExpensesFromFile(); // Load expenses from file on startup
    }

    ~ExpenseTracker() {
        saveExpensesToFile(); // Save expenses to file on exit
    }

    // Helper function to check if file exists
    bool fileExists(const string& filename) {
        ifstream file(filename);
        return file.good();  // Returns true if file exists
    }

    // Check for duplicate expense
    bool isDuplicateExpense(const string& date, const string& category, const string& description, double amount) {
        Expense* current = head;
        while (current) {
            if (current->date == date && current->category == category && current->description == description && current->amount == amount) {
                return true;  // Duplicate expense found
            }
            current = current->next;
        }
        return false;  // No duplicate found
    }

    void deleteExpense(const string& date, const string& category, const string& description, double amount) {
        // Check if the list is empty
        if (!head) {
            cout << "No expenses recorded yet.\n";
        }

        // Search for the expense in the linked list
        Expense* current = head;
        Expense* prev = nullptr;
        while (current) {
            if (current->date == date && current->category == category &&
                current->description == description && current->amount == amount) {
                // Found the expense to delete
                if (prev) {
                    prev->next = current->next; // Bypass the current node
                } else {
                    head = current->next; // Update head if deleting the first node
                }

                // Remove from categoryMap
                auto& vec = categoryMap[category];
                vec.erase(remove(vec.begin(), vec.end(), current), vec.end());

                // Free memory
                delete current;

                cout << "Expense deleted successfully.\n";
                saveExpensesToFile(); // Save updated list to the file
            }
            prev = current;
            current = current->next;
        }

        cout << "Expense not found.\n";
        };
    

    // Load expenses from file on startup
    void loadExpensesFromFile() {
    if (fileExists(filename)) {
        ifstream file(filename);
        string line;
        getline(file, line); // Skip header row
        while (getline(file, line)) {
            size_t pos = 0;
            string date, category, description;
            double amount;

            // Parse each line (date, category, description, amount)
            pos = line.find(',');
            date = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find(',');
            category = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find(',');
            description = line.substr(0, pos);
            line.erase(0, pos + 1);

            amount = stod(line);

            // Add expense if it's not a duplicate
            if (!isDuplicateExpense(date, category, description, amount)) {
                addExpenseToList(date, category, description, amount);
            }
        }
        file.close();
        cout << "Expenses loaded from file successfully.\n";
        } else {
            cout << "No previous expense data found.\n";
        }
    }

    // Add new expense entry
    void addExpenseToList(const string& date, const string& category, const string& description, double amount) {
    if (isDuplicateExpense(date, category, description, amount)) {
        cout << "Expense already exists. Not added again.\n";
        return;  // Prevent duplicate entry
    }

    // Create a new expense and add it to the linked list
    Expense* newExpense = new Expense(date, category, description, amount);
    newExpense->next = head;
    head = newExpense;

    // Update the category map for faster category searches
    categoryMap[category].push_back(newExpense);

    // Save expense to file immediately
    ofstream file(filename, ios::app);
    if (file) {
        file << date << "," << category << "," << description << "," << amount << "\n";
        file.close();
    } else {
        cout << "Error saving expense to file.\n";
    }
    cout << "Expense added successfully!\n";
    }
    void calculateTotalSpent() const {
            double total = 0.0;
            Expense* current = head;
            while (current) {
                total += current->amount;
                current = current->next;
            }
            cout << "Total amount spent: $" << total << "\n";
        }
    // Display all expenses
    void displayExpenses() const {
        if (!head) {
            cout << "No expenses recorded yet.\n";
            return;
        }
        cout << "Date\t\tCategory\tDescription\t\t\tAmount\n";
        cout << "-------------------------------------------------------\n";
        Expense* current = head;
        while (current) {
            cout << current->date << "\t" << current->category << "\t\t" << current->description << "\t\t\t$" << current->amount << "\n";
            current = current->next;
        }
    }

    // Search expenses by category
    void searchByCategory(const string& category) const {
        try{auto it = categoryMap.find(category);
        if (it == categoryMap.end()) {
            cout << "No expenses found for category: " << category << "\n";
            return;
        }
        cout << "Expenses for category: " << category << "\n";
        for (auto& expense : it->second) {
            cout << expense->date << "\t" << expense->description << "\t\t$" << expense->amount << "\n";
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    }

    // Sort expenses by amount (Merge Sort)
    Expense* mergeSort(Expense* head) {
        if (!head || !head->next) return head;
        Expense* mid = getMiddle(head);
        Expense* left = head;
        Expense* right = mid->next;
        mid->next = nullptr;
        return merge(mergeSort(left), mergeSort(right));
    }

    Expense* getMiddle(Expense* head) {
        if (!head) return head;
        Expense* slow = head;
        Expense* fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }

    Expense* merge(Expense* left, Expense* right) {
        if (!left) return right;
        if (!right) return left;
        if (left->amount < right->amount) {
            left->next = merge(left->next, right);
            return left;
        } else {
            right->next = merge(left, right->next);
            return right;
        }
    }

    void displaySortedExpenses() {
        head = mergeSort(head);
        cout << "Expenses sorted by amount:\n";
        displayExpenses();
    }

    // Binary search for an expense by amount
    bool binarySearchAmount(double target) {
        try{
            vector<double> amounts;
        Expense* current = head;
        while (current) {
            amounts.push_back(current->amount);
            current = current->next;
        }
        sort(amounts.begin(), amounts.end());
        return binarySearch(amounts, 0, amounts.size() - 1, target);
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return false;
    }
    }

    bool binarySearch(const vector<double>& amounts, int left, int right, double target) {
        if (left <= right) {
            int mid = left + (right - left) / 2;
            if (amounts[mid] == target) return true;
            if (amounts[mid] < target) return binarySearch(amounts, mid + 1, right, target);
            return binarySearch(amounts, left, mid - 1, target);
        }
        return false;
    }

    // Save all expenses to a file
    void saveExpensesToFile() {
    ofstream file(filename);
    if (file) {
        file << "Date,Category,Description,Amount\n"; // Header row
        Expense* current = head;
        while (current) {
            file << current->date << "," 
                 << current->category << "," 
                 << current->description << "," 
                 << current->amount << "\n";
            current = current->next;
        }
        file.close();
        cout << "Expenses saved to file successfully.\n";
    } else {
        cout << "Error saving expenses to file.\n";
    }
}

    // Clear the screen (cross-platform)
    void clearScreen() {
        #ifdef _WIN32
            system("CLS");
        #else
            system("clear");
        #endif
    }
};
bool adminLogin() {
    string username, password;
    const string adminUsername = "admin";
    const string adminPassword = "1234";
    char ch;  // To store password characters
    bool showPassword = false;  // Variable to control password visibility

    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter admin password: ";

    while (true) {
        ch = _getch();  // Get one character from the console (without echoing to the screen)
        
        if (ch == 13) {  // Enter key pressed
            break;
        } 
        else if (ch == 8) {  // Backspace key pressed
            if (password.length() > 0) {
                password.pop_back();
                cout << "\b \b";  // Erase last character from console (backspace)
            }
        }





















        
        else if (ch == 9) {  // Tab key pressed to toggle password visibility
            showPassword = !showPassword;
            cout << (showPassword ? " (password visible)" : " (password hidden)");
            continue; // Ignore the Tab key from being added to password
        }
        else {
            password.push_back(ch);
            if (showPassword) {
                cout << ch;  // Display character when password is visible
            } else {
                cout << "*";  // Display asterisk when password is hidden
            }
        }
    }

    cout << "\n";

    if (username == adminUsername && password == adminPassword) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Invalid credentials. Try again.\n";
        return false;
    }
}
void printHeader() {
    cout << "\n";
    cout << "**  EXPENSE TRACKER  **\n";
    cout << "** Login Page **\n";
    cout << "\t***********\n";
}
// Main function for user interaction
int main() {

    printHeader();

    while (!adminLogin()) {
        // Keep prompting for login until successful
    }

    ExpenseTracker tracker;
    int choice;
    do {
        tracker.clearScreen(); // Clear screen at the start of each loop
        cout << "\nExpense Tracker Menu\n";
        cout << "1. Add Expense\n";
        cout << "2. Display Expenses\n";
        cout << "3. Search by Category\n";
        cout << "4. Display Sorted Expenses by Amount\n";
        cout << "5. Search Expense by Amount\n";
        cout << "6. Delete an Expense\n";
        cout << "7. Total Spent\n";
        cout << "8. Exit\n";

        cout << "Enter your choice: ";
        cin >> choice;

        string date, category, description;
        double amount;
        switch (choice) {
            case 1:
                cout << "Enter date (YYYY-MM-DD): ";
                cin >> date;
                cout << "Enter category: ";
                cin >> category;
                cout << "Enter description: ";
                cin.ignore();
                getline(cin, description);
                cout << "Enter amount: ";
                cin >> amount;
                tracker.addExpenseToList(date, category, description, amount);
                break;
            case 2:
                tracker.displayExpenses();
                break;
            case 3:
                cout << "Enter category to search: ";
                cin >> category;
                tracker.searchByCategory(category);
                break;
            case 4:
                tracker.displaySortedExpenses();
                break;
            case 5:
                cout << "Enter amount to search: ";
                cin >> amount;

                // Validate the input
                if (cin.fail()) {  // Check if the input is not a valid number
                    cin.clear();    // Clear the error state
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Ignore invalid input
                    cout << "Invalid input. Please enter a numeric value for the amount.\n";
                break;          // Exit the case without searching
                }

                // Proceed to search if input is valid
                if (tracker.binarySearchAmount(amount)) {
                    cout << "Expense found with amount $" << amount << "\n";
                } else {
                    cout << "No expense found with amount $" << amount << "\n";
                }
                break;
            case 6:
                cout << "Enter date (YYYY-MM-DD): ";
                cin >> date;
                cout << "Enter category: ";
                cin >> category;
                cout << "Enter description: ";
                cin.ignore();
                getline(cin, description);
                cout << "Enter amount: ";
                cin >> amount;
                tracker.deleteExpense(date, category, description, amount);
                break;
            case 7:
                tracker.calculateTotalSpent();
                break;
            case 8:
                cout << "Exiting program.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        if (choice != 8) {
            cout << "Press Enter to continue...";
            cin.ignore();
            cin.get(); // Pause after each operation
        }
    } while (choice != 8);

    return 0;
}
