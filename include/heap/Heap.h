#ifndef HEAP_H
#define HEAP_H
#include <memory.h>
#include "heap/IHeap.h"
#include <sstream>
#include <iostream>
#include "list/XArrayList.h"
using namespace std;
/*
 * function pointer: int (*comparator)(T& lhs, T& rhs)
 *      compares objects of type T given in lhs and rhs.
 *      return: sign of (lhs - rhs)
 *              -1: of lhs < rhs
 *              0 : lhs == rhs
 *              +1: ls > rhs
 * 
 * function pointer: void (*deleteUserData)(Heap<T>* pHeap)
 *      remove user's data in case that T is a pointer type
 *      Users should pass &Heap<T>::free for "deleteUserData"
 * 
 */
template<class T>
class Heap: public IHeap<T>{
public:
    class Iterator; //forward declaration
    
protected:
    T *elements;    //a dynamic array to contain user's data
    int capacity;   //size of the dynamic array
    int count;      //current count of elements stored in this heap
    int (*comparator)(T& lhs, T& rhs);      //see above
    void (*deleteUserData)(Heap<T>* pHeap); //see above
    
public:
    Heap(   int (*comparator)(T& , T&)=0, 
            void (*deleteUserData)(Heap<T>*)=0 );
    
    Heap(const Heap<T>& heap); //copy constructor 
    Heap<T>& operator=(const Heap<T>& heap); //assignment operator
    
    ~Heap();
    
    //Inherit from IHeap: BEGIN
    void push(T item);
    T pop();
    const T peek();
    void remove(T item, void (*removeItemData)(T)=0);
    bool contains(T item);
    int size();
    void heapify(T array[], int size);
    void clear();
    bool empty();
    string toString(string (*item2str)(T&)=0 );
    //Inherit from IHeap: END
    
    void println(string (*item2str)(T&)=0 ){
        cout << toString(item2str) << endl;
    }
    
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }
    void heapsort(XArrayList<T>& arrayList);
    
public:
    /* if T is pointer type:
     *     pass the address of method "free" to Heap<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  Heap<Point*> heap(&Heap<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(Heap<T> *pHeap){
      for (int idx = 0; idx < pHeap->count; idx++) {
          if (pHeap->elements[idx]) {
              delete pHeap->elements[idx];
              pHeap->elements[idx] = nullptr;
          }
      }
  }

private:
    bool aLTb(T& a, T& b){
        return compare(a, b) < 0;
    }
    int compare(T& a, T& b){
        if(comparator) return comparator(a, b);
        else{
            if (a < b) return -1;
            else if(a > b) return 1;
            else return 0;
        }
    }
    static int compareMaxHeap(T& a, T& b){
        if(a > b) return -1;
        else if(a < b) return 1;
        else return 0;
    }
    
    void ensureCapacity(int minCapacity); 
    void swap(int a, int b);
    void reheapUp(int position);
    void reheapDown(int position);
    int getItem(T item);
    
    void removeInternalData();
    void copyFrom(const Heap<T>& heap);


    
//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
    
    //Iterator: BEGIN
    class Iterator{
    private:
        Heap<T>* heap;
        int cursor;
    public:
        Iterator(Heap<T>* heap=0, bool begin=0){
            this->heap = heap;
            if(begin && (heap !=0)) cursor = 0;
            if(!begin && (heap !=0)) cursor = heap->size();
        }
        Iterator& operator=(const Iterator& iterator){
            this->heap = iterator.heap;
            this->cursor = iterator.cursor;
            return *this;
        }
        
        T& operator*(){
            return this->heap->elements[cursor];
        }
        bool operator!=(const Iterator& iterator){
            return this->cursor != iterator.cursor;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            cursor++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
        void remove(void (*removeItemData)(T)=0){
            this->heap->remove(this->heap->elements[cursor], removeItemData);
        }
    };
    //Iterator: END
};


//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
Heap<T>::Heap(
        int (*comparator)(T&, T&), 
        void (*deleteUserData)(Heap<T>* ) ){
    this->capacity = 10;
    this->count = 0;
    this->elements = new T[this->capacity];

    this->comparator = comparator;
    this->deleteUserData = deleteUserData;
}
template<class T>
Heap<T>::Heap(const Heap<T>& heap){
    this->copyFrom(heap);
}

template<class T>
Heap<T>& Heap<T>::operator=(const Heap<T>& heap){
  if(this != &heap){
    this->removeInternalData();
    this->copyFrom(heap);
  }
    return *this;
}


template<class T>
Heap<T>::~Heap(){
    this->removeInternalData();
}

template<class T>
void Heap<T>::push(T item){ //item  = 25
    ensureCapacity(count + 1);
    elements[count] = item;
    reheapUp(count);
    ++count;
}
/*
      18
     /  \
    15   13
    /
  25 
 =>
      25
     /  \
    18   13
    /
  15 
=> array: [25, 18, 13, 15, , ]
           0   1    2   3
 
 */
template<class T>
T Heap<T>::pop(){
    if(count == 0) throw std::underflow_error("Calling to peek with the empty heap.");
    T temp = elements[0];
    elements[0] = elements[count-1];
    this->count--;
    reheapDown(0);
    return temp;
}

/*
      15
     /  \
    18   13
 => ReheapDown
      18
     /  \
    15   13
=> Array: [18, 15, 13, , , ]
 */

template<class T>
const T Heap<T>::peek(){
    if(this->count == 0) throw std::underflow_error("Calling to peek with the empty heap.");
    return elements[0];
}


template<class T>
void Heap<T>::remove(T item, void (*removeItemData)(T)){
    int foundIdx = getItem(item);
    if(foundIdx == -1) return;
    if(foundIdx == 0){
        T temp = this->pop();
        if(removeItemData) removeItemData(temp);
    }
    else{
        if(removeItemData) removeItemData(elements[foundIdx]);
        elements[foundIdx] = elements[this->count-1];
        this->count--;
        reheapDown(foundIdx);
    }
}

/*
//April 03, 2023
template<class T>
void Heap<T>::remove_bck(T item, void (*removeItemData)(T)){
    int foundIdx = this->getItem(item);
    
    //CASE 1: not found
    if(foundIdx == -1) return;

    //CASE 2: found at 0 => use pop
    if(foundIdx == 0){
        T removed = this->pop();
        if(removeItemData != 0) removeItemData(removed);
        return;
    }
    
    //CASE 3: found at anywhere else
    //Remove item's data
    if(removeItemData != 0) removeItemData(elements[foundIdx]);
    
    //Remove item: shift left
    int copyCount = (count - 1) - (foundIdx + 1) + 1;
    memcpy(&elements[foundIdx], &elements[foundIdx + 1], copyCount*sizeof(T));
    count -= 1;

    //Determine valid heap [0-> (new size -1)]
    int startOldData = foundIdx;
    int lastOldData = count - 1;
    count = foundIdx;

    //reheapify from startOldData
    for(int idx=startOldData; idx <= lastOldData; idx++)
        push(elements[idx]);
}
*/
template<class T>
bool Heap<T>::contains(T item){
    if(getItem(item) != -1) return true;
    return false;
}

template<class T>
int Heap<T>::size(){
    return this->count;
}

template<class T>
void Heap<T>::heapify(T array[], int size){
    this->removeInternalData();
    this->capacity = size;
    this->elements = new T[this->capacity];

    for(int i = 0; i < size; i++) this->elements[i] = array[i];
    this->count = size;

    for(int i = (size-1)/2; i >= 0; i--){
        reheapDown(i);
    }
}

template<class T>
void Heap<T>::clear(){
    this->removeInternalData();
    this->capacity = 10;
    this->count = 0;
    this->elements = new T[this->capacity];
}

template<class T>
bool Heap<T>::empty(){
    return (this->count == 0);
}

template<class T>
string Heap<T>::toString(string (*item2str)(T&)){
    stringstream ss;
    ss << "[";
    for(int i = 0; i < this->count; i++){
        if(item2str) ss << item2str(elements[i]);
        else ss << elements[i];
        if(i != this->count - 1) ss <<",";
    }
    ss << "]";
    return ss.str();
}


//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
void Heap<T>::ensureCapacity(int minCapacity){
    if(minCapacity >= capacity){
        //re-allocate 
        int old_capacity = capacity;
        capacity = old_capacity + (old_capacity >> 2);
        try{
            T* new_data = new T[capacity];
            //OLD: memcpy(new_data, elements, capacity*sizeof(T));
            memcpy(new_data, elements, old_capacity*sizeof(T));
            delete []elements;
            elements = new_data;
        }
        catch(std::bad_alloc e){
            e.what();
        }
    }
}

template<class T>
void Heap<T>::swap(int a, int b){
    T temp = this->elements[a];
    this->elements[a] = this->elements[b];
    this->elements[b] = temp;
}

template<class T>
void Heap<T>::reheapUp(int position){
    if(position <= 0) return;
    int parent = (position-1)/2;
    if(aLTb(this->elements[position], this->elements[parent])){
        this->swap(position, parent);
        reheapUp(parent);
    }
}

template<class T>
void Heap<T>::reheapDown(int position){
    int leftChild = position*2 + 1;
    int rightChild = position*2 + 2;
    int lastPosition = this->count - 1;

    if(leftChild <= lastPosition){
        int smallChild = leftChild; //by default => leftChild valid but rightChild invalid
        if(rightChild <= lastPosition){
            if(aLTb(this->elements[leftChild], this->elements[rightChild])) 
                smallChild = leftChild;
            else smallChild = rightChild;
        }

        if(aLTb(this->elements[smallChild], this->elements[position])){
            this->swap(smallChild, position);
            reheapDown(smallChild);
        }
    }
}

template<class T>
int Heap<T>::getItem(T item){
    int foundIdx = -1;
    for(int idx=0; idx < this->count; idx++){
        if(compare(elements[idx], item) == 0){
            foundIdx = idx;
            break;
        }
    }
    return foundIdx;
}

template<class T>
void Heap<T>::removeInternalData(){
    if(this->deleteUserData != 0) deleteUserData(this); //clear users's data if they want
    if(elements)
        delete []elements;
    elements = nullptr;
}

template<class T>
void Heap<T>::copyFrom(const Heap<T>& heap){
    capacity = heap.capacity;
    count = heap.count;
    elements = new T[capacity];
    this->comparator = heap.comparator;
    this->deleteUserData = heap.deleteUserData;
    
    //Copy items from heap:
    int size = const_cast<Heap<T>&>(heap).size();
    for(int idx=0; idx < size; idx++){
        if constexpr (std::is_pointer<T>::value) this->elements[idx] = new std::remove_pointer_t<T>(*(heap.elements[idx]));
        else this->elements[idx] = heap.elements[idx];
    }
}

template<class T>
void Heap<T>::heapsort(XArrayList<T>& arrayList){
    if(this->count > 0) this->clear();
    
    int size = arrayList.size();
    if(size == 0) return;

    typename XArrayList<T>::Iterator it(&arrayList, 0);
    typename XArrayList<T>::Iterator end(&arrayList, size);

    T arr[size];
    for(int i = 0; i < size; i++)  arr[i] = arrayList.get(i);

    this->heapify(arr, size);

    while(it != end){
        *it = this->pop();
        cout << arrayList.toString() << endl;
        ++it;
    }
}

#endif
