// ProjectTwo.cpp
 // America Sanchez Garcia
// CS 300 – ABCU Advising Assistance Program (single-file, C++11)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

// -----------------------------
// Data structures
// -----------------------------

struct Course {
    std::string number;                 // e.g., "CSCI300"
    std::string title;                  // e.g., "Introduction to Algorithms"
    std::vector<std::string> prereqs;   // e.g., {"CSCI200","MATH201"}
};

// Fast lookup by course number
typedef std::unordered_map<std::string, Course> Catalog;

// -----------------------------
// Small utilities (C++11-safe)
// -----------------------------

static std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
        s.erase(s.begin());
    }
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
    return s;
}

static std::string toUpper(std::string s) {
    for (size_t i = 0; i < s.size(); ++i) {
        s[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[i])));
    }
    return s;
}

// Simple CSV split (no quoted commas in the provided input)
static std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    std::stringstream ss(line);
    while (std::getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }
    return fields;
}

// -----------------------------
// Core features
// -----------------------------

// Load courses from CSV file into the catalog.
// CSV: courseNumber,name,prereq1,prereq2,... (0+ prereqs)
bool loadCatalogFromCSV(const std::string& filename, Catalog& catalog) {
    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'.\n";
        return false;
    }

    Catalog temp;
    std::string line;
    size_t lineNum = 0;

    while (std::getline(fin, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> fields = splitCSV(line);
        if (fields.size() < 2) {
            std::cerr << "Warning: Line " << lineNum << " has fewer than 2 fields; skipping.\n";
            continue;
        }

        Course c;
        c.number = toUpper(fields[0]);
        c.title  = fields[1];
        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields[i].empty()) c.prereqs.push_back(toUpper(fields[i]));
        }

        if (temp.find(c.number) != temp.end()) {
            std::cerr << "Warning: Duplicate definition for " << c.number
                      << "; using last occurrence.\n";
        }
        temp[c.number] = c;
    }

    if (temp.empty()) {
        std::cerr << "Error: No valid course rows were loaded from file.\n";
        return false;
    }

    catalog.swap(temp);
    return true;
}

// Print complete course list in alphanumeric order (lowest → highest)
void printCourseList(const Catalog& catalog) {
    std::vector<std::string> keys;
    keys.reserve(catalog.size());
    for (Catalog::const_iterator it = catalog.begin(); it != catalog.end(); ++it) {
        keys.push_back(it->first);
    }
    std::sort(keys.begin(), keys.end());

    std::cout << "Here is a sample schedule:\n";
    for (size_t i = 0; i < keys.size(); ++i) {
        const Course& c = catalog.at(keys[i]);
        std::cout << c.number << ", " << c.title << '\n';
    }
}

// Print a single course’s info – title + prerequisites (numbers AND titles when available)
void printCourseInfo(const Catalog& catalog, const std::string& userInput) {
    std::string key = toUpper(trim(userInput));
    Catalog::const_iterator it = catalog.find(key);
    if (it == catalog.end()) {
        std::cout << "Course not found.\n";
        return;
    }

    const Course& c = it->second;
    std::cout << c.number << ", " << c.title << '\n';

    if (c.prereqs.empty()) {
        std::cout << "Prerequisites: None\n";
        return;
    }

    std::cout << "Prerequisites: ";
    for (size_t i = 0; i < c.prereqs.size(); ++i) {
        const std::string& pnum = c.prereqs[i];
        Catalog::const_iterator pit = catalog.find(pnum);
        if (pit != catalog.end()) {
            std::cout << pit->second.number << " " << pit->second.title;
        } else {
            std::cout << pnum; // show number even if title not found
        }
        if (i + 1 < c.prereqs.size()) std::cout << ", ";
    }
    std::cout << '\n';
}

// -----------------------------
// Menu loop
// -----------------------------

void printMenu() {
    std::cout << "1. Load Data Structure.\n";
    std::cout << "2. Print Course List.\n";
    std::cout << "3. Print Course.\n";
    std::cout << "9. Exit\n";
    std::cout << "What would you like to do? ";
}

int main() {
    std::cout << "Welcome to the course planner.\n";

    Catalog catalog;
    bool loaded = false;

    while (true) {
        printMenu();

        std::string choiceLine;
        if (!std::getline(std::cin, choiceLine)) {
            std::cout << "\nThank you for using the course planner!\n";
            break;
        }

        int choice = 0;
        std::stringstream ss(choiceLine);
        ss >> choice;

        if (choice == 1) {
            std::cout << "Enter the file name to load (e.g., ABCU_Advising_Program_Input.csv): ";
            std::string filename;
            std::getline(std::cin, filename);
            filename = trim(filename);
            if (filename.empty()) {
                std::cout << "File name cannot be empty.\n";
                continue;
            }
            if (loadCatalogFromCSV(filename, catalog)) {
                loaded = true;
                std::cout << "Data loaded successfully.\n";
            } else {
                loaded = false;
                std::cout << "Failed to load data. Please check the file and try again.\n";
            }
        }
        else if (choice == 2) {
            if (!loaded) {
                std::cout << "Please load the data structure first (option 1).\n";
                continue;
            }
            printCourseList(catalog);
        }
        else if (choice == 3) {
            if (!loaded) {
                std::cout << "Please load the data structure first (option 1).\n";
                continue;
            }
            std::cout << "What course do you want to know about? ";
            std::string courseNum;
            std::getline(std::cin, courseNum);
            printCourseInfo(catalog, courseNum);
        }
        else if (choice == 9) {
            std::cout << "Thank you for using the course planner!\n";
            break;
        }
        else {
            std::cout << choice << " is not a valid option.\n";
        }
    }

    return 0;
}
