#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits>
using namespace std;

const int MAX_RECORDS = 100;
const string DATA_FILE = "lost_found_records.txt";
const string REPORT_FILE = "lost_found_report.txt";

struct Item {
    int id;
    string type;
    string itemName;
    string category;
    string description;
    string location;
    string contact;
};

Item records[MAX_RECORDS];
int recordCount = 0;
int nextId = 1;

string toLowerCase(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return value;
}

string readLine(const string& message) {
    string value;
    cout << message;
    getline(cin, value);
    return value;
}

int readChoice(const string& message) {
    int value;
    while (true) {
        cout << message;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void saveRecords() {
    ofstream file(DATA_FILE);
    if (!file) {
        cout << "Error: Could not save records.\n";
        return;
    }

    for (int i = 0; i < recordCount; i++) {
        file << records[i].id << '|'
             << records[i].type << '|'
             << records[i].itemName << '|'
             << records[i].category << '|'
             << records[i].description << '|'
             << records[i].location << '|'
             << records[i].contact << '\n';
    }
}

void loadRecords() {
    ifstream file(DATA_FILE);
    if (!file) return;

    string line;
    while (getline(file, line) && recordCount < MAX_RECORDS) {
        Item item;
        size_t start = 0, end;
        string fields[7];
        int fieldCount = 0;

        while ((end = line.find('|', start)) != string::npos && fieldCount < 6) {
            fields[fieldCount++] = line.substr(start, end - start);
            start = end + 1;
        }

        if (fieldCount == 6) {
            fields[6] = line.substr(start);
            try {
                item.id = stoi(fields[0]);
            } catch (...) {
                continue;
            }

            item.type = fields[1];
            item.itemName = fields[2];
            item.category = fields[3];
            item.description = fields[4];
            item.location = fields[5];
            item.contact = fields[6];

            records[recordCount++] = item;
            if (item.id >= nextId) nextId = item.id + 1;
        }
    }
}

bool studentLogin(string& studentName) {
    cout << "========== STUDENT LOGIN ==========\n";
    studentName = readLine("Enter student name: ");
    string rollNumber = readLine("Enter roll number: ");

    if (studentName.empty() || rollNumber.empty()) {
        cout << "Name and roll number cannot be empty.\n";
        return false;
    }

    cout << "\nWelcome, " << studentName << "!\n";
    return true;
}

void addItem(const string& type) {
    if (recordCount >= MAX_RECORDS) {
        cout << "Record storage is full.\n";
        return;
    }

    Item item;
    item.id = nextId++;
    item.type = type;

    cout << "\n--- Report " << type << " Item ---\n";
    item.itemName = readLine("Item name: ");
    item.category = readLine("Category (ID card, keys, bag, etc.): ");
    item.description = readLine("Description: ");
    item.location = readLine("Lost/found location: ");
    item.contact = readLine("Contact details: ");

    if (item.itemName.empty()) {
        cout << "Item name is required. Record was not added.\n";
        nextId--;
        return;
    }

    records[recordCount++] = item;
    saveRecords();
    cout << type << " item recorded successfully. Record ID: " << item.id << "\n";
}

void reportLostItem() {
    addItem("Lost");
}

void reportFoundItem() {
    addItem("Found");
}

void displayRecord(const Item& item) {
    cout << "\nRecord ID: " << item.id
         << "\nType: " << item.type
         << "\nItem: " << item.itemName
         << "\nCategory: " << item.category
         << "\nDescription: " << item.description
         << "\nLocation: " << item.location
         << "\nContact: " << item.contact << "\n";
}

void searchItem() {
    if (recordCount == 0) {
        cout << "No records available.\n";
        return;
    }

    string keyword = toLowerCase(readLine("Enter item name, category, or location: "));
    bool found = false;

    for (int i = 0; i < recordCount; i++) {
        if (toLowerCase(records[i].itemName).find(keyword) != string::npos ||
            toLowerCase(records[i].category).find(keyword) != string::npos ||
            toLowerCase(records[i].location).find(keyword) != string::npos) {
            displayRecord(records[i]);
            found = true;
        }
    }

    if (!found) cout << "No matching item found.\n";
}

void autoMatchItems() {
    bool matchFound = false;

    cout << "\n========== AUTOMATIC MATCHING ==========\n";
    for (int i = 0; i < recordCount; i++) {
        if (records[i].type != "Lost") continue;

        for (int j = 0; j < recordCount; j++) {
            if (records[j].type == "Found" &&
                toLowerCase(records[i].itemName) == toLowerCase(records[j].itemName)) {
                cout << "\nPossible match found!\n";
                cout << "Lost record:";
                displayRecord(records[i]);
                cout << "Found record:";
                displayRecord(records[j]);
                matchFound = true;
            }
        }
    }

    if (!matchFound) {
        cout << "No matching Lost and Found records are available.\n";
    } else {
        cout << "\nPlease verify the item with the person who reported it.\n";
    }
}

void viewAllRecords() {
    if (recordCount == 0) {
        cout << "No records available.\n";
        return;
    }

    Item sorted[MAX_RECORDS];
    for (int i = 0; i < recordCount; i++) sorted[i] = records[i];

    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            if (toLowerCase(sorted[j].itemName) >
                toLowerCase(sorted[j + 1].itemName)) {
                Item temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    cout << "\n========== ALL RECORDS (A-Z) ==========\n";
    for (int i = 0; i < recordCount; i++) displayRecord(sorted[i]);
}

void deleteRecord() {
    if (recordCount == 0) {
        cout << "No records available to delete.\n";
        return;
    }

    int id = readChoice("Enter record ID to delete: ");
    int index = -1;

    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        cout << "Record ID not found.\n";
        return;
    }

    for (int i = index; i < recordCount - 1; i++) {
        records[i] = records[i + 1];
    }

    recordCount--;
    saveRecords();
    cout << "Record deleted successfully.\n";
}

void itemStatistics() {
    int lostCount = 0, foundCount = 0;

    for (int i = 0; i < recordCount; i++) {
        if (records[i].type == "Lost") lostCount++;
        else if (records[i].type == "Found") foundCount++;
    }

    cout << "\n========== ITEM STATISTICS ==========\n";
    cout << "Total records: " << recordCount
         << "\nLost items: " << lostCount
         << "\nFound items: " << foundCount << "\n";
}

void exportReport() {
    ofstream file(REPORT_FILE);
    if (!file) {
        cout << "Error: Could not create report file.\n";
        return;
    }

    file << "SMART CAMPUS LOST & FOUND MANAGEMENT SYSTEM\n";
    file << "===========================================\n";
    file << "Total records: " << recordCount << "\n\n";

    for (int i = 0; i < recordCount; i++) {
        file << "Record ID: " << records[i].id
             << "\nType: " << records[i].type
             << "\nItem: " << records[i].itemName
             << "\nCategory: " << records[i].category
             << "\nDescription: " << records[i].description
             << "\nLocation: " << records[i].location
             << "\nContact: " << records[i].contact
             << "\n-------------------------------------------\n";
    }

    file.close();
    cout << "Report exported successfully to " << REPORT_FILE << ".\n";
}

void showMenu() {
    cout << "\n========== SMART CAMPUS LOST & FOUND ==========\n";
    cout << "1. Report Lost Item\n";
    cout << "2. Report Found Item\n";
    cout << "3. Search Item\n";
    cout << "4. Auto Match Lost & Found\n";
    cout << "5. View All Records\n";
    cout << "6. Delete Record\n";
    cout << "7. Item Statistics\n";
    cout << "8. Export Report\n";
    cout << "9. Exit\n";
}

int main() {
    string studentName;
    loadRecords();

    if (!studentLogin(studentName)) {
        cout << "Login failed. Please restart and enter the details again.\n";
        return 0;
    }

    int choice;
    do {
        showMenu();
        choice = readChoice("Enter your choice (1-9): ");

        switch (choice) {
            case 1: reportLostItem(); break;
            case 2: reportFoundItem(); break;
            case 3: searchItem(); break;
            case 4: autoMatchItems(); break;
            case 5: viewAllRecords(); break;
            case 6: deleteRecord(); break;
            case 7: itemStatistics(); break;
            case 8: exportReport(); break;
            case 9:
                saveRecords();
                cout << "Thank you for using the Lost & Found System. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please select from 1 to 9.\n";
        }
    } while (choice != 9);

    return 0;
}
