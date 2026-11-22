#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <iomanip>

using namespace std;

// Template utility to clamp or validate numeric values generically
template <typename T>
T validateRange(const T& value, const T& low, const T& high, const string& fieldName) {
    if (value < low || value > high) {
        throw out_of_range("Invalid " + fieldName + ": " + to_string(value) +
                           " (must be between " + to_string(low) + " and " + to_string(high) + ")");
    }
    return value;
}

// Base class
class Student {
protected:
    int rollNo;
    string name;

public:
    Student(int roll = 0, const string& studentName = "")
        : rollNo(roll), name(studentName) {}

    virtual ~Student() = default;

    int getRollNo() const { return rollNo; }
    string getName() const { return name; }

    // Virtual function for runtime polymorphism
    virtual void display() const {
        cout << "Roll No: " << rollNo << " | Name: " << name;
    }

    // Virtual function to get total marks (0 for base Student)
    virtual double getTotalMarks() const {
        return 0.0;
    }

    // Virtual function to get pass/fail status
    virtual string getStatus() const {
        return "N/A";
    }

    // Serialize base fields to CSV format
    virtual string serialize() const {
        return to_string(rollNo) + "," + name;
    }
};

// Derived class
class Result : public Student {
private:
    vector<double> marks;
    double maxMarkPerSubject;
    double passPercentage;

    void validateMarksList() const {
        if (marks.empty()) {
            throw invalid_argument("Marks list cannot be empty.");
        }
        for (double m : marks) {
            validateRange<double>(m, 0.0, maxMarkPerSubject, "mark");
        }
    }

public:
    Result(int roll, const string& studentName, const vector<double>& subjectMarks,
           double maxMark = 100.0, double passPct = 40.0)
        : Student(roll, studentName),
          marks(subjectMarks),
          maxMarkPerSubject(maxMark),
          passPercentage(passPct) {
        validateMarksList();
    }

    void setMarks(const vector<double>& subjectMarks) {
        marks = subjectMarks;
        validateMarksList();
    }

    const vector<double>& getMarks() const { return marks; }

    double getTotalMarks() const override {
        double total = 0.0;
        for (double m : marks) {
            total += m;
        }
        return total;
    }

    double getMaxTotalMarks() const {
        return marks.size() * maxMarkPerSubject;
    }

    double getPercentage() const {
        double maxTotal = getMaxTotalMarks();
        if (maxTotal == 0.0) return 0.0;
        return (getTotalMarks() / maxTotal) * 100.0;
    }

    string getStatus() const override {
        // Also fail if any individual subject is below passPercentage of maxMarkPerSubject
        double minSubjectPass = (passPercentage / 100.0) * maxMarkPerSubject;
        for (double m : marks) {
            if (m < minSubjectPass) {
                return "FAIL";
            }
        }
        return (getPercentage() >= passPercentage) ? "PASS" : "FAIL";
    }

    // Overridden virtual display function
    void display() const override {
        Student::display();
        cout << " | Marks: [";
        for (size_t i = 0; i < marks.size(); ++i) {
            cout << fixed << setprecision(1) << marks[i];
            if (i + 1 < marks.size()) cout << ", ";
        }
        cout << "] | Total: " << fixed << setprecision(2) << getTotalMarks()
             << "/" << getMaxTotalMarks()
             << " (" << setprecision(1) << getPercentage() << "%)"
             << " | Result: " << getStatus() << "\n";
    }

    // Format: rollNo,name,maxMarkPerSubject,passPercentage,m1,m2,m3...
    string serialize() const override {
        ostringstream oss;
        oss << rollNo << "," << name << ","
            << maxMarkPerSubject << "," << passPercentage;
        for (double m : marks) {
            oss << "," << m;
        }
        return oss.str();
    }

    // Reconstruct a Result object from a serialized CSV line
    static Result deserialize(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> parts;

        while (getline(ss, token, ',')) {
            parts.push_back(token);
        }

        if (parts.size() < 5) {
            throw runtime_error("Corrupted record line: " + line);
        }

        int roll = stoi(parts[0]);
        string studentName = parts[1];
        double maxMark = stod(parts[2]);
        double passPct = stod(parts[3]);

        vector<double> loadedMarks;
        for (size_t i = 4; i < parts.size(); ++i) {
            loadedMarks.push_back(stod(parts[i]));
        }

        return Result(roll, studentName, loadedMarks, maxMark, passPct);
    }
};

// Save all records to file
void saveRecordsToFile(const string& filename, const vector<shared_ptr<Student>>& records) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw ios_base::failure("Failed to open file for writing: " + filename);
    }

    for (const auto& ptr : records) {
        outFile << ptr->serialize() << "\n";
    }

    outFile.close();
}

// Load records from file into a vector of base pointers (polymorphic storage)
vector<shared_ptr<Student>> loadRecordsFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw ios_base::failure("Failed to open file for reading: " + filename);
    }

    vector<shared_ptr<Student>> loaded;
    string line;
    int lineNum = 0;

    while (getline(inFile, line)) {
        ++lineNum;
        if (line.empty()) continue;
        try {
            Result r = Result::deserialize(line);
            loaded.push_back(make_shared<Result>(r));
        } catch (const exception& ex) {
            cerr << "Warning on line " << lineNum << ": " << ex.what() << "\n";
        }
    }

    inFile.close();
    return loaded;
}

int main() {
    const string filename = "results.txt";
    vector<shared_ptr<Student>> records;

    // Sample data + exception handling demonstration
    struct RawEntry {
        int roll;
        string name;
        vector<double> marks;
    };

    vector<RawEntry> entries = {
        {101, "Aarav Sharma", {88.0, 92.5, 79.0}},
        {102, "Diya Nair",    {45.0, 52.0, 48.5}},
        {103, "Rohan Verma",  {95.0, 91.0, 94.0}},
        {104, "Kabir Das",    {30.0, 65.0, 55.0}},
        {105, "Invalid Test", {85.0, 105.0, 70.0}} // Triggers out_of_range exception
    };

    cout << "--- Adding Student Records ---\n";
    for (const auto& entry : entries) {
        try {
            auto res = make_shared<Result>(entry.roll, entry.name, entry.marks);
            records.push_back(res);
            cout << "Added record for Roll No " << entry.roll << " (" << entry.name << ").\n";
        } catch (const out_of_range& e) {
            cerr << "Exception caught [Roll " << entry.roll << "]: " << e.what() << "\n";
        } catch (const exception& e) {
            cerr << "Error [Roll " << entry.roll << "]: " << e.what() << "\n";
        }
    }

    // File handling: write and read back
    try {
        saveRecordsToFile(filename, records);
        cout << "\nSaved " << records.size() << " valid records to '" << filename << "'.\n";

        vector<shared_ptr<Student>> retrievedRecords = loadRecordsFromFile(filename);

        cout << "\n--- All Student Details, Total Marks & Results (Polymorphic Display) ---\n";
        for (const auto& studentPtr : retrievedRecords) {
            studentPtr->display();
        }

        // STL algorithm + lambda expression to identify students scoring above a threshold
        double threshold = 220.0;
        cout << "\n--- Students with Total Marks > " << threshold << " ---\n";

        auto aboveThresholdPredicate = [threshold](const shared_ptr<Student>& s) {
            return s->getTotalMarks() > threshold;
        };

        int countAbove = count_if(retrievedRecords.begin(), retrievedRecords.end(), aboveThresholdPredicate);
        cout << "Total count above " << threshold << ": " << countAbove << "\n";

        vector<shared_ptr<Student>> topScorers;
        copy_if(retrievedRecords.begin(), retrievedRecords.end(),
                back_inserter(topScorers), aboveThresholdPredicate);

        for (const auto& scorer : topScorers) {
            scorer->display();
        }

    } catch (const exception& e) {
        cerr << "File/System error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}