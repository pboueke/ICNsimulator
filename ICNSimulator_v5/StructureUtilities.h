#ifndef STRUCUTUREUTILITIES_H
#define STRUCUTUREUTILITIES_H

#define intptr int*
#include <string>

using namespace std;

template <class T>
struct Element{
    //Element of a linked list, used in queue and stack data structures
    T index; //Index of the element
    Element* next; //Pointer to next element

    Element(T n_index); //Start element given index
};

template <class T>
struct Ordered_LinkedList{
    //Data structure representing an ordered linked list
    int size; //List's size
    Element<T>* first; //Pointer to first element
    Element<T>* last; //Pointer to last element

    Ordered_LinkedList(); //Default constructor
    ~Ordered_LinkedList(); //Default destructor
    void Add(T index); //Adds element to list
    T Remove_Head(); //Removes first element from list
    bool Remove(T index); //Removes specific element from list
};

#endif STRUCUTUREUTILITIES_H
