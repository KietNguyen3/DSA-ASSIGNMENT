/*
 * File:   DLinkedList.h
 */

 #ifndef DLINKEDLIST_H
 #define DLINKEDLIST_H
 
 #include "IList.h"
 
 #include <sstream>
 #include <iostream>
 #include <type_traits>
 using namespace std;
 
 template <class T>
 class DLinkedList : public IList<T>
 {
 public:
     class Node;        // Forward declaration
     class Iterator;    // Forward declaration
     class BWDIterator; // Forward declaration
 
 protected:
     Node *head; // this node does not contain user's data
     Node *tail; // this node does not contain user's data
     int count;
     bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
     void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)
 
 public:
     DLinkedList(
         void (*deleteUserData)(DLinkedList<T> *) = 0,
         bool (*itemEqual)(T &, T &) = 0);
     DLinkedList(const DLinkedList<T> &list);
     DLinkedList<T> &operator=(const DLinkedList<T> &list);
     ~DLinkedList();
 
     // Inherit from IList: BEGIN
     void set(int index, T value);
     void add(T e);
     void add(int index, T e);
     T removeAt(int index);
     bool removeItem(T item, void (*removeItemData)(T) = 0);
     bool empty();
     int size();
     void clear();
     T &get(int index);
     int indexOf(T item);
     bool contains(T item);
     string toString(string (*item2str)(T &) = 0);
     // void sort();
     // Inherit from IList: END
 
     void println(string (*item2str)(T &) = 0)
     {
         cout << toString(item2str) << endl;
     }
     void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
     {
         this->deleteUserData = deleteUserData;
     }
 
     bool contains(T array[], int size)
     {
         int idx = 0;
         for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
         {
             if (!equals(*it, array[idx++], this->itemEqual))
                 return false;
         }
         return true;
     }
 
     /*
      * free(DLinkedList<T> *list):
      *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
      *  + if users want a DLinkedList removing their data,
      *      he/she must pass "free" to constructor of DLinkedList
      *      Example:
      *      DLinkedList<T> list(&DLinkedList<T>::free);
      */
     static void free(DLinkedList<T> *list)
     {
         typename DLinkedList<T>::Iterator it = list->begin();
         while (it != list->end())
         {
             delete *it;
             it++;
         }
     }
 
     /* begin, end and Iterator helps user to traverse a list forwardly
      * Example: assume "list" is object of DLinkedList
 
      DLinkedList<char>::Iterator it;
      for(it = list.begin(); it != list.end(); it++){
             char item = *it;
             std::cout << item; //print the item
      }
      */
     Iterator begin()
     {
         return Iterator(this, true);
     }
     Iterator end()
     {
         return Iterator(this, false);
     }
 
     /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
      * Example: assume "list" is object of DLinkedList
 
      DLinkedList<char>::BWDIterator it;
      for(it = list.last(); it != list.beforeFirst(); it--){
             char item = *it;
             std::cout << item; //print the item
      }
      */
     BWDIterator bbegin()
     {
         return BWDIterator(this, true);
     }
     BWDIterator bend()
     {
         return BWDIterator(this, false);
     }
 
 protected:
     static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
     {
         if (itemEqual == 0)
             return lhs == rhs;
         else
             return itemEqual(lhs, rhs);
     }
     void copyFrom(const DLinkedList<T> &list);
     void removeInternalData();
     Node *getPreviousNodeOf(int index);
 
     //////////////////////////////////////////////////////////////////////
     ////////////////////////  INNER CLASSES DEFNITION ////////////////////
     //////////////////////////////////////////////////////////////////////
 public:
     class Node
     {
     public:
         T data;
         Node *next;
         Node *prev;
         friend class DLinkedList<T>;
 
     public:
         Node(Node *next = 0, Node *prev = 0)
         {
             this->next = next;
             this->prev = prev;
         }
         Node(T data, Node *next = 0, Node *prev = 0)
         {
             this->data = data;
             this->next = next;
             this->prev = prev;
         }
     };
 
     //////////////////////////////////////////////////////////////////////
     class Iterator
     {
     private:
         DLinkedList<T> *pList;
         Node *pNode;
 
     public:
         Iterator(DLinkedList<T> *pList = 0, bool begin = true)
         {
             if (begin)
             {
                 if (pList != 0)
                     this->pNode = pList->head->next;
                 else
                     pNode = 0;
             }
             else
             {
                 if (pList != 0)
                     this->pNode = pList->tail;
                 else
                     pNode = 0;
             }
             this->pList = pList;
         }
 
         Iterator &operator=(const Iterator &iterator)
         {
             this->pNode = iterator.pNode;
             this->pList = iterator.pList;
             return *this;
         }
         void remove(void (*removeItemData)(T) = 0)
         {
             pNode->prev->next = pNode->next;
             pNode->next->prev = pNode->prev;
             Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
             if (removeItemData != 0)
                 removeItemData(pNode->data);
             delete pNode;
             pNode = pNext;
             pList->count -= 1;
         }
 
         T &operator*()
         {
             return pNode->data;
         }
         bool operator!=(const Iterator &iterator)
         {
             return pNode != iterator.pNode;
         }
         // Prefix ++ overload
         Iterator &operator++()
         {
             pNode = pNode->next;
             return *this;
         }
         // Postfix ++ overload
         Iterator operator++(int)
         {
             Iterator iterator = *this;
             ++*this;
             return iterator;
         }
     };
 
     // Backward Iterator
     class BWDIterator
     {
     private:
         DLinkedList<T> *pList;
         Node *pNode;
 
     public:
         BWDIterator(DLinkedList<T> *pList = 0, bool last = true)
         {
             if (last)
             {
                 if (pList != 0)
                     this->pNode = pList->tail->prev;
                 else
                     pNode = 0;
             }
             else
             {
                 if (pList != 0)
                     this->pNode = pList->head;
                 else
                     pNode = 0;
             }
             this->pList = pList;
         }
 
         BWDIterator &operator=(const BWDIterator &iterator)
         {
             this->pNode = iterator.pNode;
             this->pList = iterator.pList;
             return *this;
         }
         void remove(void (*removeItemData)(T) = 0)
         {
             pNode->prev->next = pNode->next;
             pNode->next->prev = pNode->prev;
             Node *pNext = pNode->next; // MUST next, so iterator-- will go to head
             if (removeItemData != 0)
                 removeItemData(pNode->data);
             delete pNode;
             pNode = pNext;
             pList->count -= 1;
         }
 
         T &operator*()
         {
             return pNode->data;
         }
         bool operator!=(const BWDIterator &iterator)
         {
             return pNode != iterator.pNode;
         }
         // Prefix -- overload
         BWDIterator &operator--()
         {
             pNode = pNode->prev;
             return *this;
         }
         // Postfix -- overload
         BWDIterator operator--(int)
         {
             BWDIterator iterator = *this;
             --*this;
             return iterator;
         }
     };
 };
 //////////////////////////////////////////////////////////////////////
 // Define a shorter name for DLinkedList:
 
 template <class T>
 using List = DLinkedList<T>;
 
 //////////////////////////////////////////////////////////////////////
 ////////////////////////     METHOD DEFNITION      ///////////////////
 //////////////////////////////////////////////////////////////////////
 
 template <class T>
 DLinkedList<T>::DLinkedList(
     void (*deleteUserData)(DLinkedList<T> *),
     bool (*itemEqual)(T &, T &)){
         head = new Node();
         tail = new Node();
         head->next = tail;
         tail->prev = head;
         this->deleteUserData = deleteUserData;
         this->itemEqual = itemEqual;
         this->count = 0;
     }
 
 template <class T>
 DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
 {
     this->deleteUserData = list.deleteUserData;
     this->itemEqual = list.itemEqual;
     this->head = new Node();
     this->tail = new Node();
     this->count = 0;
     this->head->next = this->tail;
     this->tail->prev = this->head;
 
     this->copyFrom(list);
 }
 
 template <class T>
 DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
 {
     if(this!=&list){
         this->clear();
         this->copyFrom(list);
     }
     return *this;
 }
 
 template <class T>
 DLinkedList<T>::~DLinkedList()
 {
     this->removeInternalData();
 }
 
 template <class T>
 void DLinkedList<T>::set(int index, T value){
     if(index < 0 || index >= this->count) throw std::out_of_range("Index is out of range");
     this->removeAt(index);
     this->add(index, value);
 }
 
 template <class T>
 void DLinkedList<T>::add(T e)
 {
     Node* newNode = new Node(e);
     newNode->next = this->tail;
     newNode->prev = this->tail->prev;
     this->tail->prev->next = newNode;
     this->tail->prev = newNode;
     this->count++;
 }
 template <class T>
 void DLinkedList<T>::add(int index, T e)
 {
     if(index < 0 || index > this->count){
         throw std::out_of_range("Index is out of range!");
     }
     if(index == this->count){
         add(e);
         return;
     }
     else{
         Node* newNode = new Node(e);
         Node* temp = getPreviousNodeOf(index);
 
         newNode->prev = temp;
         newNode->next = temp->next;
         temp->next->prev = newNode;
         temp->next = newNode;
         this->count++;
     }
     return;
 }
 
 template <class T>
 typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
 {
     /**
      * Returns the node preceding the specified index in the doubly linked list.
      * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
      * Efficiently navigates to the node by choosing the shorter path based on the index's position.
      */
     // TODO
     if(index < 0 || index >= this->count) throw std::out_of_range("Index is out of range!");
     if(index == 0) return this->head;
     int half = (this->count - 1)/2;
     Node* temp;
     if(index <= half){
          temp = head;
         for(int i = 0; i < index; i++){
             temp = temp->next;
         }
     }
     else if(index > half){
          temp = tail->prev;
         for(int i = this->count - 1; i > index - 1; i--){
             temp = temp->prev;
         }
     }
     return temp;
 }
 
 template <class T>
 T DLinkedList<T>::removeAt(int index)
 {
     if(index < 0 || index >= this->count) throw std::out_of_range("Index is out of range!");
     Node* temp = getPreviousNodeOf(index);
     temp = temp->next;
     temp->prev->next = temp->next;
     temp->next->prev = temp->prev;
     temp->next = temp->prev = nullptr;
     T item = temp->data;
 
     delete temp;
     this->count--;
     return item;
 }
 
 template <class T>
 bool DLinkedList<T>::empty()
 {
     return(this->count == 0);
 }
 
 template <class T>
 int DLinkedList<T>::size()
 {
     return this->count;
 }
 
 template <class T>
 void DLinkedList<T>::clear()
 {
     this->removeInternalData();
     head = new Node();
     tail = new Node();
     head->next = tail;
     tail->prev = head;
     count = 0;
 }
 
 template <class T>
 T &DLinkedList<T>::get(int index)
 {
 
     if(index < 0 || index >= this->count) throw std::out_of_range("Index is out of range!");
     Node* temp = getPreviousNodeOf(index)->next;
     return temp->data;
 }
 
 template <class T>
 int DLinkedList<T>::indexOf(T item)
 {
 
     int step = 0;
     Node* temp = this->head->next;
     while(temp != this->tail){
         if((this->itemEqual? this->itemEqual(temp->data, item) : temp->data == item)) return step;
         step++;
         temp = temp->next;
     }
     return -1;
 }
 
 
 template <class T>
 bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
 {
     // TODO
     if(!this->contains(item)) return false;
     Node* temp = this->head->next;
     for(int i = 0; i < this->count; i++){
         if((this->itemEqual? this->itemEqual(temp->data, item) : temp->data == item)){
             if(removeItemData) removeItemData(temp->data);
             this->removeAt(i);
             return true;
         }
         temp = temp->next;
     }
     return false;
 }
 
 template <class T>
 bool DLinkedList<T>::contains(T item)
 {
     // TODO
     if(indexOf(item) == -1) return false;
     else return true;
 }
 
 template <class T>
 string DLinkedList<T>::toString(string (*item2str)(T &))
 {
     /**
      * Converts the list into a string representation, where each element is formatted using a user-provided function.
      * If no custom function is provided, it directly uses the element's default string representation.
      * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
      *
      * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
      * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
      */
     // TODO
     if(this->count == 0) return "[]";
     stringstream ss;
     ss << "[" ;
     Node* temp = this->head->next;
     while(temp != this->tail){
         if(item2str){
             ss << item2str(temp->data);
         }
         else{
             ss << temp->data;
         }
         temp = temp->next;
         if(temp != this->tail) ss << ", ";
     }
     ss << "]";
     return ss.str();
 }
 
 template <class T>
 void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
 {
     /**
      * Copies the contents of another doubly linked list into this list.
      * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
      * Iterates through the source list and adds each element, preserving the order of the nodes.
      */
     // TODO
         // this->clear();
 
         Node* temp = list.head->next;
         while(temp != list.tail){
             this->add(temp->data);
             temp = temp->next;
         }
 }
 
 template <class T>
 void DLinkedList<T>::removeInternalData()
 {
     /**
      * Clears the internal data of the list by deleting all nodes and user-defined data.
      * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
      * Traverses and deletes each node between the head and tail to release memory.
      */
     // TODO
     if(!this->head || !this->tail) return;
     if(deleteUserData){
         deleteUserData(this);
         deleteUserData = nullptr;
         if(head) delete head;
         if(tail) delete tail;
         count = 0;
     }
     else{
         Node* temp = this->head->next;
         Node* prevTemp = this->head;
             
         while(temp != this->tail){
             prevTemp = temp;
             temp = temp->next;
             delete prevTemp;
         }
         delete head;
         delete tail;
         this->count = 0;
     }
     if(itemEqual) itemEqual = nullptr;
 }
 
 // template<class T> 
 // void DLinkedList<T>::sort(){
 //     if(this->size() <= 1) return;
 //     Node* current = this->head->next;
 //     Node* lastNode = this->tail;
 //     bool swapped;
 //     while(true){
 //         swapped = false;
 //         while(current->next != lastNode){
 //             if(current->data > current->next->data){
 //                 swap(current->data, current->next->data);
 //                 swapped = true;
 //             }
 //             current = current->next;
 //         }
 //         if(!swapped) break;
 //         lastNode = current->prev;
 //         current = this->head->next;
 //     }
 // }
 
 #endif /* DLINKEDLIST_H */
 