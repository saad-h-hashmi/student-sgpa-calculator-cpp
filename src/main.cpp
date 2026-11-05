#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Subject {
    std::string name;
    std::string code;
    int creditHours{};
    int obtainedMarks{};
    int totalMarks{};
};

struct Student {
    std::string name;
    std::string studentId;
    std::string semester;
    std::vector<Subject> subjects;
};

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool startsWith(const std::string& value, const std::string& prefix) {
    return value.rfind(prefix, 0) == 0;
}

double gradePoint(double percentage) {
    if (percentage >= 85.0) return 4.00;
    if (percentage >= 80.0) return 3.75;
    if (percentage >= 75.0) return 3.50;
    if (percentage >= 70.0) return 3.00;
    if (percentage >= 65.0) return 2.50;
    if (percentage >= 60.0) return 2.00;
    if (percentage >= 55.0) return 1.50;
    if (percentage >= 50.0) return 1.00;
    return 0.00;
}

std::string letterGrade(double percentage) {
    if (percentage >= 85.0) return "A";
    if (percentage >= 80.0) return "A-";
    if (percentage >= 75.0) return "B+";
    if (percentage >= 70.0) return "B-";
    if (percentage >= 65.0) return "C+";
    if (percentage >= 60.0) return "C";
    if (percentage >= 55.0) return "D+";
    if (percentage >= 50.0) return "D";
    return "F";
}

Student parseStudentHeader(const std::string& line) {
    const std::string value = trim(line.substr(std::string("Student:").size()));
    const auto open = value.find('(');
    const auto close = value.find(')', open == std::string::npos ? 0 : open);
    const auto semesterLabel = value.find("Semester:");
    if (open == std::string::npos || close == std::string::npos ||
        semesterLabel == std::string::npos || close > semesterLabel) {
        throw std::runtime_error("Invalid student header: " + line);
    }

    Student student;
    student.name = trim(value.substr(0, open));
    student.studentId = trim(value.substr(open + 1, close - open - 1));
    student.semester = trim(value.substr(semesterLabel + std::string("Semester:").size()));
    if (student.name.empty() || student.studentId.empty() || student.semester.empty()) {
        throw std::runtime_error("Student name, ID, and semester are required: " + line);
    }
    return student;
}

Subject parseSubjectHeader(const std::string& line) {
    const std::string value = trim(line.substr(std::string("Subject:").size()));
    const auto open = value.find(" (");
    const auto close = value.find(')', open == std::string::npos ? 0 : open);
    const auto creditsLabel = value.find("Credit Hours:", close == std::string::npos ? 0 : close);
    if (open == std::string::npos || close == std::string::npos ||
        creditsLabel == std::string::npos) {
        throw std::runtime_error("Invalid subject header: " + line);
    }

    Subject subject;
    subject.name = trim(value.substr(0, open));
    subject.code = trim(value.substr(open + 2, close - open - 2));
    const auto creditsText = trim(value.substr(creditsLabel + std::string("Credit Hours:").size()));
    try {
        subject.creditHours = std::stoi(creditsText);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid credit-hour value: " + line);
    }
    if (subject.name.empty() || subject.code.empty() || subject.creditHours <= 0) {
        throw std::runtime_error("Subject name, code, and positive credit hours are required: " + line);
    }
    return subject;
}

void parseTotalLine(const std::string& line, Subject& subject) {
    const auto colon = line.find(':');
    const auto slash = line.find('/', colon == std::string::npos ? 0 : colon + 1);
    if (colon == std::string::npos || slash == std::string::npos) {
        throw std::runtime_error("Invalid Total line: " + line);
    }
    try {
        subject.obtainedMarks = std::stoi(trim(line.substr(colon + 1, slash - colon - 1)));
        subject.totalMarks = std::stoi(trim(line.substr(slash + 1)));
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid marks in Total line: " + line);
    }
    if (subject.totalMarks <= 0 || subject.obtainedMarks < 0 ||
        subject.obtainedMarks > subject.totalMarks) {
        throw std::runtime_error("Total marks must be positive and obtained marks must be in range: " + line);
    }
}

std::vector<Student> loadStudents(const std::string& inputPath) {
    std::ifstream input(inputPath);
    if (!input) throw std::runtime_error("Could not open input file: " + inputPath);

    std::vector<std::string> lines;
    for (std::string line; std::getline(input, line);) lines.push_back(trim(line));

    std::vector<Student> students;
    Student* currentStudent = nullptr;

    for (std::size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        if (line.empty() || line[0] == '#') continue;

        if (startsWith(line, "Student:")) {
            students.push_back(parseStudentHeader(line));
            currentStudent = &students.back();
            continue;
        }

        if (startsWith(line, "Subject:")) {
            if (currentStudent == nullptr) {
                throw std::runtime_error("A subject appeared before any student header.");
            }
            Subject subject = parseSubjectHeader(line);
            bool foundTotal = false;
            for (++i; i < lines.size(); ++i) {
                if (startsWith(lines[i], "Total:")) {
                    parseTotalLine(lines[i], subject);
                    foundTotal = true;
                    break;
                }
                if (startsWith(lines[i], "Student:") || startsWith(lines[i], "Subject:")) {
                    throw std::runtime_error("Missing Total line for subject: " + subject.name);
                }
            }
            if (!foundTotal) throw std::runtime_error("Missing Total line for subject: " + subject.name);
            currentStudent->subjects.push_back(subject);
        }
    }

    if (students.empty()) throw std::runtime_error("No student records were found in " + inputPath);
    for (const Student& student : students) {
        if (student.subjects.empty()) {
            throw std::runtime_error("No subjects found for student: " + student.name);
        }
    }
    return students;
}

void writeReport(const std::vector<Student>& students, const std::string& outputPath) {
    std::ofstream output(outputPath);
    if (!output) throw std::runtime_error("Could not create output file: " + outputPath);

    output << "SGPA = Total Earned Grade Points / Total Credit Hours\n\n";
    for (const Student& student : students) {
        double totalPoints = 0.0;
        int totalCreditHours = 0;
        int earnedCreditHours = 0;

        output << "Student: " << student.name << " (" << student.studentId
               << "), Semester: " << student.semester << '\n';

        for (const Subject& subject : student.subjects) {
            const double percentage =
                100.0 * static_cast<double>(subject.obtainedMarks) /
                static_cast<double>(subject.totalMarks);
            const double points = gradePoint(percentage);
            totalPoints += points * subject.creditHours;
            totalCreditHours += subject.creditHours;
            if (points > 0.0) earnedCreditHours += subject.creditHours;

            output << std::fixed << std::setprecision(2)
                   << "  " << subject.name << " (" << subject.code << ")"
                   << " | Credits: " << subject.creditHours
                   << " | Marks: " << subject.obtainedMarks << '/' << subject.totalMarks
                   << " | Percentage: " << percentage << "%"
                   << " | Grade: " << letterGrade(percentage)
                   << " | Grade points: " << points << '\n';
        }

        const double sgpa = totalCreditHours > 0 ? totalPoints / totalCreditHours : 0.0;
        output << "  Total credit hours: " << totalCreditHours << '\n'
               << "  Earned credit hours: " << earnedCreditHours << '\n'
               << std::fixed << std::setprecision(2)
               << "  Total earned points: " << totalPoints << '\n'
               << std::setprecision(3) << "  SGPA: " << sgpa << " / 4.000\n\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string inputPath = argc > 1 ? argv[1] : "data/academic_records.example.txt";
    const std::string outputPath = argc > 2 ? argv[2] : "student_gpas.txt";

    try {
        const auto students = loadStudents(inputPath);
        writeReport(students, outputPath);
        std::cout << "SGPA report generated for " << students.size()
                  << " student record(s): " << outputPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
