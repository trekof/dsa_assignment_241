/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
#include "list/IList.h" //code goc 
// #include "IList.h"
#include <memory.h>
#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class XArrayList : public IList<T>
{
public:
    class Iterator; // forward declaration

protected:
    T *data;                                 // dynamic array to store the list's items
    int capacity;                            // size of the dynamic array
    int count;                               // number of items stored in the array
    bool (*itemEqual)(T &lhs, T &rhs);       // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(XArrayList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    XArrayList(
        void (*deleteUserData)(XArrayList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0,
        int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

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
    // Inherit from IList: BEGIN

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }

    /** free:
     * if T is pointer type:
     *     pass THE address of method "free" to XArrayList<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  XArrayList<Point*> list(&XArrayList<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(XArrayList<T> *list)
    {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

protected:
    void checkIndexForAdd(int index);     // check validity of index for accessing when adding an element
    void checkIndexForRemove(int index);
    void ensureCapacity(int index); // auto-allocate if needed

    /** equals:
     * if T: primitive type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     * if T: object type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     *      Therefore, class of type T MUST override operator ==
     * if T: pointer type:
     *      indexOf, contains: will use function pointer "itemEqual"
     *      to: compare two items of T type
     *      Therefore:
     *      (1): must pass itemEqual to the constructor of XArrayList
     *      (2): must define a method for comparing
     *           the content pointed by two pointers of type T
     *          See: definition of "equals" of class Point for more detail
     */
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    void copyFrom(const XArrayList<T> &list);

    void removeInternalData();

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Iterator: BEGIN
    class Iterator
    {
    private:
        int cursor;
        XArrayList<T> *pList;

    public:
        Iterator(XArrayList<T> *pList = 0, int index = 0)
        {
            this->pList = pList;
            this->cursor = index;
        }
        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            T item = pList->removeAt(cursor);
            if (removeItemData != 0)
                removeItemData(item);
            cursor -= 1; // MUST keep index of previous, for ++ later
        }

        T &operator*()
        {
            return pList->data[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->cursor++;
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
    // Iterator: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
void deleteUserData(XArrayList<T> *list) {
    // Duyệt qua từng phần tử trong danh sách và giải phóng từng con trỏ
    for (int i = 0; i < list->size(); i++) {
        delete list->get(i);  // Giải phóng đối tượng mà con trỏ đang trỏ tới
    }

}

template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),
    int capacity)
{
    // TODO
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;
    this->capacity = capacity;
    this->data = new T[this->capacity];
    this->count = 0;
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)
{
    /*
     * Copies the contents of another XArrayList into this list.
     * Initializes the list with the same capacity as the source list and copies all elements.
     * Also duplicates user-defined comparison and deletion functions, if applicable.
     */
    // TODO

    this->capacity = list.capacity;
    this->count = list.count;
    this->data = new T[this->capacity];
    for (int i = 0; i < list.count; i++) {
        this->data[i] = list.data[i];
    }

    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;
    
}

template <class T>
void XArrayList<T>::removeInternalData()
{
    /*
     * Clears the internal data of the list by deleting the dynamic array and any user-defined data.
     * If a custom deletion function is provided, it is used to free the stored elements.
     * Finally, the dynamic array itself is deallocated from memory.
     */
    // TODO
    // Trường hợp 1: Xử lý khi có con trỏ hàm deleteUserData
    if (this->deleteUserData != nullptr) {
        // Gọi hàm deleteUserData để giải phóng tài nguyên đặc biệt do người dùng định nghĩa
        this->deleteUserData(this);
    }

    // Trường hợp 2: Giải phóng dữ liệu động nếu có (áp dụng với kiểu dữ liệu con trỏ)
    if (this->data != nullptr) {
        delete[] this->data;  // Giải phóng bộ nhớ mảng động
        this->data = nullptr; // Đặt con trỏ về null để tránh trỏ tới vùng nhớ không hợp lệ
    }

    // Thiết lập lại trạng thái của đối tượng
    this->count = 0;
    this->capacity = 0;
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
    // TODO
    this->copyFrom(list);
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{
    // TODO
    if (this != &list) {
        // Giải phóng dữ liệu hiện tại trước khi sao chép
        this->removeInternalData();

        // Sao chép dữ liệu từ đối tượng khác
        // this->capacity = list.capacity;
        // this->count = list.count;
        // this->data = new T[this->capacity];
        // for (int i = 0; i < list.count; i++) {
        //     this->data[i] = list.data[i];
        // }

        // // Sao chép con trỏ hàm (nếu cần)
        // this->itemEqual = list.itemEqual;
        // this->deleteUserData = list.deleteUserData;

        this->copyFrom(list);
    }
    return *this;
}

template <class T>
XArrayList<T>::~XArrayList()
{
    // TODO
    this->removeInternalData();
}

template <class T>
void XArrayList<T>::add(T e)
{
    // TODO
    this->ensureCapacity(this->count + 1);

    this->data[this->count] = e;
    this->count = this->count + 1;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
    // TODO
    this->ensureCapacity(this->count + 1);
    this->checkIndexForAdd(index);

    int moveCount = this->count - index;
    if (moveCount > 0) {
        memmove(this->data + index + 1, this->data + index, moveCount * sizeof(T));
    }
    this->data[index] = e;
    this->count = this->count + 1;
}

template <class T>
T XArrayList<T>::removeAt(int index)
{
    // TODO
    this->checkIndexForRemove(index);

    T elementToRemove = this->data[index];
    int moveCount = this->count - index - 1;
    if (moveCount > 0) {
        memmove(this->data + index, this->data + index + 1, moveCount * sizeof(T));
    }
    this->count = this->count - 1;
    return elementToRemove;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // TODO
    // for (int index = 0; index < this->count; index++) {
    //     if (this->data[index] == item) {
    //         this->removeAt(index);
    //         return true;
    //     }
    // }
    // return false;

    // Duyệt qua tất cả các phần tử trong danh sách
    for (int index = 0; index < this->count; index++) {
        // So sánh phần tử hiện tại với phần tử cần xoá
        if (equals(this->data[index], item, this->itemEqual)) {
            // Nếu cần, gọi hàm để xoá dữ liệu của phần tử (nếu là con trỏ)
            if (removeItemData != nullptr) {
                removeItemData(this->data[index]);
            }

            // Gọi hàm removeAt để xoá phần tử khỏi danh sách
            this->removeAt(index);
            return true;  // Đã xoá phần tử đầu tiên tìm thấy
        }
    }
    return false;  // Không tìm thấy phần tử cần xoá
}

template <class T>
bool XArrayList<T>::empty()
{
    // TODO
    return this->count == 0;
}

template <class T>
int XArrayList<T>::size()
{
    // TODO
    return this->count;
}

template <class T>
void XArrayList<T>::clear()
{
    // TODO
    // delete[] this->data;
    this->removeInternalData();

    this->capacity = 10;
    this->data = new T[this->capacity];
    this->count = 0;

}

template <class T>
T &XArrayList<T>::get(int index)
{
    // TODO
    this->checkIndexForRemove(index);
    return this->data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
    // TODO
    for (int index = 0; index < this->count; index++) {
        if (item == this->data[index]) {
            return index;
        }
    }
    return -1;
}
template <class T>
bool XArrayList<T>::contains(T item)
{
    // TODO
    return (this->indexOf(item) != -1);

}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the array list into a string representation, formatting each element using a user-defined function.
     * If no function is provided, it uses the default string representation for each element.
     * Example: Given an array list with elements {1, 2, 3} and a function that converts integers to strings, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function pointer for converting items of type T to strings. If null, default to the string conversion of T.
     * @return A string representation of the array list with elements separated by commas and enclosed in square brackets.
     */

    // TODO
    stringstream ss;  // Sử dụng stringstream để xây dựng chuỗi kết quả
    ss << "[";             // Bắt đầu chuỗi với dấu ngoặc vuông mở

    for (int i = 0; i < this->count; i++) {
        // Lấy phần tử hiện tại
        T& item = this->data[i];

        // Kiểm tra nếu có hàm chuyển đổi item2str được cung cấp
        if (item2str != nullptr) {
            ss << item2str(item);  // Sử dụng hàm item2str để chuyển đổi
        } else {
            // Nếu không có item2str, sử dụng cách chuyển đổi mặc định
            ss << item;  // Sử dụng toán tử << để chuyển đổi
        }

        // Nếu không phải phần tử cuối cùng, thêm dấu phẩy và khoảng trắng
        if (i < this->count - 1) {
            ss << ", ";
        }
    }

    ss << "]";  // Kết thúc chuỗi với dấu ngoặc vuông đóng
    return ss.str();  // Trả về chuỗi kết quả

}



//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
void XArrayList<T>::checkIndexForAdd(int index)
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
void XArrayList<T>::checkIndexForRemove(int index) {
  /**
   * Validates whether the given index is within the valid range of the list.
   * Throws an std::out_of_range exception if the index is negative or exceeds
   * the number of elements. Ensures safe access to the list's elements by
   * preventing invalid index operations.
   */
  // TODO implement
  if (index < 0 || index > this->count - 1) {
        throw std::out_of_range("Index is out of range!");
  }
}

template <class T>
void XArrayList<T>::ensureCapacity(int index)
{
    /**
     * Ensures that the list has enough capacity to accommodate the given index.
     * If the index is out of range, it throws an std::out_of_range exception. If the index exceeds the current capacity,
     * reallocates the internal array with increased capacity, copying the existing elements to the new array.
     * In case of memory allocation failure, catches std::bad_alloc.
     */
    // TODO
    if (index >= this->capacity) {
        int newCapacity = this->capacity * 3/2 + 1;             //increase capacity
        T *newData = new T [newCapacity];                       //creare new arraylist of data with larger new capacity
        memmove(newData, this->data, this->count * sizeof(T));  //move data to new arraylist of data
        this->capacity = newCapacity;                           //update capacity
        delete[] this->data;                                    //delete data in old array list of data
        this->data = newData;                                   //update old data with new data
    }
}

#endif /* XARRAYLIST_H */
