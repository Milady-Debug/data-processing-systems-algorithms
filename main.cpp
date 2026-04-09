/*
#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace chrono;

// ================== Generation of Pratt steps ==================

vector<int> generatePrattSteps(int n) {
    set<int> stepsSet;

    long long pow2 = 1;
    for (int a = 0; pow2 <= n; a++) {
        long long pow3 = 1;
        for (int b = 0; pow3 <= n / pow2; b++) {
            long long step = pow2 * pow3;
            if (step <= n) {
                stepsSet.insert((int)step);
            }
            pow3 *= 3;
        }
        pow2 *= 2;
    }

    vector<int> steps(stepsSet.begin(), stepsSet.end());
    sort(steps.rbegin(), steps.rend()); // sort in descending order
    return steps;
}

// ================== Improved algorithm: shell sort with pratt steps ==================

void shellSortPratt(int* a, int n, long long& cmp, long long& mov, double& elapsed) {
    cmp = 0;
    mov = 0;
    // Generation of Pratt steps
    vector<int> steps = generatePrattSteps(n);
    auto start = high_resolution_clock::now();
    if (n <= 1) return; // обработка крайнего случая
    // Outer loop over steps
    mov++; // initialization k = 0
    for (int k = 0; k < steps.size(); k++) {
        cmp++; // check k < steps.size()
        int step = steps[k];
        mov += 3; // assignment step = steps[k] и k++  initialization i = step
        for (int i = step; i < n; i++) {
            cmp++; // i < n
            // Remember current element
            int temp = a[i];
            int j = i;
            mov += 3; // assignment j = i  assignment temp = a[i] i++

            // Search for insertion position
            while (j >= step) {
                cmp+=2; // comparison a[j - step] > temp
                if (a[j - step] > temp) {
                    // Shift element
                    a[j] = a[j - step];
                    j -= step;
                    mov += 2; // assignment j = j - step a[j] = a[j - step];
                    cmp++; // next while condition check
                } else {
                    break;
                }
            }
            // Insert element at found position
            cmp++; // check j != i
            if (j != i) {
                a[j] = temp;
                mov++; // assignment during insertion
            }
        }
        cmp++; // check k < steps.size() for next iteration
    }
    cmp++; // k == steps.size()

    auto end = high_resolution_clock::now();
    elapsed = duration<double, milli>(end - start).count();
}

// ================== FAST ALGORITHM: QUICK SORT (HOARE) WITH TWO FUNCTIONS ==================

// Partition function using Hoare scheme
int partitionHoare(int* a, int low, int high, long long& cmp, long long& mov) {
    // Choose middle element as pivot (typical for Hoare)

    int pivot = a[low + (high - low) / 2];
    mov++; // assignment pivot

    int i = low - 1;
    int j = high + 1;
    mov += 2; // assignments i and j

    while (true) {
        cmp++;        // Find element from left that is >= pivot
        do {
            i++;
            mov++; // increment i
            cmp+=2; // i <= high  check a[i] < pivot
        } while (i <= high && a[i] < pivot); // добавлена проверка границ

        // Find element from right that is <= pivot
        do {
            j--;
            mov++; // decrement j
            cmp+=2; // j >= low check a[j] > pivot
        } while (j >= low && a[j] > pivot); // добавлена проверка границ

        cmp++; // check i >= j
        if (i >= j) {
            return j; // return partition index
        }

        // Swap elements
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
        mov += 3; // three assignments during swap
    }
}

// Main quick sort recursive function
void quickSortHoare(int* a, int low, int high, long long& cmp, long long& mov) {
    cmp++; // check low < high
    if (low < high) {
        // Partition the array and get partition index
        int pi = partitionHoare(a, low, high, cmp, mov);
        mov++;
        // Recursively sort left and right parts
        // In Hoare scheme, we sort [low, pi] and [pi+1, high]
        quickSortHoare(a, low, pi, cmp, mov);
        quickSortHoare(a, pi + 1, high, cmp, mov);
    }
}

// Wrapper function for easier use (with timing)
void quickSortHoareWrapper(int* a, int n, long long& cmp, long long& mov, double& elapsed) {
    cmp = 0;
    mov = 0;
    auto start = high_resolution_clock::now();
    quickSortHoare(a, 0, n - 1, cmp, mov);
    auto end = high_resolution_clock::now();
    elapsed = duration<double, milli>(end - start).count();
}


// ================== Array generators ==================

int* generateRandomInt(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) {
        arr[i] = rand() % 10000;
    }
    return arr;
}

int* generateAscending(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    return arr;
}

int* generateDescending(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    return arr;
}

int* copyIntArray(int* src, int n) {
    int* dest = new int[n];
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
    return dest;
}

// ================== Testing on random arrays ==================

void testRandomArrays() {
    cout << "\n=============================================================" << endl;
    cout << "TABLE 1.1: RANDOM ARRAYS (AVERAGE CASE)" << endl;
    cout << "=============================================================" << endl;

    int sizes[] = {100, 200, 500, 1000, 2000, 5000, 10000, 100000, 200000, 500000, 1000000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    cout << "n" << "\t" << "Algorithm" << "\t\t" << "Time(ms)" << "\t" << "C" << "\t\t" << "M" << "\t\t" << "T=C+M" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];

        int* randArr = generateRandomInt(n);

        // Shell sort
        int* work1 = copyIntArray(randArr, n);
        long long c1, m1;
        double t1;
        shellSortPratt(work1, n, c1, m1, t1);
        cout << n << "\t" << "Shell Pratt" << "\t\t" << t1 << "\t" << c1 << "\t" << m1 << "\t" << (c1 + m1) << endl;
        delete[] work1;

        // Quick sort
        int* work2 = copyIntArray(randArr, n);
        long long c2=0, m2=0;
        double t2;
        quickSortHoareWrapper(work2, n, c2, m2, t2);
        cout << n << "\t" << "Quick Hoare" << "\t\t" << t2 << "\t" << c2 << "\t" << m2 << "\t" << (c2 + m2) << endl;
        delete[] work2;

        cout << "--------------------------------------------------------------------------------" << endl;

        delete[] randArr;
    }
}

// ================== Testing on descending arrays ==================

void testDescendingArrays() {
    cout << "\n=============================================================" << endl;
    cout << "TABLE 1.4: DESCENDING ARRAYS" << endl;
    cout << "=============================================================" << endl;

    int sizes[] = {100, 200, 500, 1000, 2000, 5000, 10000, 100000, 200000, 500000, 1000000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    cout << "n" << "\t" << "Algorithm" << "\t\t" << "Time(ms)" << "\t" << "C" << "\t\t" << "M" << "\t\t" << "T=C+M" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];

        int* descArr = generateDescending(n);

        // Shell sort
        int* work1 = copyIntArray(descArr, n);
        long long c1, m1;
        double t1;
        shellSortPratt(work1, n, c1, m1, t1);
        cout << n << "\t" << "Shell Pratt" << "\t\t" << t1 << "\t" << c1 << "\t" << m1 << "\t" << (c1 + m1) << endl;
        delete[] work1;

        // Quick sort
        int* work2 = copyIntArray(descArr, n);
        long long c2, m2;
        double t2;
        quickSortHoareWrapper(work2, n, c2, m2, t2);
        cout << n << "\t" << "Quick Hoare" << "\t\t" << t2 << "\t" << c2 << "\t" << m2 << "\t" << (c2 + m2) << endl;
        delete[] work2;

        cout << "--------------------------------------------------------------------------------" << endl;

        delete[] descArr;
    }
}

// ================== Testing on ascending arrays ==================

void testAscendingArrays() {
    cout << "\n=============================================================" << endl;
    cout << "TABLE 1.5: ASCENDING ARRAYS" << endl;
    cout << "=============================================================" << endl;

    int sizes[] = {100, 200, 500, 1000, 2000, 5000, 10000, 100000, 200000, 500000, 1000000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    cout << "n" << "\t" << "Algorithm" << "\t\t" << "Time(ms)" << "\t" << "C" << "\t\t" << "M" << "\t\t" << "T=C+M" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];

        int* ascArr = generateAscending(n);

        // Shell sort
        int* work1 = copyIntArray(ascArr, n);
        long long c1, m1;
        double t1;
        shellSortPratt(work1, n, c1, m1, t1);
        cout << n << "\t" << "Shell Pratt" << "\t\t" << t1 << "\t" << c1 << "\t" << m1 << "\t" << (c1 + m1) << endl;
        delete[] work1;

        // Quick sort
        int* work2 = copyIntArray(ascArr, n);
        long long c2, m2;
        double t2;
        quickSortHoareWrapper(work2, n, c2, m2, t2);
        cout << n << "\t" << "Quick Hoare" << "\t\t" << t2 << "\t" << c2 << "\t" << m2 << "\t" << (c2 + m2) << endl;
        delete[] work2;

        cout << "--------------------------------------------------------------------------------" << endl;

        delete[] ascArr;
    }
}

// ================== Asymptotic analysis ==================

// ================== Analysis of dependency on initial order ==================


// ================== Main function ==================


void bubbleSort(int* a, int n, long long& cmp, long long& mov, double& elapsed) {
    cmp = 0;
    mov = 0;
    auto start = high_resolution_clock::now();
    mov++;
    for (int i = 0; i < n - 1; i++) {
        cmp++;
        mov+=2;
        for (int j = 0; j < n - i - 1; j++) {
            mov++;
            cmp+=2;
            if (a[j] > a[j + 1]) {
                int tmp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = tmp;
                mov += 3;
            }
        }
        cmp++;
    }
    cmp++;
    auto end = high_resolution_clock::now();
    elapsed = duration<double, milli>(end - start).count();
}
int main() {
    srand(time(nullptr));

    cout << "=============================================================" << endl;
    cout << "PRACTICAL WORK No. 2" << endl;
    cout << "SORTING NUMERIC SEQUENCES" << endl;
    cout << "Part 2.1: IMPROVED AND FAST SORTING ALGORITHMS" << endl;
    cout << "=============================================================" << endl;
    cout << "Variant 18:" << endl;
    cout << "- Improved algorithm: Shell sort with Pratt steps" << endl;
    cout << "- Fast algorithm: Quick sort (Hoare)" << endl;
    cout << "=============================================================" << endl;

    testRandomArrays();
    testDescendingArrays();
    testAscendingArrays();
    return 0;
}*/