#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include "list/XArrayList.h"
#include "list/DLinkedList.h"
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;


template <typename T>
class List1D
{
private:
    IList<T> *pList;

public:
    List1D();
    List1D(int num_elements);
    List1D(const T *array, int num_elements);
    List1D(const List1D<T> &other);
    List1D(const DLinkedList<T>& list){
        this->pList = new DLinkedList<T>(list);
    }
    List1D(IList<T> *pList){this->pList = pList;};
    IList<T>* getList() const {return this->pList;} ;
    List1D& operator=(const List1D& other);
    virtual ~List1D();

    int size() const;
    T get(int index) const;
    void set(int index, T value);
    void add(const T &value);
    string toString() const;
    void removeAt(int index);
    // void sort();
    // void stableBubbleSort(vector<pair<int, string>>& arr, bool ascending = true);
    void sortProducts(List1D<string>& names, List1D<int>& attribute, List1D<int>& quantities, bool ascending = true);

    template<class U>
    friend ostream &operator<<(ostream &os, const List1D<T> &list);
};

// -------------------- List2D --------------------
template <typename T>
class List2D
{
private:
    IList<IList<T> *> *pMatrix;

public:
    List2D();
    List2D(List1D<T> *array, int num_rows);
    List2D(const List2D<T> &other);
    virtual ~List2D();

    int rows() const;
    void setRow(int rowIndex, const List1D<T> &row);
    T get(int rowIndex, int colIndex) const;
    List1D<T> getRow(int rowIndex) const;
    IList<T>& getRowIList(int rowIndex) const;
    string toString() const;
    void add(const List1D<T> &list) ;
    void removeAt(int index);
    List2D& operator=(const List2D& list);


    template <class U>
    friend ostream &operator<<(ostream &os, const List2D<T> &matrix);
};
struct InventoryAttribute
{
    string name;
    double value;
    inline InventoryAttribute(const string &name = "", double value = 0) : name(name), value(value) {}
    inline string toString() const { return name + ": " + to_string(value); }
    inline bool operator==(const InventoryAttribute& item){ return (this->name == item.name && this->value == item.value);}
    inline friend ostream& operator<<(ostream &os, const InventoryAttribute &attr) {
        os << attr.toString();  // Use the existing toString() method
        return os;
    }
    
};
// -------------------- InventoryManager --------------------
class InventoryManager
{
private:
    List2D<InventoryAttribute> attributesMatrix;
    List1D<string> productNames;
    List1D<int> quantities;

public:
    inline InventoryManager();
    inline InventoryManager(const List2D<InventoryAttribute> &matrix,
                     const List1D<string> &names,
                     const List1D<int> &quantities);
    inline InventoryManager(const InventoryManager &other);

    inline int size() const;
    inline List1D<InventoryAttribute> getProductAttributes(int index) const;
    inline string getProductName(int index) const;
    inline int getProductQuantity(int index) const;
    inline void updateQuantity(int index, int newQuantity);
    inline void addProduct(const List1D<InventoryAttribute> &attributes, const string &name, int quantity);
    inline void removeProduct(int index);
    inline bool equalAttributes(const List1D<InventoryAttribute>& row1, const List1D<InventoryAttribute>& row2);
    

    inline List1D<string> query(string attributeName, const double &minValue,
                         const double &maxValue, int minQuantity, bool ascending) const;

    inline void removeDuplicates();

    inline static InventoryManager merge(const InventoryManager &inv1,
                                  const InventoryManager &inv2);

    inline void split(InventoryManager &section1,
               InventoryManager &section2,
               double ratio) const;

    inline List2D<InventoryAttribute> getAttributesMatrix() const;
    inline List1D<string> getProductNames() const;
    inline List1D<int> getQuantities() const;
    inline string toString() const;
};

// -------------------- List1D Method Definitions --------------------
template <typename T>
List1D<T>::List1D()
{
    // TODO
    pList = new DLinkedList<T>();

}

template <typename T>
List1D<T>::List1D(int num_elements)
{
    // TODO
    pList = new DLinkedList<T>();
    for(int i = 0; i < num_elements; i++){
        pList->add(T());
    }
}

template <typename T>
List1D<T>::List1D(const T *array, int num_elements)
{
    // TODO
    pList = new DLinkedList<T>();
    for(int i = 0; i < num_elements; i++){
        pList->add(array[i]);
    }
}

template <typename T>
List1D<T>::List1D(const List1D<T> &other)
{
    // TODO
    DLinkedList<T>* sourceList = dynamic_cast<DLinkedList<T> *> (other.pList);
    pList = new DLinkedList<T>(*(sourceList));
}

template <typename T>
List1D<T>::~List1D()
{
    // TODO
    delete pList;
    pList = nullptr;
}

template <typename T>
int List1D<T>::size() const
{
    // TODO
    return pList->size();
}

template <typename T>
T List1D<T>::get(int index) const
{
    // TODO
    return pList->get(index);
}

template <typename T>
void List1D<T>::set(int index, T value)   
{
    // TODO
    pList->removeAt(index);
    pList->add(index, value);
}

template <typename T>
void List1D<T>::add(const T &value)
{
    // TODO
    pList->add(value);
}

template <typename T>
string List1D<T>::toString() const
{
    // TODO
    return pList->toString();
}

template <typename T>
ostream &operator<<(ostream &os, const List1D<T> &list)
{
    // TODO
    os << list.toString();
    return os;
}

template<class T>
void List1D<T>::removeAt(int index){
    this->pList->removeAt(index);
}

// template<class T>
// void List1D<T>::sort(){
//     this->pList->sort();
// }
// template<class T>
// void List1D<T>::stableBubbleSort(vector<pair<int, string>>& arr, bool ascending) {
//     int n = arr.size();
//     bool swapped;

//     for (int i = 0; i < n - 1; i++) {
//         swapped = false;
//         for (int j = 0; j < n - i - 1; j++) {
//             // Compare based on first element of the pair
//             if ((ascending && arr[j].first > arr[j + 1].first) ||
//                 (!ascending && arr[j].first < arr[j + 1].first)) {
//                 swap(arr[j], arr[j + 1]);
//                 swapped = true;
//             }
//         }
//         // If no two elements were swapped, the array is sorted
//         if (!swapped) break;
//     }
// }

template<class T>
List1D<T> &List1D<T>::operator=(const List1D<T>& other){
    if(this != &other){
        if(pList) delete this->pList;

        DLinkedList<T>* sourceList = dynamic_cast<DLinkedList<T>* > (other.pList);
        this->pList = new DLinkedList<T>(*(sourceList));
    }
    return *this;
}

template<class T>
void List1D<T>::sortProducts(List1D<string>& names, List1D<int>& attributes, List1D<int>& quantities, bool ascending) {
    int n = names.size();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            bool condition = false;

            if (attributes.get(j) == attributes.get(j + 1)) {
                // If attributes are equal, sort by quantities
                condition = ascending ? (quantities.get(j) > quantities.get(j + 1))
                                      : (quantities.get(j) < quantities.get(j + 1));
            } else {
                // Sort by attributes normally
                condition = ascending ? (attributes.get(j) > attributes.get(j + 1))
                                      : (attributes.get(j) < attributes.get(j + 1));
            }

            // Swap if the condition is met
            if (condition) {
                int tempAttr = attributes.get(j);
                attributes.set(j, attributes.get(j + 1));
                attributes.set(j + 1, tempAttr);

                int tempQty = quantities.get(j);
                quantities.set(j, quantities.get(j + 1));
                quantities.set(j + 1, tempQty);

                string tempName = names.get(j);
                names.set(j, names.get(j + 1));
                names.set(j + 1, tempName);
            }
        }
    }
}

// -------------------- List2D Method Definitions --------------------
template <typename T>
List2D<T>::List2D()
{
        pMatrix = new DLinkedList<IList<T>* >();
}

template <typename T>
List2D<T>::List2D(List1D<T> *array, int num_rows)
{
    // TODO
        pMatrix = new DLinkedList<IList<T>* >();
        for(int i = 0; i < num_rows; i++){
            IList<T>* Ptr = array[i].getList();

            if(DLinkedList<T>* sourceList = dynamic_cast<DLinkedList<T>*>(Ptr)){
                pMatrix->add(new DLinkedList<T>(*sourceList));
            }
        }
}

template <typename T>
List2D<T>::List2D(const List2D<T> &other)
{
    pMatrix = new DLinkedList<IList<T> *>();

        for(int i = 0; i < other.rows(); i++){
            IList<T>* row = other.pMatrix->get(i);


            if(DLinkedList<T> *sourceList = dynamic_cast<DLinkedList<T>*>(row)) 
                pMatrix->add(new DLinkedList<T> (*sourceList));
        }
}

template <typename T>
List2D<T>::~List2D()
{
    // TODO
    if(!this->pMatrix) return;
    while(pMatrix->size()>0){
        this->pMatrix->removeAt(0);
    }
    delete this->pMatrix;
}

template <typename T>
int List2D<T>::rows() const
{
    // TODO
    return this->pMatrix->size();
}

template <typename T>
void List2D<T>::setRow(int rowIndex, const List1D<T> &row)
{
    // Remove existing row if it exists
    if (rowIndex < this->rows()) {
        IList<T>* ptr = &this->getRowIList(rowIndex);
        this->removeAt(rowIndex);
        delete ptr;  // Prevent memory leak
    }

    // Add new row as a deep copy
    this->pMatrix->add(rowIndex, new DLinkedList<T>(*(dynamic_cast<DLinkedList<T>*>(row.getList()))));
}

template <typename T>
T List2D<T>::get(int rowIndex, int colIndex) const
{
    // TODO
    IList<T>* row = this->pMatrix->get(rowIndex);
    return row->get(colIndex);
}

template <typename T>
List1D<T> List2D<T>::getRow(int rowIndex) const
{
    // TODO
    if(rowIndex < 0 || rowIndex >= this->rows()) throw std::out_of_range("Index is out of range!");
    IList<T>* row = this->pMatrix->get(rowIndex);
    return List1D<T>(*dynamic_cast<DLinkedList<T>*>(row)); 
}

template <typename T>
IList<T>& List2D<T>::getRowIList(int rowIndex) const{
    if(rowIndex < 0 || rowIndex >= this->rows()) throw std::out_of_range("Index is out of range!");
    return *(this->pMatrix->get(rowIndex));
}

template <typename T>
string List2D<T>::toString() const
{
    // TODO
    if(this->rows() == 0) return "[]";
    stringstream ss;
    ss << "[";
    for(int i = 0; i < this->rows(); i++){
        List1D<T> row = this->getRow(i);
        ss << row.toString();
        if(i != this->rows() - 1) ss << ", ";
    }
    ss << "]";
    return ss.str();
}

template <typename T>
ostream &operator<<(ostream &os, const List2D<T> &matrix)
{
    os << matrix.toString();
    return os;
}

template <typename T>
void List2D<T>::add(const List1D<T> &list) {
    DLinkedList<T>* sourceList = dynamic_cast<DLinkedList<T>* >(list.getList());
    this->pMatrix->add(new DLinkedList<T>(*(sourceList)));
}
template<class T>
void List2D<T>::removeAt(int index){
    this->pMatrix->removeAt(index);
}
template<class T>
List2D<T> &List2D<T>::operator=(const List2D<T>& list){
    if(this != &list){
        delete pMatrix;
        pMatrix = new DLinkedList<IList<T>*>();

        for(int i = 0; i < list.rows(); i++){
            DLinkedList<T>* sourceList = dynamic_cast<DLinkedList<T>* >(list.pMatrix->get(i));
            pMatrix->add(new DLinkedList<T>(*sourceList));
        }
    }
    return *this;
}
// -------------------- InventoryManager Method Definitions --------------------
InventoryManager::InventoryManager() : attributesMatrix(), productNames(), quantities(){}

InventoryManager::InventoryManager(const List2D<InventoryAttribute> &matrix,
                                   const List1D<string> &names,
                                   const List1D<int> &quantities)
: attributesMatrix(matrix), productNames(names), quantities(quantities) {}

InventoryManager::InventoryManager(const InventoryManager &other)
: attributesMatrix(other.attributesMatrix), productNames(other.productNames), quantities(other.quantities){}

int InventoryManager::size() const
{
    return this->productNames.size();
}

List1D<InventoryAttribute> InventoryManager::getProductAttributes(int index) const
{
    // TODO
    if(index < 0 || index >= this->attributesMatrix.rows()) throw std::out_of_range("Index is invalid!");
    return attributesMatrix.getRow(index);
}

string InventoryManager::getProductName(int index) const
{
    // TODO
    if(index < 0 || index >= this->productNames.size()) throw std::out_of_range("Index is invalid!");
    return this->productNames.get(index);
}

int InventoryManager::getProductQuantity(int index) const
{
    // TODO
    if(index < 0 || index >= this->quantities.size()) throw std::out_of_range("Index is invalid!");
    return quantities.get(index);
}

void InventoryManager::updateQuantity(int index, int newQuantity)
{
    // TODO
    if(index < 0 || index >= this->quantities.size()) throw std::out_of_range("Index is invalid!");
    this->quantities.set(index, newQuantity);
}

void InventoryManager::addProduct(const List1D<InventoryAttribute> &attributes, const string &name, int quantity)
{
    // TODO
    this->attributesMatrix.add(attributes);
    this->productNames.add(name);
    this->quantities.add(quantity);
}

void InventoryManager::removeProduct(int index)
{
    // TODO
    if(index < 0 || index >= this->attributesMatrix.rows()) throw std::out_of_range("Index is invalid!");
    this->attributesMatrix.removeAt(index);
    this->productNames.removeAt(index);
    this->quantities.removeAt(index);
}

bool InventoryManager::equalAttributes(const List1D<InventoryAttribute>& row1, const List1D<InventoryAttribute>& row2){
    if(row1.size() != row2.size()) return false;
    for(int i = 0; i < row1.size(); i++){
        if(row1.get(i).value != row2.get(i).value || row1.get(i).name != row2.get(i).name) return false;
    }
    return true;
}

List1D<string> InventoryManager::query(string attributeName, const double &minValue,
    const double &maxValue, int minQuantity, bool ascending) const
{
// TODO
List1D<string> result;
List1D<int> attribute;
List1D<int> quantity;
for(int i = 0; i < attributesMatrix.rows(); i++){
List1D<InventoryAttribute> row = attributesMatrix.getRow(i);

for(int j = 0; j < row.size(); j++){
if(attributeName == row.get(j).name){
if(row.get(j).value>=minValue && row.get(j).value<=maxValue){
if(this->quantities.get(i) >= minQuantity){
result.add(productNames.get(i));
attribute.add(row.get(j).value);
quantity.add(quantities.get(i));
}
}
}
}
}

result.sortProducts(result, attribute, quantity, ascending);
return result;
}

void InventoryManager::removeDuplicates()
{
    // TODO
    if(this->quantities.size() <= 1) return;

    for(int i = 0; i < this->productNames.size(); i++){
        string name = productNames.get(i);
        
        
        for(int j = this->productNames.size() - 1; j > i; j--){
            if(name == this->productNames.get(j)){
                List1D<InventoryAttribute> row1 = this->attributesMatrix.getRow(i);
                List1D<InventoryAttribute> row2 = this->attributesMatrix.getRow(j);

                if(!equalAttributes(row1, row2)) continue;
                
                this->attributesMatrix.removeAt(j);
                this->productNames.removeAt(j);
                int item = this->quantities.get(i);
                item += this->quantities.get(j);
                this->quantities.removeAt(j);
                this->quantities.set(i, item);
            }
        }
    }
}

InventoryManager InventoryManager::merge(const InventoryManager &inv1,
                                         const InventoryManager &inv2)
{
    // TODO
    InventoryManager merged(inv1);
    
    for(int i = 0; i < inv2.productNames.size(); i++){
            List1D<InventoryAttribute> row = inv2.attributesMatrix.getRow(i);
            
            merged.attributesMatrix.add(row);
            merged.productNames.add(inv2.productNames.get(i));
            merged.quantities.add(inv2.quantities.get(i));
    }
    return merged;
}

void InventoryManager::split(InventoryManager &section1,
                             InventoryManager &section2,
                             double ratio) const
{
    // TODO
    //remove


    int x = section1.productNames.size();
    int y = section2.productNames.size();

    for(int i = 0; i < x; i++){
        section1.removeProduct(0);
    }
    for(int i = 0; i < y; i++){
        section2.removeProduct(0);
    }

    int n;

    if(ratio < 0){
        n = 0;
    }
    else if(ratio > 1){
        n = this->productNames.size();
    }
    else{
        n = ceil(this->productNames.size() * ratio  - 1e-9);
    }

    for(int i = 0; i < n; i++){
        List1D<InventoryAttribute> row = this->attributesMatrix.getRow(i);
        section1.attributesMatrix.add(row);
        section1.productNames.add(this->productNames.get(i));
        section1.quantities.add(this->quantities.get(i));
    }
    for(int i = n; i < this->productNames.size(); i++){
        List1D<InventoryAttribute> row = this->attributesMatrix.getRow(i);
        section2.attributesMatrix.add(row);
        section2.productNames.add(this->productNames.get(i));
        section2.quantities.add(this->quantities.get(i));
    }
}

List2D<InventoryAttribute> InventoryManager::getAttributesMatrix() const
{
    // TODO
    return this->attributesMatrix;
}

List1D<string> InventoryManager::getProductNames() const
{
    // TODO
    return this->productNames;
}

List1D<int> InventoryManager::getQuantities() const
{
    // TODO
    return this->quantities;
}

string InventoryManager::toString() const
{

    // TODO
    stringstream ss;
    ss << "InventoryManager[" << endl << "  AttributesMatrix: ";
    ss << this->attributesMatrix.toString() << "," << endl;
    ss << "  ProductNames: " << this->productNames.toString()<<","<<endl;
    ss << "  Quantities: " << this->quantities.toString()<<endl;
    ss << "]";
    return ss.str();
}



#endif /* INVENTORY_MANAGER_H */
