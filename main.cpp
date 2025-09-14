#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

const string INPUT_FILE = "car_records.txt";
const int MAX_RECORDS = 100;
const int ID_LENGTH = 10;
const double MIN_PRICE = 24995.00;

struct Car {
    string id;
    string model;
    int quantity;
    double price;
};

enum MenuOption {
    PRINT_INVENTORY = 1,
    PRINT_INVALID_RECORDS,
    QUIT
};

int readDataFromFile(Car inventory[]);
bool isValidRecord(const string& id, const string& model, int quantity, double price, string& errorMsg);
bool isValidCarId(const string& id);
bool isValidModel(const string& model);
void printInventory(const Car inventory[], int count);
void printInvalidRecords();
void displayMenu();

int main() {
    Car inventory[MAX_RECORDS];
    int count = readDataFromFile(inventory);
    
    int choice;
    do {
        displayMenu();
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter 1, 2, or 3 " << endl;
            continue;
        }
        
        switch (choice) {
            case PRINT_INVENTORY:
                printInventory(inventory, count);
                break;
            case PRINT_INVALID_RECORDS:
                printInvalidRecords();
                break;
            case QUIT:
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice, Try again." << endl;
        }
    } while (choice != QUIT);
    
    return 0;
}

int readDataFromFile(Car inventory[]) {
    ifstream infile(INPUT_FILE);
    ofstream errorfile("invalid_records.txt");
    
    if (!infile) {
        cout << "Error: Could not open file '" << INPUT_FILE << "'" << endl;
        return 0;
    }
    
    if (!errorfile) {
        cout << "Error: Could not create error file" << endl;
        infile.close();
        return 0;
    }
    
    infile.seekg(0, ios::end);
    if (infile.tellg() == 0) {
        cout << "Input file is empty" << endl;
        infile.close();
        errorfile.close();
        return 0;
    }
    infile.seekg(0, ios::beg);
    
    int recordCount = 0;
    string line;
    
    while (getline(infile, line) && recordCount < MAX_RECORDS) {
        stringstream ss(line);
        string id, model;
        int qty;
        double prc;
        
        if (ss >> id >> model >> qty >> prc) {
            string errors;
            if (isValidRecord(id, model, qty, prc, errors)) {
                inventory[recordCount].id = id;
                inventory[recordCount].model = model;
                inventory[recordCount].quantity = qty;
                inventory[recordCount].price = prc;
                recordCount++;
            } else {
                errorfile << id << " " << model << " " << qty << " " << prc << " - " << errors << endl;
            }
        } else {
            errorfile << line << " - Bad format" << endl;
        }
    }
    
    if (recordCount == MAX_RECORDS && !infile.eof()) {
        cout << "Warning: Storage full some records skipped" << endl;
    }
    
    infile.close();
    errorfile.close();
    
    cout << "Processed " << recordCount << " valid records" << endl;
    return recordCount;
}

bool isValidRecord(const string& id, const string& model, int quantity, double price, string& errorMsg) {
    errorMsg = "";
    bool valid = true;
    
    if (!isValidCarId(id)) {
        errorMsg += "Invalid ID; ";
        valid = false;
    }
    
    if (!isValidModel(model)) {
        errorMsg += "Invalid model; ";
        valid = false;
    }
    
    if (quantity <= 0) {
        errorMsg += "Quantity must be > 0; ";
        valid = false;
    }
    
    if (price <= MIN_PRICE) {
        stringstream ss;
        ss << fixed << setprecision(2) << MIN_PRICE;
        errorMsg += "Price must be > $" + ss.str() + "; ";
        valid = false;
    }
    
    return valid;
}

bool isValidCarId(const string& id) {
    if (id.length() != ID_LENGTH) return false;
    
    for (int i = 0; i < 2; i++) {
        if (!isalpha(id[i]) || toupper(id[i]) == 'O') return false;
    }
    
    for (int i = 2; i < 8; i++) {
        if (!isalnum(id[i]) || toupper(id[i]) == 'O') return false;
    }
    
    for (int i = 8; i < ID_LENGTH; i++) {
        if (!isdigit(id[i])) return false;
    }
    
    return true;
}

bool isValidModel(const string& model) {
    if (model.length() < 3) return false;
    if (!isalpha(model[0])) return false;
    
    for (char c : model) {
        if (!isalnum(c)) return false;
    }
    
    return true;
}

void printInventory(const Car inventory[], int count) {
    if (count == 0) {
        cout << "No valid records to display" << endl;
        return;
    }
    
    cout << "\nCar Inventory:" << endl;
    cout << "----------------------------------------" << endl;
    cout << setw(12) << left << "ID" 
         << setw(15) << left << "Model" 
         << setw(8) << right << "Qty" 
         << setw(12) << right << "Price" << endl;
    cout << "----------------------------------------" << endl;
    
    for (int i = 0; i < count; i++) {
        cout << setw(12) << left << inventory[i].id
             << setw(15) << left << inventory[i].model
             << setw(8) << right << inventory[i].quantity
             << setw(12) << right << fixed << setprecision(2) << inventory[i].price << endl;
    }
    cout << "----------------------------------------" << endl;
}

void printInvalidRecords() {
    ifstream errorfile("invalid_records.txt");
    
    if (!errorfile) {
        cout << "No invalid records found or error file missing" << endl;
        return;
    }
    
    errorfile.seekg(0, ios::end);
    if (errorfile.tellg() == 0) {
        cout << "No invalid records found" << endl;
        errorfile.close();
        return;
    }
    errorfile.seekg(0, ios::beg);
    
    cout << "\nInvalid Records: " << endl;
    cout << "----------------------------------------" << endl;
    
    string line;
    while (getline(errorfile, line)) {
        cout << line << endl;
    }
    
    errorfile.close();
    cout << "----------------------------------------" << endl;
}

void displayMenu() {
    cout << "\nMenu Options: " << endl;
    cout << "1. Show inventory" << endl;
    cout << "2. Show invalid records" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter your choice (1-3): ";
}

/*
Menu Options:
1. Show inventory
2. Show invalid records
3. Exit
Enter your choice (1-3): 1

Car Inventory:
----------------------------------------
ID          Model               Qty       Price
----------------------------------------
AB12345678  ModelX                5    30000.00
MN12345678  GoodOne              10    35000.00
WX12345678  Epsilon345           12    30000.00
YZ12345678  Zeta678               3    27500.00
----------------------------------------

Menu Options:
1. Show inventory
2. Show invalid records
3. Exit
Enter your choice (1-3): 2

Invalid Records:
----------------------------------------
CD98765432 ModelY 0 25000 - Quantity must be > 0;
EF1234O678 ModelZ 3 26000 - Invalid ID;
GH12345678 Bad_Model 2 27000 - Invalid model;
IJ123456789 TooLong 1 28000 - Invalid ID;
KL1234567 Short 4 29000 - Invalid ID;
OP12345678 Alpha123 15 45000.5 - Invalid ID;
QR12345678 Beta456 0 20000 - Quantity must be > 0; Price must be > $24995.00;
ST12345678 Gamma789 -5 50000 - Quantity must be > 0;
UV12345678 Delta012 8 24000 - Price must be > $24995.00;
----------------------------------------

Menu Options:
1. Show inventory
2. Show invalid records
3. Exit
Enter your choice (1-3): 3
Goodbye!
*/
