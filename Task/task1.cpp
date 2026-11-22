#include <iostream>
#include <cstring>
#include <iomanip>
using namespace std;

class Student {
private:
    int rollNo;
    char* name;
    float marks;
    static int totalStudents;

    void copyName(const char* src) {
        if (src != nullptr) {
            name = new char[strlen(src) + 1];
            strcpy(name, src);
        } else {
            name = new char[1];
            name[0] = '\0';
        }
    }

public:
    // Parameterized constructor
    Student(int r, const char* n, float m) : rollNo(r), marks(m) {
        copyName(n);
        ++totalStudents;
    }

    // Deep copy constructor
    Student(const Student& other) : rollNo(other.rollNo), marks(other.marks) {
        copyName(other.name);
        ++totalStudents;
    }

    // Destructor
    ~Student() {
        delete[] name;
        --totalStudents;
    }

    // Getters
    int getRollNo() const { return rollNo; }
    const char* getName() const { return name; }
    float getMarks() const { return marks; }

    // Overloaded setMarks functions
    void setMarks(float m) {
        marks = m;
    }

    void setMarks(float sub1, float sub2, float sub3) {
        marks = (sub1 + sub2 + sub3) / 3.0f;
    }

    // Helper for grade calculation
    char computeGrade() const {
        if (marks >= 90.0f) return 'A';
        if (marks >= 75.0f) return 'B';
        if (marks >= 60.0f) return 'C';
        if (marks >= 40.0f) return 'D';
        return 'F';
    }

    // Overloaded display functions
    void display() const {
        cout << "Roll No: " << rollNo
             << " | Name: " << name
             << " | Marks: " << fixed << setprecision(2) << marks << endl;
    }

    void display(bool showGrade) const {
        display();
        if (showGrade) {
            cout << "   -> Grade Awarded: " << computeGrade() << endl;
        }
    }

    // Static member function
    static int getStudentCount() {
        return totalStudents;
    }

    // Overloaded > operator
    bool operator>(const Student& rhs) const {
        return this->marks > rhs.marks;
    }

    // Friend function declaration
    friend void inspectRecord(const Student& st);
};

int Student::totalStudents = 0;

void inspectRecord(const Student& st) {
    cout << "[Friend Access] Student: " << st.name
         << " (Roll #" << st.rollNo << ") -> Marks: "
         << fixed << setprecision(2) << st.marks << endl;
}

int main() {
    Student st1(201, "Rohan", 84.0f);
    Student st2(202, "Sneha", 71.5f);
    Student st3(203, "Vikram", 0.0f);

    st2.setMarks(81.0f);
    st3.setMarks(91.0f, 89.0f, 93.0f);

    cout << "=== Student Records ===" << endl;
    st1.display();
    st2.display(true);
    st3.display(true);

    cout << "\n=== Friend Function Output ===" << endl;
    inspectRecord(st2);

    cout << "\n=== Marks Comparison (> Operator) ===" << endl;
    if (st1 > st2)
        cout << st1.getName() << " scored higher than " << st2.getName() << endl;
    else
        cout << st2.getName() << " scored higher than or equal to " << st1.getName() << endl;

    if (st3 > st1)
        cout << st3.getName() << " scored higher than " << st1.getName() << endl;
    else
        cout << st1.getName() << " scored higher than or equal to " << st3.getName() << endl;

    cout << "\nActive student objects: " << Student::getStudentCount() << endl;

    Student* tempStudent = new Student(204, "Ananya", 87.5f);
    cout << "After dynamic allocation, active students: "
         << Student::getStudentCount() << endl;

    delete tempStudent;
    cout << "After dynamic deallocation, active students: "
         << Student::getStudentCount() << endl;

    return 0;
}