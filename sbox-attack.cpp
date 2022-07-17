/**
 * @author Noah Cussatti
 * @file sbox-attack.cpp
 */

#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <time.h>
#include <random>
#include <algorithm>

using namespace std;

/**
 * @brief Gets the intersection of two given vectors
 * @param v1    the first vector
 * @param v2    the second vector
 * @return  A vector that is the intersection between v1 and v2
 */
vector<int> intersection(vector<int> v1, vector<int> v2)
{
    vector<int> result;

    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());
    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(result));

    return result;
}

/**
 * @brief given the 2-bit string, returns the index of the given string
 * @param str   thr 2-bit string
 * @return an integer corresponding to an index
 */
int sboxPos(string str)
{
    if (str == "00")
        return 0;
    else if (str == "01")
        return 1;
    else if (str == "10")
        return 2;
    else if (str == "11")
        return 3;

    cerr << "ERROR: incorrect given input in \"sboxPos\"" << endl;
    return -1;
}

/**
 * @brief the S function
 * @param s the sbox
 * @param x the inputted param
 * @return the output of the sbox given x
 */
int S(int *s, int x)
{
    string bits = bitset<4>(x).to_string();
    int col = sboxPos(string(1, bits[1]) + bits[2]);
    int row = sboxPos(string(1, bits[0]) + bits[3]);
    return s[4 * row + col];
}

/**
 * @brief Creates a distribution table given an sbox
 * @param sbox  the substition box
 * @param in    input length of the sbox
 * @param out   output length of the sbox
 * @return The distribution table
 */
int **distributionTable(int *sbox, int in, int out)
{
    int sizeIn = pow(2, in);
    int sizeOut = pow(2, out);

    // Creating distribution table
    int **table = 0;
    table = new int *[sizeIn];

    // Filling up the table with 0s
    for (int i = 0; i < sizeIn; i++)
    {
        table[i] = new int[sizeOut];
        for (int j = 0; j < sizeOut; j++)
            table[i][j] = 0;
    }

    // Filling out distribution table
    for (int i = 0; i < sizeIn; i++)
        for (int j = 0; j < sizeIn; j++)
            table[i ^ j][S(sbox, i) ^ S(sbox, j)]++;

    return table;
}

/**
 * @brief Prints a differential distribution table
 * @param sbox  the substition box
 * @param in    input length of the sbox
 * @param out   output length of the sbox
 */
void dtPrint(int **table, int in, int out)
{
    int sizeIn = pow(2, in);
    int sizeOut = pow(2, out);
    cout << right << setw(3) << "     0    1    2    3" << endl;
    cout << "----------------------------" << endl;
    for (int i = 0; i < sizeIn; i++)
    {
        cout << right << setw(3) << hex << uppercase << i << dec << "  | ";
        for (int j = 0; j < sizeOut; j++)
            cout << right << setw(3) << table[i][j] << "  ";
        cout << endl;
    }
    cout << endl
         << endl;
}

/**
 * @brief Attacks an sbox using a random x1 and x2 values
 * @param sbox  The sbox to attack
 */
void attack(int *sbox)
{
    // RNG
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> dist(0, 15);

    // Getting random arbitrary key
    int key = dist(rng);

    bool done = false;
    int round = 1;
    vector<int> prevKeys;

    // Loop will stop when 2 or less possible keys are found
    while (!done)
    {
        cout << "---- Round " << round << " ----" << endl;

        // Randomizing x1 and x2 for new round
        int x1 = dist(rng);
        int x2 = dist(rng);

        // printing x1 and x2
        cout << hex << uppercase << "x1: " << x1 << "  ;  x2: " << x2 << endl
             << endl;

        int xorX = x1 ^ x2;
        int xorX1 = x1 ^ key;
        int xorX2 = x2 ^ key;
        int y1 = S(sbox, xorX1);
        int y2 = S(sbox, xorX2);
        int xorY = y1 ^ y2;

        vector<vector<int>> inputValues;
        for (int i = 0; i < 4; i++)
            inputValues.push_back(vector<int>());

        // Adding the possible inputs from input XOR into inputValues
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
                if (((i ^ j) == xorX))
                    inputValues[(S(sbox, i) ^ S(sbox, j))].push_back(i);

        // Printing out possible input values for sbox
        for (int i = 0; i < 4; i++)
        {
            if (!inputValues[i].empty())
            {
                if (xorY == i)
                    cout << "* ";
                else
                    cout << "  ";
                cout << xorX << " --> " << i << ":  ";
                for (int j = 0; j < inputValues[i].size() - 1; j++)
                    cout << inputValues[i][j] << ", ";

                cout << inputValues[i][inputValues[i].size() - 1] << endl;
            }
        }
        cout << endl;

        // Creating the current possible Keys vector
        vector<int> currentKeys;
        for (int i = 0; i < inputValues[xorY].size(); i++)
        {
            int pKey = inputValues[xorY][i] ^ x1;
            currentKeys.push_back(pKey);
        }

        // If this is the first round
        if (prevKeys.empty())
            for (int i = 0; i < currentKeys.size(); i++)
                prevKeys.push_back(currentKeys[i]);

        else
        {
            // Getting the intersection of the previous keys and current keys
            vector<int> temp = intersection(currentKeys, prevKeys);

            // Checking if we have enough keys and can exit loop
            if (temp.size() <= 2)
                done = true;

            // Putting temp into prevKeys
            prevKeys.clear();
            for (int i = 0; i < temp.size(); i++)
                prevKeys.push_back(temp[i]);
        }

        // Printing the possible keys
        cout << "Possible keys are: " << endl
             << "{ ";
        for (int i = 0; i < prevKeys.size() - 1; i++)
            cout << prevKeys[i] << ", ";
        cout << prevKeys[prevKeys.size() - 1] << " }" << endl
             << endl
             << endl;

        // Incrementing rounds
        round++;
    }
}

int main()
{
    // sboxes from hw1a
    int s0[16] = {1, 0, 3, 2, 3, 2, 1, 0, 0, 2, 1, 3, 3, 1, 3, 2};
    int s1[16] = {0, 1, 2, 3, 2, 0, 1, 3, 3, 0, 1, 0, 2, 1, 0, 3};

    // Creating the distribution tables for both sboxes
    int **dt0 = distributionTable(s0, 4, 2);
    int **dt1 = distributionTable(s1, 4, 2);

    // Command line code
    string input = "";
    bool done = false;

    cout << "============ COMMANDS ============" << endl;
    cout << "d0 : Prints the distribution table for S-Box 0." << endl;
    cout << "d1 : Prints the distribution table for S-Box 1." << endl;
    cout << "a0 : Shows S-Box 0 attack and the possible keys." << endl;
    cout << "a1 : Shows S-Box 1 attack and the possible keys." << endl;
    cout << " h : Prints the commands." << endl;
    cout << " q : Closes the program." << endl;
    cout << "==================================" << endl
         << endl;

    while (!done)
    {
        cout << "Input command ('q' to quit):  ";
        cin >> input;

        if (input == "h")
        {
            cout << endl
                 << "============ COMMANDS ============" << endl;
            cout << "d0 : Prints the distribution table for S-Box 0." << endl;
            cout << "d1 : Prints the distribution table for S-Box 1." << endl;
            cout << "a0 : Shows S-Box 0 attack and the possible keys." << endl;
            cout << "a1 : Shows S-Box 1 attack and the possible keys." << endl;
            cout << " h : Prints the commands." << endl;
            cout << " q : Closes the program." << endl;
            cout << "==================================" << endl
                 << endl;
        }
        else if (input == "d0")
        {
            cout << endl
                 << "  s0";
            dtPrint(dt0, 4, 2);
        }
        else if (input == "d1")
        {
            cout << endl
                 << "  s1";
            dtPrint(dt1, 4, 2);
        }
        else if (input == "a0")
        {
            cout << endl;
            attack(s0);
        }
        else if (input == "a1")
        {
            cout << endl;
            attack(s1);
        }
        else if (input == "q")
            done = true;
        else
            cout << endl
                 << "Invalid input, please try again. (input 'h' for help)" << endl
                 << endl;
    }

    return EXIT_SUCCESS;
}