#ifndef INVENTORY_COMPRESSOR_H
#define INVENTORY_COMPRESSOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <utility>
#include "inventory.h"
#include "hash/xMap.h"
#include "heap/Heap.h"
#include "list/XArrayList.h"
// ! thêm này vô để chạy test
namespace std {
    inline ostream& operator<<(ostream& os, const pair<char, int>& p) {
        os << "(" << p.first << ", " << p.second << ")";
        return os;
    }
};
template<int treeOrder>
class HuffmanTree {
public:
    struct HuffmanNode {
        char symbol;
        int freq;
        int order;
        XArrayList<HuffmanNode*> children;

        HuffmanNode(char s, int f) : symbol(s), freq(f) {}; //Leaf node
        HuffmanNode(char s = '\0', int f = 0, int o = 0) : symbol(s), freq(f), order(o) {}
        HuffmanNode(int f, const  XArrayList<HuffmanNode*>& childs){
            this->freq = f;
            this->symbol = '\0';
            for(int i = 0; i < childs.size(); i++){
                this->children.add(childs.get(i));
            }
        }; //Internal node
    };

    static int compareFreq(HuffmanNode*& a, HuffmanNode*& b) {
        if (a->freq != b->freq) {
            return a->freq - b->freq;  // Compare frequencies
        }
        // If frequencies are equal, prioritize real nodes over dummy nodes
        

        // cout << a->symbol << endl;

        if (a->symbol == '\0' && b->symbol != '\0') return 1;  // b comes first
        if (a->symbol != '\0' && b->symbol == '\0') return -1; // a comes first
    
        return 0;  // If both are real or both are dummy, keep the current order
    }

    HuffmanTree();
    ~HuffmanTree();
    void removeInternalHelper(HuffmanNode* node);

    char digitChar(int i) {
        if (i >= 0 && i <= 9) return '0' + i;
        return 'a' + (i - 10); // supports up to 36 (treeOrder <= 36)
    }

    int charToDigit(char c) {
        if (isdigit(c)) return c - '0';
        return c - 'a' + 10;
    }

    void build(XArrayList<pair<char, int>>& symbolsFreqs);
    void generateCodesHelper(HuffmanNode* node, string code, xMap<char, string>& table);
    void generateCodes(xMap<char, std::string>& table);
    void decodeHelper(HuffmanNode* node,int& index, const string& huffmanCode, string& result);
    std::string decode(const std::string& huffmanCode);

private:
    HuffmanNode* root;
};

template<int treeOrder>
class InventoryCompressor {
public:
    InventoryCompressor(InventoryManager* manager);
    ~InventoryCompressor();

    void buildFreqTable(xMap<char, int>& map, string& products);
    XArrayList<pair<char, int>> convertToPairs(xMap<char, int>& map);
    void buildHuffman();
    void printHuffmanTable();
    std::string productToString(const List1D<InventoryAttribute>& attributes, const std::string& name);
    void encodeHelper(const string& productInfo, xMap<char, string>& table, string& result);
    std::string encodeHuffman(const List1D<InventoryAttribute>& attributes, const std::string& name);
    std::string decodeHuffman(const std::string& huffmanCode, List1D<InventoryAttribute>& attributesOutput, std::string& nameOutput);

private:
    xMap<char, std::string>* huffmanTable;
    InventoryManager* invManager;
    HuffmanTree<treeOrder>* tree;
};


#endif // INVENTORY_COMPRESSOR_H

template <int treeOrder>
HuffmanTree<treeOrder>::HuffmanTree()
{
    this->root = nullptr;
}

template<int treeOrder>
void HuffmanTree<treeOrder>::removeInternalHelper(HuffmanNode* node){
    if(!node) return;

    for(int i = 0; i < node->children.size(); i++){
        removeInternalHelper(node->children.get(i));
    }

    delete node;
}

template<int treeOrder>
HuffmanTree<treeOrder>::~HuffmanTree()
{
    //TODO
    removeInternalHelper(this->root);
}

template <int treeOrder>
void HuffmanTree<treeOrder>::build(XArrayList<pair<char, int>>& symbolsFreqs) {
    
auto Nodescompare = [](HuffmanNode*& a, HuffmanNode*& b) -> int {
    if (a->freq < b->freq) return -1;
    if (a->freq > b->freq) return 1;

    // Prioritize real symbols over dummy '\0' characters
    bool aIsNull = (a->symbol == '\0');
    bool bIsNull = (b->symbol == '\0');
    if (aIsNull != bIsNull) {
        return aIsNull ? 1 : -1; // a is dummy -> a > b
    }

    if (a->order < b->order) return -1;
    if (a->order > b->order) return 1;

    return 0;
};
    if(symbolsFreqs.empty()) return;

    if(symbolsFreqs.size() == 1){
        for(int i = 0; i < treeOrder - 1; i++){
            symbolsFreqs.add({'\0', 0});
        }
    }
    else{
        int count = 0;
        while(((symbolsFreqs.size() - 1) % (treeOrder - 1)) != 0){
            symbolsFreqs.add({'\0', 0});
        }
    }
    
    XArrayList<pair<char,int>> holder(symbolsFreqs);


    int size = holder.size();

    bool swapped = 1;
    
    for(int i = 0; i < size - 1; i++){
        if(!swapped) break;
        swapped = 0;
        for(int j = 0; j < size - i - 1; j++){
            auto& temp1 = holder.get(j);
            auto& temp2 = holder.get(j+1);
            if(temp1.first > temp2.first){
                swap(temp1, temp2);
                swapped = 1;
            }
        }
    }
    symbolsFreqs = holder;

    Heap<HuffmanNode*> container(Nodescompare);
    
    for (int i = 0; i < symbolsFreqs.size(); i++) {
        container.push(new HuffmanNode(symbolsFreqs.get(i).first, symbolsFreqs.get(i).second, i));
    }

    
    ////

    int nextOrder = symbolsFreqs.size() ; // Start internal nodes with higher order
    
    // Build the tree by repeatedly taking treeOrder nodes with lowest frequencies
    while (container.size() > 1) {
        // Create a new internal node with next order value
        HuffmanNode* internalNode = new HuffmanNode('\0', 0, nextOrder++);
        int totalFreq = 0;
        
        // Extract exactly treeOrder nodes or all remaining nodes
        int nodesToExtract = (container.size() < treeOrder) ? container.size() : treeOrder;
        
        for (int i = 0; i < nodesToExtract; i++) {
            HuffmanNode* node = container.pop();
            internalNode->children.add(node);
            totalFreq += node->freq;
        }
        
        internalNode->freq = totalFreq;
        container.push(internalNode);
    }
    
    
    if (!container.empty()) {
        root = container.pop();
    }
}
template<int treeOrder>
void HuffmanTree<treeOrder>::generateCodesHelper(HuffmanNode* node, string code, xMap<char, string>& table){
    if(!node) return;

    if(node->children.size() == 0){
        table.put(node->symbol, code);
        return;
    }

    for(int i = 0; i < node->children.size(); i++){
        generateCodesHelper(node->children.get(i), code + digitChar(i), table);
    }
}

template <int treeOrder>
void HuffmanTree<treeOrder>::generateCodes(xMap<char, std::string> &table)
{
    //TODO
    generateCodesHelper(this->root, "", table);
}

template<int treeOrder>
void HuffmanTree<treeOrder>::decodeHelper(HuffmanNode* node,int& index, const string& huffmanCode, string& result){
    if(!node || index > huffmanCode.length()) return;


    if(node->children.size() == 0 && node->symbol != '\0'){

        result += node->symbol;
        return;
    }
    int branch = charToDigit(huffmanCode[index++]);

    if(branch >= node->children.size() || branch < 0) return;

    decodeHelper(node->children.get(branch), index, huffmanCode, result);
}

template <int treeOrder>
string HuffmanTree<treeOrder>::decode(const std::string &huffmanCode)
{
    //TODO
    string result = "";
    int index = 0;
    while(index < huffmanCode.length()){
        decodeHelper(this->root, index, huffmanCode, result);
    }
    if(result == "") return "\\x00"; 
    return result;
}

template <int treeOrder>
InventoryCompressor<treeOrder>::InventoryCompressor(InventoryManager *manager)
{
    //TODO
    this->invManager = manager;
    this->tree = new HuffmanTree<treeOrder>();
    this->huffmanTable = nullptr;
}

template <int treeOrder>
InventoryCompressor<treeOrder>::~InventoryCompressor()
{
    //TODO
    delete this->tree;
    if(huffmanTable != nullptr) delete this->huffmanTable;
}

template<int treeOrder>
void InventoryCompressor<treeOrder>::buildFreqTable(xMap<char, int>& map, string& products){
    for(char c : products){
        int count = 0;
        if(map.containsKey(c)) count = map.get(c);
        map.put(c, count + 1);
    }
}

template<int treeOrder>
XArrayList<pair<char, int>> InventoryCompressor<treeOrder>::convertToPairs(xMap<char, int>& map){
    XArrayList<pair<char, int>> list;
    DLinkedList<char> keys = map.keys();
    for(char k : keys)
        list.add({k, map.get(k)});
    return list;
}

template <int treeOrder>
void InventoryCompressor<treeOrder>::buildHuffman()
{
    //TODO

    auto hashFunction = [](char& key, int tableSize) { 
        return static_cast<int>(key) % tableSize; 
    };

    this->huffmanTable = new xMap<char, string>(hashFunction);

    int productCount = this->invManager->size(); 

    string allProducts = "";

    for(size_t i = 0; i < productCount; i++){
        List1D<InventoryAttribute> list = this->invManager->getProductAttributes(i);
        string name = this->invManager->getProductName(i);
        allProducts += this->productToString(list, name);
    }

    xMap<char, int> map(hashFunction);
    XArrayList<pair<char, int>> list;

    
    buildFreqTable(map, allProducts);
    list = convertToPairs(map);

    this->tree->build(list);
    
    tree->generateCodes(*this->huffmanTable);
    
}

template <int treeOrder>
void InventoryCompressor<treeOrder>::printHuffmanTable() {
    DLinkedList<char> keys = huffmanTable->keys();
    for (char ch : keys) {
        if(ch == '\0') std::cout << "'\\x00' : " << huffmanTable->get(ch) << std::endl;
        else std::cout << "'" << ch << "' : " << huffmanTable->get(ch) << std::endl;
    }
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::productToString(const List1D<InventoryAttribute> &attributes, const std::string &name)
{
    //TODO
    int size = attributes.size();
    stringstream ss;
    ss << name <<":";
    for(int i = 0; i < size; i++){
        ss<< "(";
        ss << attributes.get(i).name;
        ss << ": ";
        ss << std::fixed << std::setprecision(6) << attributes.get(i).value;
        ss << ")";
        if(i != size - 1) ss << ", ";
    }
    return ss.str();
}

template<int treeOrder>
void InventoryCompressor<treeOrder>::encodeHelper(const string& productInfo, xMap<char, string>& table, string& result){
    for(auto c : productInfo){
        result += table.get(c);
    }
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::encodeHuffman(const List1D<InventoryAttribute> &attributes, const std::string &name)
{
    //TODO
    string productInfo = productToString(attributes, name);

    if(productInfo.length() == 0) return "";
    string result = "";

    if (this->huffmanTable == nullptr) {
    stringstream os;
    os << "key (" << productInfo[0] << ") is not found";
    throw KeyNotFound(os.str());  // or std::runtime_error(os.str());
}

    encodeHelper(productInfo, *this->huffmanTable, result);

    return result;
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::decodeHuffman(const std::string &huffmanCode, List1D<InventoryAttribute> &attributesOutput, std::string &nameOutput)
{
    //TODO
    string result = this->tree->decode(huffmanCode);

    int size = this->invManager->size();

    for (int i = 0; i < size; i++) {
        string temp = productToString(invManager->getProductAttributes(i), invManager->getProductName(i));
        if (result == temp) {
            List1D<InventoryAttribute> attr = invManager->getProductAttributes(i);
            for (int j = 0; j < attr.size(); j++) {
                attributesOutput.add(attr.get(j));
            }
            nameOutput = invManager->getProductName(i);
            return result;
        }
    }
    return "\\x00";
}

