
#include<bits/stdc++.h>
#include <fstream>
#include <stdexcept>
using namespace std;

const int AND8 = 255;
const int INT_SZ_IN_CHAR = 4;
const int SZ_CHAR = 8*sizeof(char);


int toInt(char ch)
{
    if(ch == '0'){
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

    HuffmanTreeNode(char inputChar, bool isEOF, int frequency){
        character = inputChar;
        this->isEOF = isEOF;
        this->frequency = frequency;
        isChar = !isEOF;
    }

    HuffmanTreeNode(int frequency){
        this->frequency = frequency;
        isChar = false;
        isEOF = false;
    }

    void makeLeftChild(HuffmanTreeNode *child){
        this->leftChild = child;
        return;
    }

    void makeRightChild(HuffmanTreeNode *child){
        this->rightChild = child;
        return;
    }

    void generateCharacterCodes(string& characterCode, map<char,string>& characterCodeMap, string& eofCode){

        if(isChar){
            characterCodeMap[character] = characterCode;
            return;
        }

        if(isEOF){
            eofCode = characterCode;
            return;
        }

        if(leftChild != nullptr){
            characterCode.push_back('0');
            (*leftChild).generateCharacterCodes(characterCode, characterCodeMap, eofCode);
            characterCode.pop_back();
        }

        if(rightChild != nullptr){
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

    HuffmanNodePointer(HuffmanTreeNode* nodePointer)
    {
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


void createFrequencyMap(map<char,int>& frequencyMap, fstream& SourceFile)
{
    char ch;
    while(SourceFile >> noskipws >> ch)
    {
        frequencyMap[ch]++;
    }
    return;
}

void breakInteger(int n, char chArr[])
{
    
    for(int i = 0; i < INT_SZ_IN_CHAR; i++)
    {
        chArr[i] = ((n >> ((INT_SZ_IN_CHAR - i - 1) * SZ_CHAR)) & AND8);
    }

    return;
}

void createHeaderFile(map<char,int>& frequencyMap, fstream& HeaderFile)
{

    char chArr[INT_SZ_IN_CHAR];

    for(auto it: frequencyMap)
    {
        HeaderFile << it.first;
        breakInteger(it.second, chArr);
        for(int i = 0; i < INT_SZ_IN_CHAR; i++){
            HeaderFile << chArr[i];
        }
    }
    
    return;
}

void createBodyFile(map<char,string>& characterCodeMap, string eofCode, fstream& SourceFile, fstream& BodyFile)
{

    char ch;
    queue<char> outputQueue;

    while(SourceFile >> noskipws >> ch)
    {
        for(auto it: characterCodeMap[ch]){
            outputQueue.push(it);
        }
    }
    for(auto it: eofCode)
    {
        outputQueue.push(it);
    }
    
    while(!outputQueue.empty())
    {
        char opchar = 0;
        int val = 0;
        for(int i = 7; i >= 0; i--){
            if(!outputQueue.empty()){
                val = toInt(outputQueue.front());
                outputQueue.pop();
            }            
            opchar |= val<<i;
        }
        BodyFile << opchar;
        
    }
    
    return;
}

int main()
{

    string sourceFilename, headerFilename, bodyFilename;
    fstream SourceFile, HeaderFile, BodyFile;
    cout<<"Enter source file name\n";
    cin>>sourceFilename;
    cout<<"Enter header file name\n";
    cin>>headerFilename;
    headerFilename = headerFilename + ".huffh";
    cout<<"Enter body file name\n";
    cin>>bodyFilename;
    bodyFilename = bodyFilename + ".huffb";

    try
    {
        SourceFile.open(sourceFilename, ios::in | ios::binary);
        HeaderFile.open(headerFilename, ios::trunc | ios::out | ios::binary);
        BodyFile.open(bodyFilename, ios::trunc | ios::out | ios::binary);
    }
    catch(...){
        cerr << "Error opening files\n";
        exit(1);
    }

    map<char,int> frequencyMap;
    map<char,string> characterCodeMap;
    string eofCode;
    string characterCode = "";
    
    createFrequencyMap(frequencyMap, SourceFile);
    SourceFile.clear();
    SourceFile.seekg(0);

    HuffmanTreeNode* root = buildHuffmanTree(frequencyMap);

    root->generateCharacterCodes(characterCode, characterCodeMap, eofCode);

    createHeaderFile(frequencyMap, HeaderFile);
    createBodyFile(characterCodeMap, eofCode, SourceFile, BodyFile);

    SourceFile.close();
    HeaderFile.close();
    BodyFile.close();
    return 0;
}