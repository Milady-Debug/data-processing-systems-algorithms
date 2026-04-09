#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace chrono;
/*
// ================== Helper functions for working with arrays ==================

// Copy character array
char* copyCharArray(char* src, int n) {
    char* dest = new char[n];
    for (int i = 0; i < n; ++i)
        dest[i] = src[i];
    return dest;
}

// Print character array (for debugging, but not used in tests)
void printCharArray(char* arr, int n, const char* name) {
    cout << name << ": [";
    for (int i = 0; i < n; ++i) {
        cout << "'" << arr[i] << "'";
        if (i < n - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// ================== Character deletion algorithms (Task 1) ==================

// First deletion algorithm (shift on each match)
// Parameters:
//   x       - character array
//   n       - array size (passed by reference because it changes)
//   key     - character to delete
//   cmp     - comparison counter (returned)
//   mov     - movement counter (returned)
//   elapsed - execution time in milliseconds (returned)
void delFirstMethod(char* x, int& n, char key, long long& cmp, long long& mov, double& elapsed) {
    auto start = high_resolution_clock::now();
    cmp = 0;
    mov = 0;
    int i = 0;
    mov++;
    while (i < n) {
        cmp+=2;                          // comparison of while condition (i < n) comparison x[i] == key
        if (x[i] == key) {
            // found key - delete by shifting
            mov++;
            for (int j = i; j < n - 1; j++) {
                cmp++;                    // comparison j < n-1 (loop condition)
                x[j] = x[j + 1];
                mov+=2;                     // one movement
            }
            cmp++;
            n++;
            mov++;// size decreased
            // i is not incremented because a new element came to position i
        } else {
            i++;
            mov++;
        }
    }
    cmp++;
    auto end = high_resolution_clock::now();
    elapsed = duration<double, milli>(end - start).count();
}

// Second deletion algorithm (single pass with two indices)
void delSecondMethod(char* x, int& n, char key, long long& cmp, long long& mov, double& elapsed) {
    auto start = high_resolution_clock::now();
    cmp = 0;
    mov = 0;
    int j = 0;
    mov+=2;
    for (int i = 0; i < n; i++) {
        cmp++;                            // comparison i < n (loop condition)
        x[j] = x[i];
        mov+=2;                             // movement
        cmp++;                             // comparison x[i] != key
        if (x[i] != key) {
            j++;
            mov++;
        }
    }
    cmp++;
    n = j;
    mov++;
    auto end = high_resolution_clock::now();
    elapsed = duration<double, milli>(end - start).count();
}

// ================== Character array generators for Task 1 ==================

// All elements equal to key (worst case for deletion – need to delete everything)
char* generateAllKey(int n, char key) {
    char* arr = new char[n];
    for (int i = 0; i < n; ++i)
        arr[i] = key;
    return arr;
}

// Random characters (digits and uppercase letters), key probability ~1/36
char* generateRandomChars(int n, char key) {
    char* arr = new char[n];
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int setSize = sizeof(charset) - 1;
    for (int i = 0; i < n; ++i) {
        arr[i] = charset[rand() % setSize];
    }
    return arr;
}

// No element equals key (best case for deletion – nothing is deleted)
char* generateNoKey(int n, char key) {
    char* arr = new char[n];
    // Use characters different from key. For example, if key = 'X', take 'A'..'W'
    if (key >= 'A' && key <= 'Z') {
        // If key is an uppercase letter, use other letters
        for (int i = 0; i < n; ++i) {
            char c;
            do {
                c = 'A' + (rand() % 26);
            } while (c == key);
            arr[i] = c;
        }
    } else {
        // Otherwise just random letters
        for (int i = 0; i < n; ++i)
            arr[i] = 'A' + (rand() % 26);
    }
    return arr;
}

// ================== Testing deletion algorithms (Task 1) ==================

void runDeletionTests() {
    srand(time(nullptr));

    // Sizes from Task 1
    int sizes[] = {100, 200, 500, 1000, 2000, 5000, 10000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    char key = 'X';   // selected key for deletion

    cout << "===== Task 1: Deleting characters from array =====\n";
    cout << "Key to delete: '" << key << "'\n";

    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];
        cout << "\n---------- Array size: " << n << " ----------\n";

        // Generate reference arrays for three cases
        char* allKeyArr = generateAllKey(n, key);     // all equal to key
        char* randomArr = generateRandomChars(n, key); // random
        char* noKeyArr  = generateNoKey(n, key);       // no key elements

        // Variables for results
        char* work = nullptr;
        long long cmp, mov;
        double elapsed;

        // ----- First algorithm -----
        cout << "\n--- First algorithm (delFirstMethod) ---\n";

        // Case: all elements are deleted (worst for first algorithm?)
        work = copyCharArray(allKeyArr, n);
        int tempN = n;
        delFirstMethod(work, tempN, key, cmp, mov, elapsed);
        cout << "All deleted   : n=" << n
             << ", time=" << elapsed << " ms"
             << ", cmp=" << cmp << ", mov=" << mov
             << ", total=" << (cmp + mov) << endl;
        delete[] work;

        // Case: random filling (average)
        work = copyCharArray(randomArr, n);
        tempN = n;
        delFirstMethod(work, tempN, key, cmp, mov, elapsed);
        cout << "Random        : n=" << n
             << ", time=" << elapsed << " ms"
             << ", cmp=" << cmp << ", mov=" << mov
             << ", total=" << (cmp + mov) << endl;
        delete[] work;

        // Case: no deletions (best)
        work = copyCharArray(noKeyArr, n);
        tempN = n;
        delFirstMethod(work, tempN, key, cmp, mov, elapsed);
        cout << "No deletions  : n=" << n
             << ", time=" << elapsed << " ms"
             << ", cmp=" << cmp << ", mov=" << mov
             << ", total=" << (cmp + mov) << endl;
        delete[] work;

        // ----- Second algorithm -----
        cout << "\n--- Second algorithm (delSecondMethod) ---\n";

        // All deleted
        work = copyCharArray(allKeyArr, n);
        tempN = n;
        delSecondMethod(work, tempN, key, cmp, mov, elapsed);
        cout << "All deleted   : n=" << n
             << ", time=" << elapsed << " ms"
             << ", cmp=" << cmp << ", mov=" << mov
             << ", total=" << (cmp + mov) << endl;
        delete[] work;

        // Random
        work = copyCharArray(randomArr, n);
        tempN = n;
        delSecondMethod(work, tempN, key, cmp, mov, elapsed);
        cout << "Random        : n=" << n
             << ", time=" << elapsed << " ms"
             << ", cmp=" << cmp << ", mov=" << mov
             << ", total=" << (cmp + mov) << endl;
        delete[] work;

        // No deletions
        work = copyCharArray(noKeyArr, n);
        tempN = n;
        delSecondMethod(work, tempN, key, cmp, mov, elapsed);
        cout << "No deletions  : n=" << n
             << ", time=" << elapsed << " ms"
             << ", cmp=" << cmp << ", mov=" << mov
             << ", total=" << (cmp + mov) << endl;
        delete[] work;

        // Free reference arrays
        delete[] allKeyArr;
        delete[] randomArr;
        delete[] noKeyArr;
    }
}

// ================== Sorting algorithms (Tasks 2-4) ==================

void selectionSort(int* a, int n, long long& cmp, long long& mov, double& elapsed) {
    cmp = 0;
    mov = 0;
    auto start = high_resolution_clock::now();
    mov++;
    for (int i = 0; i < n - 1; i++) {
        cmp++;
        int min_idx = i;
        mov+=3;
        for (int j = i + 1; j < n; j++) {
            mov++;
            cmp+=2;
            if (a[j] < a[min_idx]) {
                min_idx = j;
                mov++;
            }
        }
        cmp+=2;
        if (min_idx != i) {
            int tmp = a[i];
            a[i] = a[min_idx];
            a[min_idx] = tmp;
            mov += 3;
        }
    }
    cmp++;
    auto end = high_resolution_clock::now();
    elapsed = duration<double, milli>(end - start).count();
}

void bubbleSort(int* a, int n, long long& cmp, long long& mov, double& elapsed) {
    auto start = high_resolution_clock::now();
    cmp = 0;
    mov = 0;
    mov++;
    for (int i = 0; i < n - 1; i++) {
        cmp++;
        mov+=2;
        for (int j = 0; j < n - i - 1; j++) {
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

int* generateRandomInt(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) arr[i] = rand() % 1000;
    return arr;
}

int* generateAscending(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) arr[i] = i;
    return arr;
}

int* generateDescending(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) arr[i] = n - i;
    return arr;
}

int* copyIntArray(int* src, int n) {
    int* dest = new int[n];
    for (int i = 0; i < n; ++i) dest[i] = src[i];
    return dest;
}

void runSortingTests() {
    srand(time(nullptr));

    int sizes[] = {100, 200, 500, 1000, 2000, 5000, 10000,
                   100000, 200000, 500000, 1000000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    cout << "\n===== Tasks 2-4: Comparison of Selection sort and Bubble sort =====\n";

    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];
        cout << "\n---------- Array size: " << n << " ----------\n";

        int* randArr = generateRandomInt(n);
        int* ascArr  = generateAscending(n);
        int* descArr = generateDescending(n);

        int* work = nullptr;
        long long cmp, mov;
        double elapsed;

        // Selection sort
        cout << "\n--- Selection sort ---\n";
        work = copyIntArray(randArr, n);
        selectionSort(work, n, cmp, mov, elapsed);
        cout << "Average case: n=" << n << ", time=" << elapsed << " ms, cmp=" << cmp << ", mov=" << mov << ", total=" << (cmp+mov) << endl;
        delete[] work;

        work = copyIntArray(ascArr, n);
        selectionSort(work, n, cmp, mov, elapsed);
        cout << "Best case   : n=" << n << ", time=" << elapsed << " ms, cmp=" << cmp << ", mov=" << mov << ", total=" << (cmp+mov) << endl;
        delete[] work;

        work = copyIntArray(descArr, n);
        selectionSort(work, n, cmp, mov, elapsed);
        cout << "Worst case  : n=" << n << ", time=" << elapsed << " ms, cmp=" << cmp << ", mov=" << mov << ", total=" << (cmp+mov) << endl;
        delete[] work;

        // Bubble sort
        cout << "\n--- Bubble sort ---\n";
        work = copyIntArray(randArr, n);
        bubbleSort(work, n, cmp, mov, elapsed);
        cout << "Average case: n=" << n << ", time=" << elapsed << " ms, cmp=" << cmp << ", mov=" << mov << ", total=" << (cmp+mov) << endl;
        delete[] work;

        work = copyIntArray(ascArr, n);
        bubbleSort(work, n, cmp, mov, elapsed);
        cout << "Best case   : n=" << n << ", time=" << elapsed << " ms, cmp=" << cmp << ", mov=" << mov << ", total=" << (cmp+mov) << endl;
        delete[] work;

        work = copyIntArray(descArr, n);
        bubbleSort(work, n, cmp, mov, elapsed);
        cout << "Worst case  : n=" << n << ", time=" << elapsed << " ms, cmp=" << cmp << ", mov=" << mov << ", total=" << (cmp+mov) << endl;
        delete[] work;

        delete[] randArr;
        delete[] ascArr;
        delete[] descArr;
    }
}

// ================== Main function ==================

int main() {
    runDeletionTests();   // task 1
    runSortingTests();    // tasks 2-4
    return 0;
}*/