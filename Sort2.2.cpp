#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <set>
#include <sstream>

using namespace std;
using namespace chrono;

// ============================================================================
// 1. Data Structure (Variant 3 - Schedule Records)
// ============================================================================
struct ScheduleRecord {
    int groupNumber;
    char discipline[50];
    char type[10];
    int roomNumber;
    int dayOfWeek;
    int classNumber;

    ScheduleRecord() : groupNumber(0), roomNumber(0), dayOfWeek(0), classNumber(0) {
        memset(discipline, 0, sizeof(discipline));
        memset(type, 0, sizeof(type));
    }

    bool operator<(const ScheduleRecord& other) const {
        if (dayOfWeek != other.dayOfWeek)
            return dayOfWeek < other.dayOfWeek;
        return classNumber < other.classNumber;
    }

    void print() const {
        cout << "Gr." << groupNumber << " " << discipline << " " << type
             << " Rm." << roomNumber << " Day" << dayOfWeek << " Class" << classNumber << endl;
    }
};

// ============================================================================
// 2. Helper Functions
// ============================================================================

void generateDataFile(const string& filename, int count) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error creating file: " << filename << endl;
        return;
    }

    const char* disciplines[] = {"Math", "Physics", "CS", "History", "English", "Philosophy", "Chemistry", "Biology"};
    const char* types[] = {"lek", "lab", "prak"};

    set<int> usedKeys;
    srand(static_cast<unsigned>(time(nullptr)));

    int generated = 0, attempts = 0;
    while (generated < count && attempts < count * 10) {
        ScheduleRecord rec;
        rec.groupNumber = 100 + (rand() % 50);
        strncpy(rec.discipline, disciplines[rand() % 8], sizeof(rec.discipline) - 1);
        strncpy(rec.type, types[rand() % 3], sizeof(rec.type) - 1);
        rec.roomNumber = 100 + (rand() % 500);
        rec.dayOfWeek = 1 + (rand() % 6);  // Days 1-6 only (no Sunday)
        rec.classNumber = 1 + (rand() % 6);

        int key = rec.dayOfWeek * 100 + rec.classNumber;
        if (usedKeys.find(key) == usedKeys.end()) {
            usedKeys.insert(key);
            file.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
            generated++;
        }
        attempts++;
    }
    file.close();
}

int countRecords(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) return 0;
    streamsize size = file.tellg();
    file.close();
    return static_cast<int>(size / sizeof(ScheduleRecord));
}

void printFileFormatted(const string& filename, const string& title, int limit = 0) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "File " << filename << " not found!" << endl;
        return;
    }

    ScheduleRecord rec;
    int count = 0;

    if (!title.empty()) {
        cout << title << endl;
        cout << string(70, '-') << endl;
    }
    cout << left << setw(6) << "Group"
         << setw(22) << "Discipline"
         << setw(8) << "Type"
         << setw(8) << "Room"
         << setw(6) << "Day"
         << "Class" << endl;
    cout << string(70, '-') << endl;

    while (file.read(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord))) {
        cout << left << setw(6) << rec.groupNumber
             << setw(22) << rec.discipline
             << setw(8) << rec.type
             << setw(8) << rec.roomNumber
             << setw(6) << rec.dayOfWeek
             << rec.classNumber << endl;
        count++;
        if (limit > 0 && count >= limit) break;
    }
    cout << string(70, '-') << endl;
    cout << "Total records: " << count << endl << endl;
    file.close();
}

void printFileAsArray(const string& filename, const string& label) {
    ifstream file(filename, ios::binary);
    if (!file) return;
    cout << label << ": ";
    ScheduleRecord rec;
    bool first = true;
    while (file.read(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord))) {
        if (!first) cout << " ";
        cout << "D" << rec.dayOfWeek << "P" << rec.classNumber;
        first = false;
    }
    cout << endl;
    file.close();
}

void copyFile(const string& src, const string& dst) {
    ifstream in(src, ios::binary);
    ofstream out(dst, ios::binary);
    out << in.rdbuf();
    in.close(); out.close();
}

void printSeparator(const string& title) {
    cout << "\n" << string(70, '=') << endl;
    cout << " " << title << endl;
    cout << string(70, '=') << endl << endl;
}

// ============================================================================
// 3. Algorithm 1: Straight Merge Sort (External)
// ============================================================================

void splitFile(const string& src, const string& dest1, const string& dest2, int blockSize) {
    ifstream in(src, ios::binary);
    ofstream out1(dest1, ios::binary), out2(dest2, ios::binary);
    if (!in || !out1 || !out2) return;

    ScheduleRecord rec;
    int count = 0;
    bool toFile1 = true;

    while (in.read(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord))) {
        if (toFile1)
            out1.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
        else
            out2.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
        count++;
        if (count == blockSize) { toFile1 = !toFile1; count = 0; }
    }
    in.close(); out1.close(); out2.close();
}

void mergeFiles(const string& dest, const string& src1, const string& src2, int runSize) {
    ifstream in1(src1, ios::binary), in2(src2, ios::binary);
    ofstream out(dest, ios::binary);
    if (!in1 || !in2 || !out) return;

    ScheduleRecord rec1, rec2;
    bool has1 = static_cast<bool>(in1.read(reinterpret_cast<char*>(&rec1), sizeof(ScheduleRecord)));
    bool has2 = static_cast<bool>(in2.read(reinterpret_cast<char*>(&rec2), sizeof(ScheduleRecord)));

    while (has1 || has2) {
        int count1 = 0, count2 = 0;
        while (count1 < runSize && count2 < runSize && has1 && has2) {
            if (rec1 < rec2) {
                out.write(reinterpret_cast<char*>(&rec1), sizeof(ScheduleRecord));
                count1++;
                has1 = static_cast<bool>(in1.read(reinterpret_cast<char*>(&rec1), sizeof(ScheduleRecord)));
            } else {
                out.write(reinterpret_cast<char*>(&rec2), sizeof(ScheduleRecord));
                count2++;
                has2 = static_cast<bool>(in2.read(reinterpret_cast<char*>(&rec2), sizeof(ScheduleRecord)));
            }
        }
        while (count1 < runSize && has1) {
            out.write(reinterpret_cast<char*>(&rec1), sizeof(ScheduleRecord));
            count1++;
            has1 = static_cast<bool>(in1.read(reinterpret_cast<char*>(&rec1), sizeof(ScheduleRecord)));
        }
        while (count2 < runSize && has2) {
            out.write(reinterpret_cast<char*>(&rec2), sizeof(ScheduleRecord));
            count2++;
            has2 = static_cast<bool>(in2.read(reinterpret_cast<char*>(&rec2), sizeof(ScheduleRecord)));
        }
    }
    in1.close(); in2.close(); out.close();
}

double straightMergeSort(const string& filename, bool verbose = false) {
    string tempB = "temp_b.dat", tempC = "temp_c.dat";
    int n = countRecords(filename);
    if (n <= 1) return 0.0;

    auto start = high_resolution_clock::now();

    int runSize = 1, iteration = 1;
    while (runSize < n) {
        if (verbose) {
            cout << "Iteration " << iteration << " (blockSize = " << runSize << ")" << endl;
            cout << "Split Phase:" << endl;
        }
        splitFile(filename, tempB, tempC, runSize);
        if (verbose) {
            printFileAsArray(tempB, "File B");
            printFileAsArray(tempC, "File C");
            cout << "Merge Phase:" << endl;
        }
        mergeFiles(filename, tempB, tempC, runSize);
        if (verbose) {
            printFileAsArray(filename, "File A (result)");
            cout << endl;
        }
        runSize *= 2;
        iteration++;
    }
    auto stop = high_resolution_clock::now();
    remove(tempB.c_str()); remove(tempC.c_str());

    return duration<double, milli>(stop - start).count();
}

// ============================================================================
// 4. Algorithm 2: Natural Merge Sort (External)
// ============================================================================

void splitNatural(const string& src, const string& dest1, const string& dest2) {
    ifstream in(src, ios::binary);
    ofstream out1(dest1, ios::binary), out2(dest2, ios::binary);
    if (!in) return;

    ScheduleRecord rec, prev;
    bool first = true, writeToFile1 = true;

    while (in.read(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord))) {
        if (first) {
            prev = rec; first = false;
            if (writeToFile1)
                out1.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
            else
                out2.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
            continue;
        }
        if (rec < prev) writeToFile1 = !writeToFile1;
        if (writeToFile1)
            out1.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
        else
            out2.write(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord));
        prev = rec;
    }
    in.close(); out1.close(); out2.close();
}

int countSeries(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) return 0;
    ScheduleRecord rec, prev;
    int seriesCount = 0;
    bool first = true;
    while (file.read(reinterpret_cast<char*>(&rec), sizeof(ScheduleRecord))) {
        if (first) { seriesCount = 1; first = false; prev = rec; continue; }
        if (rec < prev) seriesCount++;
        prev = rec;
    }
    file.close();
    return seriesCount;
}

double naturalMergeSort(const string& filename) {
    string tempB = "temp_b.dat", tempC = "temp_c.dat", tempA = "temp_a.dat";
    auto start = high_resolution_clock::now();

    splitNatural(filename, tempB, tempC);
    bool sorted = false;
    while (!sorted) {
        mergeFiles(tempA, tempB, tempC, 100000);
        if (countSeries(tempA) <= 1) {
            sorted = true;
            copyFile(tempA, filename);
        } else {
            splitNatural(tempA, tempB, tempC);
        }
    }
    auto stop = high_resolution_clock::now();
    remove(tempB.c_str()); remove(tempC.c_str()); remove(tempA.c_str());
    return duration<double, milli>(stop - start).count();
}

// ============================================================================
// 5. TEST: Numeric Example from Assignment 1, Section 2
// ============================================================================

struct NumericRecord {
    int key;
    NumericRecord(int k = 0) : key(k) {}
    bool operator<(const NumericRecord& other) const { return key < other.key; }
};

void generateNumericFile(const string& filename, const vector<int>& data) {
    ofstream file(filename, ios::binary);
    for (int val : data) {
        NumericRecord rec(val);
        file.write(reinterpret_cast<char*>(&rec), sizeof(NumericRecord));
    }
    file.close();
}

void printNumericFile(const string& filename, const string& label) {
    ifstream file(filename, ios::binary);
    if (!file) return;
    cout << label << ": ";
    NumericRecord rec;
    bool first = true;
    while (file.read(reinterpret_cast<char*>(&rec), sizeof(NumericRecord))) {
        if (!first) cout << " ";
        cout << rec.key;
        first = false;
    }
    cout << endl;
    file.close();
}

void splitNumericFile(const string& src, const string& dest1, const string& dest2, int blockSize) {
    ifstream in(src, ios::binary);
    ofstream out1(dest1, ios::binary), out2(dest2, ios::binary);
    if (!in || !out1 || !out2) return;
    NumericRecord rec;
    int count = 0;
    bool toFile1 = true;
    while (in.read(reinterpret_cast<char*>(&rec), sizeof(NumericRecord))) {
        if (toFile1) out1.write(reinterpret_cast<char*>(&rec), sizeof(NumericRecord));
        else out2.write(reinterpret_cast<char*>(&rec), sizeof(NumericRecord));
        count++;
        if (count == blockSize) { toFile1 = !toFile1; count = 0; }
    }
    in.close(); out1.close(); out2.close();
}

void mergeNumericFiles(const string& dest, const string& src1, const string& src2, int runSize) {
    ifstream in1(src1, ios::binary), in2(src2, ios::binary);
    ofstream out(dest, ios::binary);
    if (!in1 || !in2 || !out) return;
    NumericRecord rec1, rec2;
    bool has1 = static_cast<bool>(in1.read(reinterpret_cast<char*>(&rec1), sizeof(NumericRecord)));
    bool has2 = static_cast<bool>(in2.read(reinterpret_cast<char*>(&rec2), sizeof(NumericRecord)));
    while (has1 || has2) {
        int c1 = 0, c2 = 0;
        while (c1 < runSize && c2 < runSize && has1 && has2) {
            if (rec1.key <= rec2.key) {
                out.write(reinterpret_cast<char*>(&rec1), sizeof(NumericRecord));
                c1++;
                has1 = static_cast<bool>(in1.read(reinterpret_cast<char*>(&rec1), sizeof(NumericRecord)));
            } else {
                out.write(reinterpret_cast<char*>(&rec2), sizeof(NumericRecord));
                c2++;
                has2 = static_cast<bool>(in2.read(reinterpret_cast<char*>(&rec2), sizeof(NumericRecord)));
            }
        }
        while (c1 < runSize && has1) {
            out.write(reinterpret_cast<char*>(&rec1), sizeof(NumericRecord));
            c1++;
            has1 = static_cast<bool>(in1.read(reinterpret_cast<char*>(&rec1), sizeof(NumericRecord)));
        }
        while (c2 < runSize && has2) {
            out.write(reinterpret_cast<char*>(&rec2), sizeof(NumericRecord));
            c2++;
            has2 = static_cast<bool>(in2.read(reinterpret_cast<char*>(&rec2), sizeof(NumericRecord)));
        }
    }
    in1.close(); in2.close(); out.close();
}

void testNumericExample() {
    vector<int> initialData = {8, 2, 13, 4, 15, 6, 9, 11, 3, 7, 5, 10, 1, 12, 14};
    string testFile = "test_numeric.dat";
    string tempB = "temp_b.dat", tempC = "temp_c.dat";

    cout << "Initial File A:" << endl;
    generateNumericFile(testFile, initialData);
    printNumericFile(testFile, "File A");
    cout << endl;

    int n = initialData.size(), blockSize = 1, iteration = 1;
    while (blockSize < n) {
        cout << "Iteration " << iteration << " (blockSize = " << blockSize << ")" << endl;
        cout << "Split Phase:" << endl;
        splitNumericFile(testFile, tempB, tempC, blockSize);
        printNumericFile(tempB, "File B");
        printNumericFile(tempC, "File C");
        cout << "Merge Phase:" << endl;
        mergeNumericFiles(testFile, tempB, tempC, blockSize);
        printNumericFile(testFile, "File A (result)");
        cout << endl;
        blockSize *= 2; iteration++;
    }
    cout << "Final Sorted File A:" << endl;
    printNumericFile(testFile, "File A");
    cout << endl;
    remove(testFile.c_str()); remove(tempB.c_str()); remove(tempC.c_str());
}

// ============================================================================
// 6. Complexity Analysis (Assignment 3) - FIXED VERSION
// ============================================================================

void runComplexityAnalysis() {
    cout << "ASSIGNMENT 3: PRACTICAL COMPLEXITY ANALYSIS" << endl << endl;
    int testSizes[] = {8, 16, 32};
    double straightTimes[3], naturalTimes[3];

    for (int i = 0; i < 3; i++) {
        int n = testSizes[i];
        string testFile = "test_complexity_" + to_string(n) + ".dat"; // Уникальное имя для каждого размера

        printSeparator("TEST WITH " + to_string(n) + " RECORDS");

        // =========================================================================
        // STRAIGHT MERGE SORT TEST
        // =========================================================================
        cout << "[STRAIGHT MERGE SORT]" << endl;

        // Генерируем НОВЫЙ неотсортированный файл
        generateDataFile(testFile, n);

        cout << "\n>>> UNSORTED FILE (" << n << " records):" << endl;
        printFileFormatted(testFile, "", 0); // 0 = показать все записи

        // Straight Merge Sort
        straightTimes[i] = straightMergeSort(testFile);
        cout << ">>> Straight Merge Sort completed: " << fixed << setprecision(3) << straightTimes[i] << " ms" << endl;

        cout << "\n>>> SORTED FILE (Straight) (" << n << " records):" << endl;
        printFileFormatted(testFile, "", 0);

        // =========================================================================
        // NATURAL MERGE SORT TEST
        // =========================================================================
        cout << "\n[NATURAL MERGE SORT]" << endl;

        // Генерируем НОВЫЙ неотсортированный файл для Natural Sort
        generateDataFile(testFile, n);

        cout << "\n>>> UNSORTED FILE (" << n << " records):" << endl;
        printFileFormatted(testFile, "", 0);

        // Natural Merge Sort
        naturalTimes[i] = naturalMergeSort(testFile);
        cout << ">>> Natural Merge Sort completed: " << fixed << setprecision(3) << naturalTimes[i] << " ms" << endl;

        cout << "\n>>> SORTED FILE (Natural) (" << n << " records):" << endl;
        printFileFormatted(testFile, "", 0);

        // Cleanup
        remove(testFile.c_str());
    }

    // Таблица результатов
    cout << "\n" << string(70, '=') << endl;
    cout << "RESULTS TABLE" << endl;
    cout << string(70, '=') << endl;
    cout << left << setw(10) << "Records" << setw(18) << "Straight(ms)" << "Natural(ms)" << endl;
    cout << string(70, '-') << endl;
    cout << fixed << setprecision(3);
    for (int i = 0; i < 3; i++) {
        cout << left << setw(10) << testSizes[i]
             << setw(18) << straightTimes[i]
             << naturalTimes[i] << endl;
    }
    cout << string(70, '=') << endl << endl;
}

// ============================================================================
// TEST: Natural Merge Sort Example from Section 4 (Numeric Keys)
// ============================================================================

void printNumericWithRuns(const vector<int>& data, const string& label) {
    if (!label.empty()) cout << label;
    if (data.empty()) { cout << "(empty)" << endl; return; }

    for (size_t i = 0; i < data.size(); i++) {
        cout << data[i];
        if (i + 1 < data.size() && data[i+1] < data[i]) {
            cout << "'";
        }
        if (i + 1 < data.size()) cout << " ";
    }
    cout << endl;
}

int countRuns(const vector<int>& data) {
    if (data.empty()) return 0;
    int runs = 1;
    for (size_t i = 1; i < data.size(); i++) {
        if (data[i] < data[i-1]) runs++;
    }
    return runs;
}

void splitRuns(const vector<int>& src, vector<int>& dest1, vector<int>& dest2) {
    dest1.clear(); dest2.clear();
    if (src.empty()) return;

    bool writeToFirst = true;
    for (size_t i = 0; i < src.size(); i++) {
        (writeToFirst ? dest1 : dest2).push_back(src[i]);
        if (i + 1 < src.size() && src[i+1] < src[i]) {
            writeToFirst = !writeToFirst;
        }
    }
}

vector<int> mergeRuns(const vector<int>& v1, const vector<int>& v2) {
    vector<int> result;
    size_t i1 = 0, i2 = 0;

    while (i1 < v1.size() || i2 < v2.size()) {
        size_t runStart1 = i1, runStart2 = i2;

        while (i1 + 1 < v1.size() && v1[i1+1] >= v1[i1]) i1++;
        size_t runEnd1 = i1;
        if (i1 < v1.size()) i1++;

        while (i2 + 1 < v2.size() && v2[i2+1] >= v2[i2]) i2++;
        size_t runEnd2 = i2;
        if (i2 < v2.size()) i2++;

        size_t p1 = runStart1, p2 = runStart2;
        while (p1 <= runEnd1 && p1 < v1.size() && p2 <= runEnd2 && p2 < v2.size()) {
            if (v1[p1] <= v2[p2]) {
                result.push_back(v1[p1++]);
            } else {
                result.push_back(v2[p2++]);
            }
        }
        while (p1 <= runEnd1 && p1 < v1.size()) result.push_back(v1[p1++]);
        while (p2 <= runEnd2 && p2 < v2.size()) result.push_back(v2[p2++]);
    }
    return result;
}

void testNaturalMergeExample() {
    cout << endl;
    printSeparator("TEST: Natural Merge Sort Example from Section 4");

    vector<int> data = {17, 31, 5, 59, 13, 41, 43, 67, 11, 23, 29, 47, 3, 7, 71, 2, 19, 57, 37, 61};

    cout << "Initial data:" << endl;
    cout << "File A: ";
    printNumericWithRuns(data, "");
    cout << "Number of runs: " << countRuns(data) << endl;
    cout << endl;

    vector<int> fileB, fileC, fileA = data;
    int iteration = 1;

    while (countRuns(fileA) > 1) {
        cout << "Iteration " << iteration << endl;
        cout << endl;

        cout << "Split Phase:" << endl;
        splitRuns(fileA, fileB, fileC);

        cout << "File B: ";
        printNumericWithRuns(fileB, "");
        cout << "  Number of runs: " << countRuns(fileB) << endl;

        cout << "File C: ";
        printNumericWithRuns(fileC, "");
        cout << "  Number of runs: " << countRuns(fileC) << endl;
        cout << endl;

        cout << "Merge Phase:" << endl;
        fileA = mergeRuns(fileB, fileC);

        cout << "File A (result): ";
        printNumericWithRuns(fileA, "");
        cout << "  Number of runs: " << countRuns(fileA) << endl;
        cout << endl;

        iteration++;
    }

    cout << "Final Sorted File A:" << endl;
    cout << "File A: ";
    printNumericWithRuns(fileA, "");
    cout << "Number of runs: " << countRuns(fileA) << endl;
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    cout << "================================================================" << endl;
    cout << "PRACTICAL WORK 2.2: EXTERNAL SORTING ALGORITHMS" << endl;
    cout << "================================================================" << endl << endl;

    // =========================================================================
    // ASSIGNMENT 1: STRAIGHT MERGE SORT
    // =========================================================================
    printSeparator("ASSIGNMENT 1: STRAIGHT MERGE SORT (Direct Merge)");

    cout << ">>> TEST: Numeric Example from Section 2 <<<" << endl;
    cout << "Initial array: 8 2 13 4 15 6 9 11 3 7 5 10 1 12 14" << endl << endl;
    testNumericExample();

    const string mainFile = "schedule.dat";
    const int RECORD_COUNT = 32;

    cout << ">>> TEST: File with ScheduleRecord structures <<<" << endl;
    cout << "Generating " << RECORD_COUNT << " unique unsorted records..." << endl;
    generateDataFile(mainFile, RECORD_COUNT);
    cout << "File created: " << mainFile << ", Records: " << countRecords(mainFile) << endl << endl;

    printFileFormatted(mainFile, "File BEFORE sorting", 0);

    cout << "Running Straight Merge Sort..." << endl << endl;
    double straightTime = straightMergeSort(mainFile, false);
    cout << fixed << setprecision(3);
    cout << "Sorting completed in: " << straightTime << " ms" << endl << endl;

    printFileFormatted(mainFile, "File AFTER sorting", 0);

    cout << endl << string(70, '=') << endl << endl;


    // =========================================================================
    // ASSIGNMENT 2: NATURAL MERGE SORT
    // =========================================================================
    printSeparator("ASSIGNMENT 2: NATURAL MERGE SORT");

    cout << ">>> TEST: Numeric Example from Section 4 <<<" << endl;
    cout << "Initial array: 17 31 5 59 13 41 43 67 11 23 29 47 3 7 71 2 19 57 37 61" << endl;
    cout << "Runs marked with ' : 17 31' 5 59' 13 41 43 67' 11 23 29 47' 3 7 71' 2 19 57' 37 61" << endl << endl;
    testNaturalMergeExample();

    cout << ">>> TEST: File with ScheduleRecord structures <<<" << endl;
    cout << "Regenerating file with " << RECORD_COUNT << " records..." << endl;
    generateDataFile(mainFile, RECORD_COUNT);
    cout << "File: " << mainFile << ", Records: " << countRecords(mainFile) << endl << endl;


    printFileFormatted(mainFile, "File BEFORE sorting", 0);

    cout << "Running Natural Merge Sort..." << endl;
    double naturalTime = naturalMergeSort(mainFile);
    cout << fixed << setprecision(3);
    cout << "Sorting completed in: " << naturalTime << " ms" << endl << endl;


    printFileFormatted(mainFile, "File AFTER sorting", 0);

    cout << endl << string(70, '=') << endl << endl;


    // =========================================================================
    // COMPLEXITY ANALYSIS (Assignment 3) - FIXED
    // =========================================================================
    runComplexityAnalysis();


    // =========================================================================
    // FINAL SUMMARY
    // =========================================================================
    printSeparator("SUMMARY");

    cout << fixed << setprecision(3);
    cout << "Algorithm 1 (Straight Merge): " << straightTime << " ms" << endl;
    cout << "Algorithm 2 (Natural Merge):  " << naturalTime << " ms" << endl << endl;
    cout << "Both algorithms sorted " << RECORD_COUNT << " records by Day of Week." << endl;
    cout << "Sorting key: dayOfWeek (primary), classNumber (secondary)" << endl << endl;

    // Cleanup
    remove(mainFile.c_str());

    cout << "Program completed successfully." << endl;
    return 0;
}