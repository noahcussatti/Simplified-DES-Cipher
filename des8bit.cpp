/**
 * @author Noah Cussatti
 * @date CSCI 4230, Section 01, Summer 2022
 * @file des8bit.cpp
 */

#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <iomanip>
#include <bitset>

using namespace std;

/**
 * @brief permutes the given string using given permutation table
 * @param str   the string being permuted
 * @param perm  the permutation table
 * @param size  the size of permutation table
 * @return a permuted string
 */
string permute(string str, int *perm, int size)
{
    string result = "";
    for (int i = 0; i < size; i++)
        result += str[perm[i] - 1];

    return result;
}

/**
 * @brief shifts the given str
 * @param str       the string being shifted
 * @param shifts    The length being shifted
 * @return a shifted version of str
 */
string shiftLeft(string str, int shifts)
{
    while (shifts > 0)
    {
        char first = str[0];
        for (int i = 0; i < str.size(); i++)
            str[i] = str[i + 1];

        str[str.size() - 1] = first;
        shifts--;
    }
    return str;
}

/**
 * @brief performs an XOR operation on the given bit strings
 * @param str1  first bit string
 * @param str   second bit string
 * @return a bit string that XOR'd through every bit of both bit strings
 */
string xorFunction(string str1, string str2)
{
    string result = "";
    for (int i = 0; i < str1.size(); i++)
    {
        if (str1[i] == str2[i])
            result += "0";
        else
            result += "1";
    }

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
 * @brief performs the F function
 * @param input the 4-bit input string
 * @param key   the 8-bit key used in the xor
 * @return a permuted 4-bit string
 */
string functionF(string input, string key)
{
    int expandperm8[8] = {4, 1, 2, 3, 2, 3, 4, 1};
    int perm4[4] = {2, 4, 3, 1};

    string s0[4][4] = {{"01", "00", "11", "10"},
                       {"11", "10", "01", "00"},
                       {"00", "10", "01", "11"},
                       {"11", "01", "11", "10"}};

    string s1[4][4] = {{"00", "01", "10", "11"},
                       {"10", "00", "01", "11"},
                       {"11", "00", "01", "00"},
                       {"10", "01", "00", "11"}};

    // expand/permute input bits into 8bits
    string perm = permute(input, expandperm8, 8);

    // xor the expanded input w/ the key
    string xorStr = xorFunction(perm, key);

    int col0 = sboxPos(string(1, xorStr[1]) + xorStr[2]);
    int row0 = sboxPos(string(1, xorStr[0]) + xorStr[3]);
    string s0Str = s0[row0][col0];

    int col1 = sboxPos(string(1, xorStr[5]) + xorStr[6]);
    int row1 = sboxPos(string(1, xorStr[4]) + xorStr[7]);
    string s1Str = s1[row1][col1];

    return permute((s0Str + s1Str), perm4, 4);
}

/**
 * @brief creates a vector fo keys given a key using permutation tables
 * @param key   the input key
 * @return a vector of 2 keys
 */
vector<string> createKeys(string key)
{
    vector<string> result;

    int perm10[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
    int perm8[8] = {6, 3, 7, 4, 8, 5, 10, 9};

    string perm = permute(key, perm10, 10);

    string lBits = shiftLeft(perm.substr(0, 5), 1);
    string rBits = shiftLeft(perm.substr(5, 5), 1);
    result.push_back(permute((lBits + rBits), perm8, 8));

    string lBits2 = shiftLeft(perm.substr(0, 5), 3);
    string rBits2 = shiftLeft(perm.substr(5, 5), 3);
    perm = permute((lBits2 + rBits2), perm8, 8);
    result.push_back(perm);

    return result;
}

/**
 * @brief main encryption algorithm
 * @param plaintext the inputted plaintext
 * @param key       the inputted key
 * @return the ciphertext of the plaintext
 */
string encrypt(string plaintext, string key)
{
    // Creating variables
    vector<string> keys = createKeys(key);
    int initialPerm[8] = {2, 6, 3, 1, 4, 8, 5, 7};
    int inversePerm[8] = {4, 1, 3, 5, 7, 2, 8, 6};

    // First permutation
    string perm = permute(plaintext, initialPerm, 8);

    // First run through
    string lBits = perm.substr(0, 4);
    string rBits = perm.substr(4, 4);
    string xorStr = xorFunction(lBits, functionF(rBits, keys[0]));

    // Second run through
    lBits = rBits;
    rBits = xorStr;
    xorStr = xorFunction(lBits, functionF(rBits, keys[1]));
    perm = xorStr + rBits;

    // Inverse initial perm.
    return permute(perm, inversePerm, 8);
}

/**
 * @brief main decryption algorithm
 * @param ciphertext the inputted ciphertext
 * @param key       the inputted key
 * @return the decrypted text
 */
string decrypt(string ciphertext, string key)
{
    // Creating variables
    vector<string> keys = createKeys(key);
    int initialPerm[8] = {2, 6, 3, 1, 4, 8, 5, 7};
    int inversePerm[8] = {4, 1, 3, 5, 7, 2, 8, 6};

    // First permutation
    string perm = permute(ciphertext, initialPerm, 8);

    // First run through
    string lBits = perm.substr(0, 4);
    string rBits = perm.substr(4, 4);
    string xorStr = xorFunction(lBits, functionF(rBits, keys[1]));

    // Second run through
    lBits = rBits;
    rBits = xorStr;
    xorStr = xorFunction(lBits, functionF(rBits, keys[0]));
    perm = xorStr + rBits;

    // Inverse initial perm.
    return permute(perm, inversePerm, 8);
}

/* Testing in main */
int main()
{
    /* TEST 1 */
    string plaintext = "00101000";
    string key = "1100011110";
    string ciphertext = "10001010";

    // Printing out known data
    cout << "Test 1" << endl;
    cout << "--------------------------" << endl;
    cout << left << setw(12) << "Key:" << key << endl;
    cout << left << setw(12) << "Plaintext:" << plaintext << endl;
    cout << "--------------------------" << endl;

    // Encrypting plaintext
    string result1 = encrypt(plaintext, key);

    // Checking if encrypted equals ciphertext
    cout << left << setw(12) << "Encrypted:" << result1 << endl;
    if (result1 == ciphertext)
        cout << "Encryption matched Ciphertext, SUCCESS!" << endl;
    else
        cout << "Encryption did not match Ciphertext, FAILED" << endl;

    // Decrypting ciphertext
    string result2 = decrypt(result1, key);

    // Checking if decrypted equals plaintext
    cout << left << setw(12) << "Decrypted:" << result2 << endl;
    if (result2 == plaintext)
        cout << "Decryption matched Plaintext, SUCCESS!" << endl;
    else
        cout << "Decryption did not match Plaintext, FAILED" << endl;
    cout << "--------------------------" << endl
         << endl
         << endl;

    /* TEST 2 */
    string cryptoInput = "crypto";

    // Printing out known data
    cout << "Test 2: \"crypto\" plaintext" << endl;
    cout << "--------------------------" << endl;
    cout << left << setw(12) << "Key:" << key << endl;
    cout << left << setw(12) << "Plaintext:" << cryptoInput << endl;
    cout << left << setw(12) << "PT Binary:";

    // Converting and printing out plaintext to binary
    string plaintext2[6];
    for (int i = 0; i < cryptoInput.size(); i++)
    {
        plaintext2[i] = bitset<8>(cryptoInput[i]).to_string();
        cout << plaintext2[i] << " ";
    }
    cout << endl
         << "--------------------------" << endl;

    // Encrypting plaintext
    string ciphertext2 = "";
    string ciphertext2arr[6];
    cout << left << setw(12) << "CT Binary:";
    for (int i = 0; i < 6; i++)
    {
        ciphertext2arr[i] = bitset<8>(encrypt(plaintext2[i], key)).to_string();
        unsigned long bin = bitset<8>(encrypt(plaintext2[i], key)).to_ulong();
        ciphertext2 += static_cast<char>(bin);
        cout << ciphertext2arr[i] << " ";
    }
    cout << endl;
    cout << left << setw(12) << "Ciphertext:" << ciphertext2 << endl;

    // Decrypting ciphertext
    string decryptCrypto[6];
    string decryptStr = "";
    cout << left << setw(12) << "DC Binary:";
    for (int i = 0; i < 6; i++)
    {
        decryptCrypto[i] = bitset<8>(decrypt(ciphertext2arr[i], key)).to_string();
        unsigned long bin = bitset<8>(decrypt(ciphertext2arr[i], key)).to_ulong();
        decryptStr += static_cast<char>(bin);
        cout << decryptCrypto[i] << " ";
    }
    cout << endl;
    cout << left << setw(12) << "Decrypted:" << decryptStr << endl;
    if (decryptStr == cryptoInput)
        cout << "Decryption matched Plaintext, SUCCESS!" << endl;
    else
        cout << "Decryption did not match Plaintext, FAILED" << endl;
    cout << "--------------------------" << endl
         << endl
         << endl;

    /* USER INPUT */

    // Taking in user input
    string inputPT;
    string inputKey;
    cout << left << setw(17) << "Enter Plaintext: ";
    cin >> inputPT;
    cout << left << setw(17) << "Enter Key: ";
    cin >> inputKey;

    // checking if key's length is 10
    if (inputKey.size() != 10)
    {
        cerr << "ERROR: Key input length must be 10" << endl;
        return EXIT_FAILURE;
    }

    // Printing out known data
    cout << endl
         << "Testing user input..." << endl;
    cout << "--------------------------" << endl;
    cout << left << setw(12) << "Key:" << inputKey << endl;
    cout << left << setw(12) << "Plaintext:" << inputPT << endl;
    cout << left << setw(12) << "PT Binary:";

    // Converting and printing out plaintext to binary
    string inputPTArr[inputPT.size()];
    for (int i = 0; i < inputPT.size(); i++)
    {
        inputPTArr[i] = bitset<8>(inputPT[i]).to_string();
        cout << inputPTArr[i] << " ";
    }
    cout << endl
         << "--------------------------" << endl;

    // Encrypting plaintext
    string inputCipher = "";
    string inputCipherArr[inputPT.size()];
    cout << left << setw(12) << "CT Binary:";
    for (int i = 0; i < inputPT.size(); i++)
    {
        inputCipherArr[i] = bitset<8>(encrypt(inputPTArr[i], inputKey)).to_string();
        unsigned long bin = bitset<8>(encrypt(inputPTArr[i], inputKey)).to_ulong();
        inputCipher += static_cast<char>(bin);
        cout << inputCipherArr[i] << " ";
    }
    cout << endl;
    cout << left << setw(12) << "Ciphertext:" << inputCipher << endl;

    // Decrypting ciphertext
    string inputDecryptArr[inputPT.size()];
    string inputDecrypt = "";
    cout << left << setw(12) << "DC Binary:";
    for (int i = 0; i < inputPT.size(); i++)
    {
        inputDecryptArr[i] = bitset<8>(decrypt(inputCipherArr[i], inputKey)).to_string();
        unsigned long bin = bitset<8>(decrypt(inputCipherArr[i], inputKey)).to_ulong();
        inputDecrypt += static_cast<char>(bin);
        cout << inputDecryptArr[i] << " ";
    }
    cout << endl;
    cout << left << setw(12) << "Decrypted:" << inputDecrypt << endl;
    if (inputDecrypt == inputPT)
        cout << "Decryption matched Plaintext, SUCCESS!" << endl;
    else
        cout << "Decryption did not match Plaintext, FAILED" << endl;
    cout << "--------------------------" << endl;

    return EXIT_SUCCESS;
}