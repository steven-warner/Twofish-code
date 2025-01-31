/*
    This program encrypts some text using twofish

    DISCLAIMER: This is an open source algorithm made by Bruce Schneider I have made an implementation of the algorithm link below

    https://en.wikipedia.org/wiki/Twofish

    Date: 1/24/25

    Author: Steven Warner
*/

#include <iostream>
#include <random>
#include <String>
#include <iomanip>
#include <ctime>
#include <cstdlib>
using namespace std;

// public members
unsigned long long int key1 = 0;
unsigned long long int key2 = 0;
unsigned long long int key3 = 0;
unsigned long long int key4 = 0;
const int sBoxLength = 256;
unsigned long long int sBox[sBoxLength];
unsigned long long int inverseBox[sBoxLength];

const int blockSize = 4;
const int arrayLength = 8;
const int spaceValue = 255;
const int rounds = 16;
const int maxLength = 40;

// swaps bytes
void swapBlocks(unsigned long long int &leftBlock, unsigned long long int &rightBlock){
    // variable creation
    unsigned long long int temp = 0;
    // swaps blocks
    temp = leftBlock;
    leftBlock = rightBlock;
    rightBlock = temp;
}

// converts data to blocks
void convertToBlocks(string text, unsigned long long int blockArray[]){
    // variable creation
    int blockIndex = 0;
    int lettersInBlocks = 0;


    // adds data to block
    for (int i = 0;i < text.length();i++){
        // adds value to block
        if (text[i] != ' '){
            blockArray[blockIndex] |= (int)text[i];
        }
        else {
            blockArray[blockIndex] |= spaceValue;
        }

        // shifts to next byte space
        blockArray[blockIndex] <<= 8;

        // records letters in block
        lettersInBlocks += 1;

        // adds block to array
        if (lettersInBlocks == blockSize){
            blockIndex += 1;
            lettersInBlocks = 0;
        }

    }
}
// converts blocks to strings
string convertBlockToString(unsigned long long int blockArray[]){
    // variable creation
    string blockText = "";
    string formatedText = "";
    int currentByte = 0;
    unsigned long long int blockArrayCopy[arrayLength];

    // makes copy of array
    for (int i = 0;i < arrayLength;i++){
        blockArrayCopy[i] = blockArray[i];
    }

    // converts each index to string
    for (int i = 0;i < arrayLength;i++){
        // goes through each byte to get each letter
        for (int j = 0;j <= blockSize;j++){
            // gets first byte
            currentByte = blockArrayCopy[i] & 0xff;
            currentByte <<= 64; // goes over 8 bytes

            // adds value to string places it backwards because the bytes are loaded in backwards
            if (currentByte != spaceValue){
                blockText = (char)currentByte + blockText;
            }
            else{
                blockText = " " + blockText;
            }

            //cout << hex << currentByte << " " << blockArray[i]<< endl;

            // slides data to the right
            blockArrayCopy[i] >>= 8;
        }

        // adds to formated text
        formatedText += blockText;

        // erases original block text
        blockText = "";
    }
    // returns string
    return formatedText;
}



// rolls array to side
void roll(unsigned long long int inputArray[], int length){
    // variable creation
    int newArray[length];
    int counter = 1;

    // rolls array
    for (int i = 0;i < length;i++){
        // goes back to beginning
        if (counter == length){
            counter = 0;
        }

        // goes to next value
        newArray[counter] = inputArray[i];
        counter += 1;
    }

    // sets values to new array
    for (int i = 0;i < length;i++){
        inputArray[i] = newArray[i];
    }

}

// sha hash algorithm
unsigned long long int sha2(string input, string salt){
    // variable creation
    string pepper = "this is a pepper";
    string hashText = input + salt + pepper;
    int rounds = 32;
    int hashLength = 8;
    unsigned long long int hashArray[hashLength];
    unsigned long long int fullHash = 0;

    // resets hash array
    for (int i = 0;i < hashLength;i++){
        hashArray[i] = 0;
    }

    // adds values to array
    for (int i = 0;i < hashText.length();i++){
        hashArray[i % (hashLength - 1)] = hashArray[i % (hashLength - 1)] ^ (int)(hashText[i] + 5); // the plus 5 is to help make sure its a non 0 added
    }

    // makes hash
    for (int i = 0;i < rounds;i++){
        // makes calculations
        int ch = hashArray[4] + hashArray[5] + hashArray[6];
        int sigma1 = hashArray[4];
        int ma = hashArray[0] + hashArray[1] + hashArray[2];
        int sigma0 = hashArray[0];

        // adds values to first section
        hashArray[0] = hashArray[7] + ch + sigma1 + ma + sigma0;
        hashArray[3] = ch + sigma1;

        // rolls array
        roll(hashArray, hashLength);
    }

    // makes full hash
    for (int i = 0;i < hashLength;i++){
        //cout << hex << fullHash << endl;
        fullHash <<= 8;
        fullHash ^= hashArray[i];

    }

    // returns hash
    return fullHash;
}

// makes keys warning srand must be called prior
void makeKey(unsigned long long int &key){
    // variable creation
    unsigned long long int randomValue = 0;
    string inputTextForHash = "";

    // makes random value
    randomValue = rand() % 0xffffffffffffffffffff;

    // mixes random value
    for (int i = 0;i < 1000;i++){
        //inputTextForHash += (char)rand();
        randomValue <<= 8;
        randomValue ^= rand();
    }

    // sets key
    key = randomValue ^ sha2(inputTextForHash,"");
}

// prints array
void printArray(unsigned long long int arr[], int length){
    // prints array
    for (int i = 0;i < length;i++){
        cout << arr[i] << " ";
    }

    // goes to next line
    cout << endl;
}

// makes clear array
void makeClearArray(unsigned long long int arr[], int length){
    // makes array
    for (int i = 0;i < length;i++){
        arr[i] = 0;
    }
}

// makes sbox
void makeSBox(unsigned long long int leftKey, unsigned long long int rightKey){
    // variable creation
    unsigned long long int length = 256;
    unsigned long long int valueArray[length];
    int valuesInArray = 0;
    unsigned int currentValue = 0;
    int currentIndex = 0;
    unsigned long long int input = 0;
    unsigned long long int output = 0;
    unsigned int temp = 0;
    // makes value array
    makeClearArray(valueArray, length);

    // sets initial value
    valueArray[0] = leftKey ^ rightKey + 5;

    // sets s boxes to linear numbers
    for (int i = 0;i < sBoxLength;i++){
        sBox[i] = i;
        inverseBox[i] = i;
    }

    // gets key values for sbox
    for (int i = 1;i < length;i++){
        // randomly adds values based on conditions
        if (i % 23 == 0){
            valueArray[i] = leftKey ^ rightKey ^ valueArray[i - 1];
        }
        else if (i % 10 == 0){
            valueArray[i] = valueArray[i] * rightKey + i % length;
        }
        else{
            valueArray[i] = valueArray[i - 1] * leftKey + i;
        }
    }

    // adds s box values
    for (int i = 0;i < length - 1;i++){
        // goes through each byte to add to the s box
        do {
            // gets current value
            currentValue = valueArray[i] & 0xff;


            // swaps values
            temp = sBox[currentValue % length];
            sBox[currentValue % length] = sBox[i];
            sBox[i] = temp;


            // goes to next byte
            valueArray[i] >>= 8;

        } while (valueArray[i] > 0);

        // adds value back to next value
        valueArray[i + 1] ^= valueArray[i];
    }

    // helps make sure they are correct
    for (int i = 0;i < sBoxLength;i++){
        // fixes values
        // gets input and output
        output = sBox[i];
        input = i;

        // makes inverse
        inverseBox[output] = input;
    }
}

// s box replacement
void sBoxReplace(unsigned long long int& value, unsigned long long int replacementArray[]){
    // variable creation
    unsigned long long int currentValue = 0;
    unsigned long long int swapValue = 0;
    unsigned long long int fullByte = 0xffffffffffffffff;
    unsigned long long int counter = 0;
    unsigned long long int tempValue = fullByte;
    unsigned long long int subValue = 0;

    // replaces all bytes
    while (tempValue > 0){
        // gets current value for byte position
        currentValue = (value >> (counter * 8)) & 0xff;

        // makes swap value
        swapValue = replacementArray[currentValue] << (counter * 8);

        // replaces current byte
        subValue |= swapValue;
        //cout << swapValue << endl;

        // goes to next byte
        tempValue >>= 8;
        counter += 1;
    }

    // swaps sub value to main value
    value = subValue;
}

// encrypt
void encrypt(unsigned long long int blockArray[],unsigned long long int leftKeyInput, unsigned long long int rightKeyInput, unsigned long long int leftKeyOutput,unsigned long long int rightKeyOutput){
    // variable creation
    unsigned long long int subValue = 0;
    string subValueAsString = "";
    unsigned long long int tempValue = leftKeyInput;
    int currentValue = 0;

    // makes tempValue
    tempValue ^= rightKeyInput;
    tempValue ^= leftKeyInput;
    tempValue ^= rightKeyOutput;
    tempValue ^= leftKeyOutput;

    // converts left key to substitution value as a string
    while (tempValue > 0){
        // adds value to string
        currentValue = tempValue & 0xff;
        subValueAsString += (char)currentValue;

        // goes to next byte
        tempValue >>= 8;
    }

    // computes sub value
    subValue = sha2(subValueAsString, "");

    //cout << subValueAsString << "|" << endl;
    //cout << subValue << endl;

    // input whitening
    for (int i = 0;i < arrayLength;i++){
        // whitens right
        if (i % 2 != 0){
            blockArray[i] ^= rightKeyInput;
        }
        // whitens left
        else{
            blockArray[i] ^= leftKeyInput;
        }
    }

    // substitutes bytes
    for (int i = 0;i < arrayLength;i++){
        blockArray[i] ^= subValue;
        sBoxReplace(blockArray[i], sBox);
    }

    // preforms key rounds substitution
    for (int i = 0;i < arrayLength;i++){
        for (int j = 0;j < rounds;j++){
            // swaps blocks
            swapBlocks(blockArray[i],blockArray[i + 1]);

            // substitutes blocks
            blockArray[i] ^= leftKeyInput;
            blockArray[i + 1] ^= rightKeyInput;
        }
    }

    // output whitening
    for (int i = 0;i < arrayLength;i++){
        // whitens right
        if (i % 2 != 0){
            blockArray[i] ^= rightKeyOutput;
        }
        // whitens left
        else{
            blockArray[i] ^= leftKeyOutput;
        }
    }

    // counter mode for block chaining
    for (int i = 0;i < arrayLength;i++){
        blockArray[i] ^= i;
    }
}

// decrypt
void decrypt(unsigned long long int blockArray[],unsigned long long int leftKeyInput, unsigned long long int rightKeyInput, unsigned long long int leftKeyOutput,unsigned long long int rightKeyOutput){
    // variable creation
    unsigned long long int subValue = 0;
    string subValueAsString = "";
    unsigned long long int tempValue = leftKeyInput;
    int currentValue = 0;

    // makes tempValue
    tempValue ^= rightKeyInput;
    tempValue ^= leftKeyInput;
    tempValue ^= rightKeyOutput;
    tempValue ^= leftKeyOutput;

    // converts left key to substitution value as a string
    while (tempValue > 0){
        // adds value to string
        currentValue = tempValue & 0xff;
        subValueAsString += (char)currentValue;

        // goes to next byte
        tempValue >>= 8;
    }

    // computes sub value
    subValue = sha2(subValueAsString, "");

    //cout << subValueAsString << "|" << endl;
    //cout << subValue << endl;

    // counter based chaining
    for (int i = 0;i < arrayLength;i++){
        blockArray[i] ^= i;
    }

    // output whitening
    for (int i = arrayLength;i >= 0;i--){
        // whitens right
        if (i % 2 != 0){
            blockArray[i] ^= rightKeyOutput;
        }
        // whitens left
        else{
            blockArray[i] ^= leftKeyOutput;
        }
    }


    // preforms key rounds substitution
    for (int i = arrayLength;i >= 0;i--){
        for (int j = 0;j < rounds;j++){
            // swaps blocks
            swapBlocks(blockArray[i],blockArray[i + 1]);

            // substitutes blocks
            blockArray[i] ^= leftKeyInput;
            blockArray[i + 1] ^= rightKeyInput;
        }
    }

    // substitutes bytes
    for (int i = arrayLength;i >= 0;i--){
        sBoxReplace(blockArray[i], inverseBox);
        blockArray[i] ^= subValue;
    }

    // input whitening
    for (int i = arrayLength;i >= 0;i--){
        // whitens right
        if (i % 2 != 0){
            blockArray[i] ^= rightKeyInput;
        }
        // whitens left
        else{
            blockArray[i] ^= leftKeyInput;
        }
    }
}

// runs system check to make sure everything works well
void runDiagnostic(){
    // variable creation
    bool goodEncryption = true;
    bool goodSbox = true;
    unsigned long long int arr[arrayLength];

    // checks s-boxes
    for (int i = 0;i < sBoxLength;i++){
        if (inverseBox[sBox[i]] != i){
            goodSbox = false;
            break;
        }
    }

    // makes array
    for (int i = 0;i < arrayLength;i++){
        arr[i] = i;
    }

    // encrypts and decrypts data
    encrypt(arr, key1, key2, key3, key4);
    decrypt(arr, key1, key2, key3, key4);

    // checks encryption
    for (int i = 0;i < arrayLength;i++){
        if (arr[i] != i){
            goodEncryption = false;
            break;
        }
    }

    // prints the validity
    cout << "Diagnostics: " << endl;
    cout << "\tEncryption works(should be 1): " << goodEncryption << endl;
    cout << "\tS-box works(should be 1): " << goodSbox << endl;

}

// copies array to another array

int main()
{
    // sets up random generator
    srand(time(0));

    // opening message
    cout << "Twofish cipher (Key size 256)" << endl;
    // variable creation
    unsigned long long int blockArray[arrayLength];
    makeKey(key1);
    makeKey(key2);
    makeKey(key3);
    makeKey(key4);
    string input = "";
    makeClearArray(blockArray, arrayLength);
    makeClearArray(sBox,sBoxLength);
    makeClearArray(inverseBox,sBoxLength);

    // makes s-boxes
    makeSBox(key1, key2);

    // runs test to make sure everything works
    runDiagnostic();
    cout << endl;

    // gets user input
    do{
        // gets input
        cout << "Please enter data to be encrypted: ";
        getline(cin, input);
        //cout << input << endl;
        cout << endl;

        // prints warning message
        if (input.length() >= maxLength){
            cout << "Please enter something shorter" << endl;
        }

    } while(input.length() >= maxLength);


    // encrypts and decrypts data
    convertToBlocks(input, blockArray);
    cout << "Encrypted text: " << endl;
    encrypt(blockArray, key1, key2, key3, key4);
    cout << convertBlockToString(blockArray) << endl << endl;

    cout << "Decrypted text: " << endl;
    decrypt(blockArray, key1, key2, key3, key4);
    cout << convertBlockToString(blockArray) << endl << endl;


    // outputs keys
    cout << "Key: " << hex << key1 << key2 << key3 << key4 << endl << endl;

    // outputs vulnerabilities
    cout << "Warning not for cryptographic use only for educational use only:" << endl;

    cout << "____________________________________________________________________________________________________________________" << endl;
    cout << "Remember keys should always be sent and stored securely this is a demo and only demonstrates twofish as an algorithm" << endl;
    cout << "____________________________________________________________________________________________________________________" << endl;

    // credits
    cout << "Programming by Steven Warner" << endl;
    cout << "Original design by Bruce Schneider" << endl;
    return 0;
}
