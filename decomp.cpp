#include<bits/stdc++.h>
#include <fstream>
#include <stdexcept>
using namespace std;

const int AND8 = 255;
const int INT_SZ_IN_CHAR = 4;
const int SZ_CHAR = 8*sizeof(char);


int toInt(char ch)
{
    if(ch == '0')
    {
        return 0;
    }
    return 1;
}

class HuffmanTreeNode
{
public:
    bool isEOF;
    bool isChar;
    char character;
    int frequency;
    HuffmanTreeNode *leftChild = nullptr;
    HuffmanTreeNode *rightChild = nullptr;

    HuffmanTreeNode(char inputChar, bool isEOF, int frequency)
    {
        character = inputChar;
        this->isEOF = isEOF;
        this->frequency = frequency;
        isChar = !isEOF;
    }

    HuffmanTreeNode(int frequency)
    {
        this->frequency = frequency;
        isChar = false;
        isEOF = false;
    }

    void makeLeftChild(HuffmanTreeNode *child)
    {
        this->leftChild = child;
        return;
    }

    void makeRightChild(HuffmanTreeNode *child)
    {
        this->rightChild = child;
        return;
    }

    void generateCharacterCodes(string& characterCode, map<char,string>& characterCodeMap, string& eofCode)
    {

        if(isChar)
        {
            characterCodeMap[character] = characterCode;
            return;
        }

        if(isEOF)
        {
            eofCode = characterCode;
            return;
        }

        if(leftChild != nullptr)
        {
            characterCode.push_back('0');
            (*leftChild).generateCharacterCodes(characterCode, characterCodeMap, eofCode);
            characterCode.pop_back();
        }

        if(rightChild != nullptr)
        {
            characterCode.push_back('1');
            (*rightChild).generateCharacterCodes(characterCode, characterCodeMap, eofCode);
            characterCode.pop_back();
        }

        return;
    }

};

class HuffmanNodePointer
{
    public:
    HuffmanTreeNode* nodePointer;

    HuffmanNodePointer(HuffmanTreeNode* nodePointer){
        this->nodePointer = nodePointer;
    }
};

bool operator>(const HuffmanNodePointer& node1, const HuffmanNodePointer& node2)
{
    return node1.nodePointer->frequency > node2.nodePointer->frequency;
}

HuffmanTreeNode* buildHuffmanTree(map<char,int>& frequencyMap)
{
    priority_queue<HuffmanNodePointer, vector<HuffmanNodePointer>, greater<HuffmanNodePointer>> pq;

    for(auto it: frequencyMap)
    {
        HuffmanTreeNode* newNode = new HuffmanTreeNode(it.first, false, it.second);
        HuffmanNodePointer newPointer = HuffmanNodePointer(newNode);
        pq.push(newPointer);                        
    }
    HuffmanTreeNode* newNode = new HuffmanTreeNode('a', true, 1);
    HuffmanNodePointer newPointer = HuffmanNodePointer(newNode);
    pq.push(newPointer);

    while(pq.size() > 1)
    {
        HuffmanNodePointer node1 = pq.top();
        pq.pop();
        HuffmanNodePointer node2 = pq.top();
        pq.pop();
        HuffmanTreeNode* newNode = new HuffmanTreeNode(node1.nodePointer->frequency + node2.nodePointer->frequency);
        (*newNode).makeLeftChild(node1.nodePointer);
        (*newNode).makeRightChild(node2.nodePointer);
        HuffmanNodePointer newPointer = HuffmanNodePointer(newNode);
        pq.push(newPointer);
    }

    return pq.top().nodePointer;
}

int makeInt(char chArr[])
{
    int result = 0;
    
    for(int i = 0; i < INT_SZ_IN_CHAR; i++){
        result |= ((chArr[i] & AND8) << ((INT_SZ_IN_CHAR - i - 1) * SZ_CHAR));
    }

    return result;
}

void createFrequencyMap(map<char,int>& frequencyMap, fstream& HeaderFile)
{
    char ch;
    char chArr[INT_SZ_IN_CHAR];

    while(HeaderFile >> noskipws >> ch){

        for(int i = 0; i < INT_SZ_IN_CHAR; i++)
        {
            try
            {
                HeaderFile >> chArr[i];
            }
            catch(...)
            {
                cerr<<"Error reading header file\n";
                exit(1);
            }
        }

        frequencyMap[ch] = makeInt(chArr);
    }

    return;
}

void getBinaryString(vector<int>& binaryString, fstream& BodyFile)
{
    char ch;
    
    while(BodyFile >> noskipws >> ch)
    {
        for(int i = 7; i >= 0; i--)
        {
            binaryString.push_back((ch>>i)&1);
        }
    }
    
    return;
}

void generateSourceFile(vector<int>& binaryString, fstream& SourceFile, HuffmanTreeNode* root)
{
    HuffmanTreeNode* ptr = root;
    char ch;
    
    if(root->isEOF)
    {
        return;
    }

    for(auto it: binaryString)
    {
        if(it == 1)
        {
            ptr = ptr->rightChild;
        }
        else
        {
            ptr = ptr->leftChild;
        }
        if(ptr->isEOF)
        {
            break;
        }
        if(ptr->isChar)
        {
            SourceFile << ptr->character;
            ptr = root;
        }
    }

    return;
}

int main(){

    string sourceFilename, headerFilename, bodyFilename;
    fstream SourceFile, HeaderFile, BodyFile;
    cout<<"Enter destination file name\n";
    cin>>sourceFilename;
    cout<<"Enter header file name(.huffh)\n";
    cin>>headerFilename;
    cout<<"Enter body file name(.huffb)\n";
    cin>>bodyFilename;

    try
    {
        SourceFile.open(sourceFilename, ios::trunc | ios::out | ios::binary);
        HeaderFile.open(headerFilename, ios::in | ios::binary);
        BodyFile.open(bodyFilename, ios::in | ios::binary);
    }
    catch(...)
    {
        cerr << "Error opening files\n";
        exit(1);
    }

    map<char,int> frequencyMap;
    vector<int> bodyFileBinaryString;

    try
    {
        createFrequencyMap(frequencyMap, HeaderFile);
    }
    catch(...)
    {
        cerr << "Error reading Header file: file may be corrupted\n";
        exit(1);
    }

    getBinaryString(bodyFileBinaryString, BodyFile);

    HuffmanTreeNode* root = buildHuffmanTree(frequencyMap);

    generateSourceFile(bodyFileBinaryString, SourceFile, root);

    SourceFile.close();
    HeaderFile.close();
    BodyFile.close();
    return 0;
}