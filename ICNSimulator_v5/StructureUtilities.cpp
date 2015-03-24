#include "StructureUtilities.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

template <class T>
Element<T>::Element(T n_index){
    index = n_index;
    next = NULL; //Can't set beforehand
}

template <class T>
Ordered_LinkedList<T>::Ordered_LinkedList(){
    //List starts empty
    size = 0;
    first = last = NULL;
}

template <class T>
Ordered_LinkedList<T>::~Ordered_LinkedList(){
    //Empties list
    while (size){
        Remove_Head();
    }
}

template <class T>
void Ordered_LinkedList<T>::Add(T index){
    //Adds new element to list, keeping the order it was in
    Element<T>* e = new Element<T>(index);
    if (!size){
        //List was empty
        first = e;
        last = e;
    }
    else if (e->index > first->index){
        //Element is new first
        e->next = first;
        first = e;
    }
    else{
        Element<T>* it_p = first; //Pointer to previous element
        for (Element<T>* it = first->next; it != NULL; it = it->next){
            //Iterator starts at first, ends at last-1 and next it is next element
            if (e->index > it->index){
                it_p->next = e;
                e->next = it;
                ++size;
                return;
            }
            it_p = it;
        }
        //Element is new last
        //Will never reach this point otherwise, because of conditional return
        last->next = e;
        last = e;
    }
    //Increments size for cases where there was no conditional return
    ++size;
}

template <class T>
T Ordered_LinkedList<T>::Remove_Head(){
    //Removes first element of list
    Element<T>* aux = first;
    T ind = first->index;
    first = first->next;
    delete aux;
    --size;
    return ind;
}

template <class T>
bool Ordered_LinkedList<T>::Remove(T index){
    //Removes specific element
    Element<T>* prev = NULL;
    for (Element<T>* it = first; it != NULL; it = it->next){
        //Iterates list looking for a specific key
        //If key has a duplicate, removes first iteration
        if (it->index == index){
            if (prev) prev->next = it->next;
            else first = it->next;
            delete it;
            --size;
            return true;
        }
        prev = it;
    }
    return false;
}
