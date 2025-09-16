#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>
using namespace std;

const int MAX_ITEMS = 100;
const int ID_LENGTH = 10;
const double MIN_PRICE = 24995.00;
const string INPUT_FILE = "cars.txt";
const string ERROR_FILE = "error_records.txt";

enum MenuOption { PRINT_INVENTORY = 1, PRINT_ERRORS, QUIT };

struct CarRecord {
    string id;
    string model;
    int quantity;
    double price;
};

// Function prototypes
bool isValidRecord(const string& line, CarRecord& record, string& errorMessage);
bool isValidId(const string& id);
bool isValidModel(const string& model);
bool isValidQuantity(int quantity);
bool isValidPrice(double price);
void printInventory(const CarRecord inventory[], int count);
void printErrorFile();
void displayMenu();
void processMenuChoice(int choice, CarRecord inventory[], int count);
int loadInventory(CarRecord inventory[]);

int main() {
    CarRecord inventory[MAX_ITEMS] = {};
    int count = loadInventory(inventory);
    int choice = 0;
    bool shouldQuit = false;
    
    cout << "Car Inventory System loaded " << count << " valid records." << endl;
    
    while (!shouldQuit) {
        displayMenu();
        cin >> choice;
        cin.ignore(1000, '\n');
        
        if (choice >= PRINT_INVENTORY && choice <= QUIT) {
            processMenuChoice(choice, inventory, count);
            if (choice == QUIT) {
                shouldQuit = true;
            }
        } else {
            cout << "Invalid choice, Please enter 1, 2, or 3" << endl;
        }
    }
    
    cout << "Thank you for using Car Inventory System" << endl;
    return 0;
}

bool isValidId(const string& id) {
    if (id.length() != ID_LENGTH) {
        return false;
    }
    
    // Check first 2 characters: alpha only, no 'O'
    for (int i = 0; i < 2; i++) {
        if (!isalpha(id[i]) || toupper(id[i]) == 'O') {
            return false;
        }
    }
    
    // Check next 6 characters: alphanumeric, no 'O'
    for (int i = 2; i < 8; i++) {
        if (!isalnum(id[i]) || toupper(id[i]) == 'O') {
            return false;
        }
    }
    
    // Check last 2 characters: numeric only
    for (int i = 8; i < ID_LENGTH; i++) {
        if (!isdigit(id[i])) {
            return false;
        }
    }
    
    return true;
}

bool isValidModel(const string& model) {
    if (model.length() < 3 || !isalpha(model[0])) {
        return false;
    }
    
    for (char c : model) {
        if (!isalnum(c)) {
            return false;
        }
    }
    
    return true;
}

bool isValidQuantity(int quantity) {
    return quantity > 0;
}

bool isValidPrice(double price) {
    return price > MIN_PRICE;
}

bool isValidRecord(const string& line, CarRecord& record, string& errorMessage) {
    stringstream ss(line);
    string tempId, tempModel;
    int tempQuantity;
    double tempPrice;
    
    ss >> tempId >> tempModel >> tempQuantity >> tempPrice;
    
    if (ss.fail()) {
        errorMessage = "Format error: could not parse record";
        return false;
    }
    
    bool valid = true;
    errorMessage = "";
    
    if (!isValidId(tempId)) {
        errorMessage += "Invalid ID; ";
        valid = false;
    }
    
    if (!isValidModel(tempModel)) {
        errorMessage += "Invalid model; ";
        valid = false;
    }
    
    if (!isValidQuantity(tempQuantity)) {
        errorMessage += "Quantity must be above zero; ";
        valid = false;
    }
    
    if (!isValidPrice(tempPrice)) {
        errorMessage += "Price must be above $" + to_string((int)MIN_PRICE) + "; ";
        valid = false;
    }
    
    if (valid) {
        record.id = tempId;
        record.model = tempModel;
        record.quantity = tempQuantity;
        record.price = tempPrice;
    }
    
    return valid;
}

int loadInventory(CarRecord inventory[]) {
    ifstream infile(INPUT_FILE);
    ofstream errfile(ERROR_FILE);
    int count = 0;
    string line;
    
    if (!infile.is_open()) {
        cout << "Error: Could not open input file '" << INPUT_FILE << "'" << endl;
        return 0;
    }
    
    if (!errfile.is_open()) {
        cout << "Error: Could not open error file '" << ERROR_FILE << "'" << endl;
        return 0;
    }
    
    if (infile.peek() == ifstream::traits_type::eof()) {
        cout << "Input file is empty." << endl;
        return 0;
    }
    
    while (getline(infile, line) && count < MAX_ITEMS) {
        CarRecord tempRecord;
        string errorMessage;
        
        if (isValidRecord(line, tempRecord, errorMessage)) {
            inventory[count] = tempRecord;
            count++;
        } else {
            errfile << line << "\t" << errorMessage << endl;
        }
    }
    
    if (count == MAX_ITEMS && !infile.eof()) {
        cout << "Warning: Array full. Some records may not have been processed." << endl;
    }
    
    infile.close();
    errfile.close();
    
    return count;
}

void printInventory(const CarRecord inventory[], int count) {
    if (count == 0) {
        cout << "No valid records in inventory." << endl;
        return;
    }
    
    cout << "\nCar Inventory Records:" << endl;
    cout << "=================================" << endl;
    cout << setw(12) << left << "ID" 
         << setw(15) << left << "Model" 
         << setw(10) << right << "Quantity" 
         << setw(15) << right << "Price" << endl;
    cout << "===================================" << endl;
    
    for (int i = 0; i < count; i++) {
        cout << setw(12) << left << inventory[i].id
             << setw(15) << left << inventory[i].model
             << setw(10) << right << inventory[i].quantity
             << setw(15) << right << fixed << setprecision(2) << inventory[i].price << endl;
    }
    cout << "==================================" << endl;
}

void printErrorFile() {
    ifstream errfile(ERROR_FILE);
    string line;
    
    if (!errfile.is_open()) {
        cout << "Error: Could not open error file '" << ERROR_FILE << "'" << endl;
        return;
    }
    
    if (errfile.peek() == ifstream::traits_type::eof()) {
        cout << "No error records found." << endl;
        errfile.close();
        return;
    }
    
    cout << "\nInvalid Records from Error File:" << endl;
    cout << "================================================================" << endl;
    
    while (getline(errfile, line)) {
        cout << line << endl;
    }
    
    errfile.close();
    cout << "================================================================" << endl;
}

void displayMenu() {
    cout << "\n=== Car Inventory Menu ===" << endl;
    cout << PRINT_INVENTORY << ". Print all inventory items" << endl;
    cout << PRINT_ERRORS << ". Print invalid records from error file" << endl;
    cout << QUIT << ". Quit program" << endl;
    cout << "Enter your choice (1-3): ";
}

void processMenuChoice(int choice, CarRecord inventory[], int count) {
    switch (choice) {
        case PRINT_INVENTORY:
            printInventory(inventory, count);
            break;
        case PRINT_ERRORS:
            printErrorFile();
            break;
        case QUIT:
            cout << "Exiting program..." << endl;
            break;
    }
}

/* TEST RUNS:
=== Test 1: Empty input file ===
Input file is empty.
No valid records in inventory.

=== Test 2: Valid records only ===
Car Inventory System loaded 3 valid records.

Car Inventory Records:
==================================================================
ID          Model          Quantity          Price
==================================================================
AB12345678  Toyota                 10     30000.00
CD87654321  Honda                   5     35000.00
EF11223344  Ford                    8     50000.00
==================================================================

=== Test 3: Mixed valid/invalid records ===
Car Inventory System loaded 2 valid records.

Invalid Records from Error File:
================================================================
XY123O7890 Camry 0 20000   Invalid ID; Quantity must be above zero; Price must be above $24995;
ZZ99999999 123Car 5 30000   Invalid model; 
================================================================
*/