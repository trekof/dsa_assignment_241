/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

// #include "list/IList.h"  //code goc
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
    // Inherit from IList: END

    //---------- dua ra public de test --------------
    // void removeInternalData();
    //---------- dua ra public de test --------------
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

// //---------- TO TEST getPreviousNodeOf(int index) ------------------
    // Node* getPrevNodeOf(int index);
// //---------- TO TEST getPreviousNodeOf(int index) ------------------

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
    void removeInternalData(); // dua ra public de test
    Node *getPreviousNodeOf(int index);

    void checkIndex(int index);     // check validity of index for accessing
    void checkIndexForAdd(int index);
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

    class BWDIterator 
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;
    public: 
        BWDIterator(DLinkedList<T> *pList = 0, bool bbegin = true)
        {
            if (bbegin) 
            {
                if (pList != 0) {
                    this->pNode = pList->tail;
                }
                else {
                    pNode = 0;
                }
            }
            else 
            {
                if (pList != 0) {
                    this->pNode = pList->head->next;
                }
                else {
                    pNode = 0;
                }
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
        bool operator!=(const BWDIterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        BWDIterator &operator--()
        {
            pNode = pNode->prev;
            return *this;
        }
        // Postfix ++ overload
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

//---------- TO TEST getPreviousNodeOf(int index) ------------------
// template <class T>
// typename DLinkedList<T>::Node *DLinkedList<T>::getPrevNodeOf(int index) {
//     return this->getPreviousNodeOf(index);
// }
//---------- TO TEST getPreviousNodeOf(int index) ------------------

template <class T>
void deleteUserData(DLinkedList<T> *list) {
    // Duyệt qua từng phần tử trong danh sách và giải phóng từng con trỏ
    // for (int i = 0; i < list->size(); i++) {
    //     delete list->get(i);  // Giải phóng đối tượng mà con trỏ đang trỏ tới
    // }

    typename DLinkedList<T>::Node* currentNode = list->head->next;
    while (currentNode != list->tail) {
        if (currentNode != nullptr) {
            delete currentNode->data;
        }
        currentNode = currentNode->next;
    }
}

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // TODO
    //cout << "OK1";
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;

    this->head = new Node(0, nullptr, nullptr);
    this->tail = new Node(0, nullptr, nullptr);

    this->tail->prev = this->head;
    this->head->next = this->tail;
    this->count = 0;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // TODO
    this->head = new Node(0, nullptr, nullptr);
    this->tail = new Node(0, nullptr, nullptr);

    this->tail->prev = this->head;
    this->head->next = this->tail;
    this->count = 0;
    //cout << "Copy constructor is called." << endl; // Thêm câu lệnh để kiểm tra
    this->copyFrom(list);
    //cout << "Copy completed in constructor." << endl; // Thêm câu lệnh để kiểm tra

    // this->head = new Node(0, nullptr, nullptr);
    // this->tail = new Node(0, nullptr, nullptr);

    // if (this->count != 0) {

    // }


    //return *this;
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // TODO
    if (this != &list) {
        this->copyFrom(list);
    }
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // TODO
    //cout << "The destructor is called." << endl;
    this->removeInternalData();
    this->head = nullptr;
    this->tail = nullptr;
}

template <class T>
void DLinkedList<T>::checkIndex(int index)
{
    /**
     * Validates whether the given index is within the valid range of the list.
     * Throws an std::out_of_range exception if the index is negative or exceeds the number of elements.
     * Ensures safe access to the list's elements by preventing invalid index operations.
     */
    // TODO
    if (index < 0 || index >= this->count) {
        throw std::out_of_range("Index is out of range!");
    }
}

template <class T>
void DLinkedList<T>::checkIndexForAdd(int index)
{
    /**
     * Validates whether the given index is within the valid range of the list.
     * Throws an std::out_of_range exception if the index is negative or exceeds the number of elements.
     * Ensures safe access to the list's elements by preventing invalid index operations.
     */
    // TODO
    if (index < 0 || index > this->count) {
        throw std::out_of_range("Index is out of range!");
    }
}

template <class T>
void DLinkedList<T>::add(T e)
{
    // TODO
    Node* newNode = new Node(e, nullptr, nullptr);
    if (this->count == 0) {
        this->head->next = newNode;
        this->tail->prev = newNode;
        newNode->prev = this->head;
        newNode->next = this->tail;
    }
    else {
        Node* preTail = this->tail->prev;
        preTail->next = newNode;
        this->tail->prev = newNode;
        newNode->prev = preTail;
        newNode->next = this->tail;
    }
    this->count = this->count + 1;
    //cout << "The data of prevtail is: " << this->tail->prev->data << endl;

}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // TODO
    this->checkIndexForAdd(index);

    Node* newNode = new Node(e, nullptr, nullptr);
    // Node* prevIndex = this->getPreviousNodeOf(index);

    // newNode->next = prevIndex->next;
    // newNode->prev = prevIndex;
    // prevIndex->next->prev = newNode;
    // prevIndex->next = newNode; 

    // Node *temp = this->head->next;
    // if (index == 0)
    // {
    //     newNode->next = temp;

    //     if (this->count > 0)
    //     {
    //         temp->prev = newNode;
    //     }
    //     this->head->next = newNode;

    //     this->count = this->count + 1;
    //     return;
    // }

    // find the previous node of index
    // for (int i = 0; i < index - 1 && temp != NULL; i++)
    // {
    //     temp = temp->next;
    // }
    Node* temp = nullptr;
    temp = this->getPreviousNodeOf(index);

    newNode->prev = temp;

    newNode->next = temp->next;

    temp->next = newNode;

    if (newNode->next != this->tail)
    {
        newNode->next->prev = newNode;
    }
    else {
        this->tail->prev = newNode;
    }

    this->count = this->count + 1;
    //cout << "The data of prevtail is: " << this->tail->prev->data << endl;
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
    this->checkIndexForAdd(index);

    // Node* prevIndex = nullptr;
    // if (index <= this->count / 2) {
    //     // prevIndex = this->head;
    //     prevIndex = this->head->next;
    //     for (int i = 0; i < index - 1; i++) {
    //         prevIndex = prevIndex->next;
    //     }
    // }
    // else {
    //     prevIndex = this->tail->prev;
    //     for (int i = this->count - 1; i >= index - 1; i--) {
    //         prevIndex = prevIndex->prev;
    //     }
    // }
    if (index == 0) {
        Node* temp = this->head;
        return temp;
    }

    Node *temp = this->head->next;
    for (int i = 0; i < index - 1 && temp != NULL; i++)
    {
        temp = temp->next;
    }

    return temp;
    // return prevIndex;
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    // TODO

    // if (index == 0) {
    //     Node* nodeToRemove = this->head->next;
    //     T dataAtNodeRemoved = nodeToRemove->data;

    //     this->head->next = nodeToRemove->next;
    //     nodeToRemove->next->prev = this->head;
    // }
    this->checkIndex(index);

    Node* prevIndex = this->getPreviousNodeOf(index);
    Node* nodeToRemove = prevIndex->next;
    T dataAtNodeRemoved = nodeToRemove->data;

    prevIndex->next = nodeToRemove->next;
    nodeToRemove->next->prev = prevIndex;

    if constexpr (is_pointer<T>::value) {
        //cout << "Data at node removed is a pointer. " << endl;
        //this->deleteUserData(dataAtNodeRemoved);
        //delete dataAtNodeRemoved;
        //this->deleteUserData();
        delete nodeToRemove->data;
    }
    // else {
    //     cout << "Data at node removed is not a pointer." << endl;
    // }

    delete nodeToRemove;

    this->count = this->count - 1;
    return dataAtNodeRemoved;
}

template <class T>
bool DLinkedList<T>::empty()
{
    // TODO
    return this->count == 0;
}

template <class T>
int DLinkedList<T>::size()
{
    // TODO
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // TODO
    // Node *current = this->head->next;
    // Node *temp = nullptr;

    // while(current != this->tail) {
    //     temp = current;
    //     current = current->next;
    //     //delete temp;
    //     temp->data = 0;
    // }

    // this->head->next = this->tail;
    // this->tail->prev = this->head;

    // this->count = 0;


    this->removeInternalData();
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    // TODO
    this->checkIndex(index);
    return this->getPreviousNodeOf(index)->next->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // TODO
    Node* iNode = this->head->next;
    for (int i = 0; i < this->count; i++) {
        // if (this->itemEqual(item, iNode->data)) {
        //     return i;
        // }
        // else {
        //     if (item == iNode->data) {
        //     return i;
        //     }
        // }

        
        if constexpr (is_pointer<T>::value) {
            if (this->equals(iNode->data, item, this->itemEqual)) {
                return i;
            }
        }
        // ---------- Item IS NOT A POINTER ---------------
        if (item == iNode->data) {
            return i;
        }
        iNode = iNode->next;
    }
    
    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // TODO
    Node *current = this->head->next; // Start from the first actual node
    while (current != this->tail) {   // Traverse until we reach the tail
        if (equals(current->data, item, this->itemEqual)) { // Found the item
            // Remove the node from the list
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // Optional: Call the function to delete the item data
            if (removeItemData) {
                removeItemData(current->data);
            }

            delete current;  // Delete the node
            this->count--;    // Decrement the size of the list
            return true;      // Indicate successful removal
        }
        current = current->next; // Move to the next node
    }
    return false; // Item not found in the list
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // TODO
    return this->indexOf(item) != -1;

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
    stringstream resultString;
    resultString << "[";
    Node* currentNode = this->head->next;
    while (currentNode != this->tail) {
        if (item2str != nullptr) {
            resultString << item2str(currentNode->data);
        }
        else {
            resultString << currentNode->data;
        }
        if (currentNode->next != this->tail) {
            resultString << ", ";
        }
        currentNode = currentNode->next;
    }
    resultString << "]";
    return resultString.str();
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
    this->removeInternalData();


    // this->head = list->head;
    // this->tail = list->tail;
    //this->count = list.count;
    this->head = new Node(0, nullptr, nullptr);
    this->tail = new Node(0, nullptr, this->head);
    this->head->next = this->tail;

    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;

    if (list.count == 0) {
        return;
    }

    Node* iNode = list.head->next;
    while (iNode != list.tail) {
        this->add(iNode->data);
        iNode = iNode->next;
    }

}

// template <class T>
// void DLinkedList<T>::removeInternalData()
// {
//     /**
//      * Clears the internal data of the list by deleting all nodes and user-defined data.
//      * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
//      * Traverses and deletes each node between the head and tail to release memory.
//      */
//     // TODO
//     // this->deleteUserData(this);

//     //-------------24/09/2024-------------
//     if (this->deleteUserData != nullptr) {
//     this->deleteUserData(this);
//     }

//     // if (this->deleteUserData != nullptr) {
//     //     Node* current = this->head->next;
//     //     while (current != this->tail) {
//     //         this->deleteUserData(current->data); // Delete data in Node
//     //         current = current->next;
//     //     }
//     // }
//     //-------------24/09/2024--------------

//     Node* nodeToDelete = nullptr;
//     while (this->head->next != this->tail) {
//         nodeToDelete = this->head->next;
//         this->head->next = nodeToDelete->next;
//         //------------24/09/2024------------
//         if (nodeToDelete->next != nullptr) {
//             nodeToDelete->next->prev = this->head;
//         }
//         //------------24/09/2024------------
//         delete nodeToDelete;
//     }

//     this->count = 0;
// }

template <class T>
void DLinkedList<T>::removeInternalData()
{
    // Node* nodeToDelete = nullptr;

    // while (this->head->next != this->tail) {
    //     nodeToDelete = this->head->next;
    //     this->head->next = nodeToDelete->next;

    //     // Kiểm tra nếu dữ liệu là con trỏ, chỉ xóa nếu cần
    //     if constexpr (std::is_pointer<T>::value) {
    //         if (this->deleteUserData != nullptr) {
    //             this->deleteUserData(nodeToDelete->data);
    //         }
    //     }

    //     delete nodeToDelete;
    // }
    // cout << "Head"<< this->head->data << endl;
    // cout << "Tail"<< this->tail->data << endl;
    // Node* currentNode = this->head;
    // while(currentNode != NULL) {
    //     Node* temp = currentNode;
    //     cout << "current data: " << temp->data << endl;
    //     currentNode = currentNode->next;
    //     delete temp;
    //     this->println();
    // }
    if (this->count == 0)
    {
        return;
    }
    while(this->count > 0)
    {
        Node* temp = this->head;

        this->head = this->head->next;

        if (this->head != nullptr)
        {
            this->head->prev = nullptr;
        }

        // If data is a pointer 
        if constexpr (is_pointer<T>::value) {
            delete temp->data;
        }

        delete temp;
        --count;
    }


    // this->head = nullptr;
    // this->tail = nullptr;

    this->count = 0;
    
}



#endif /* DLINKEDLIST_H */
