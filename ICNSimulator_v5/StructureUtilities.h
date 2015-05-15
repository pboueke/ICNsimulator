/*
 * StructureUtilities.h
 *
 *  Created on: 24/03/2015
 *      Author: Pedro H Boueke
 */

/* READ-ME
 * The compiler requires both the template definition and type you intend to use with it.
 * Usual approaches include the use o 'export' for compiling the .cpp file, or instantiating
 * a 'template class yourclass<yourtype>;' where it is being called. Those methods are not ideal,
 * you should define everything in the header.
 * For more info: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2003/n1426.pdf
*/

#ifndef STRUCUTUREUTILITIES_H_
#define STRUCUTUREUTILITIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

template<class T>
struct Element {
	//Element of a linked list, used in queue and stack data structures
	T index; //Index of the element
	Element* next; //Pointer to next element

	Element(T n_index); //Start element given index
};

template<class T>
struct Ordered_LinkedList {
	//Data structure representing an ordered linked list
	int size; //List's size
	Element<T>* first; //Pointer to first element
	Element<T>* last; //Pointer to last element

	Ordered_LinkedList(); //Default constructor
	~Ordered_LinkedList(); //Default destructor
	void Add(T index); //Adds element to list
	Element<T>* GetElementN(int index); //Returns element at nth positions
	T Remove_Head(); //Removes first element from list
	bool Remove(T index); //Removes specific element from list
	int Size(); //Gets size of list
};

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
    else if (*(e->index) < *(first->index)){
        //Element is new first
        e->next = first;
        first = e;
    }
    else{
        Element<T>* it_p = first; //Pointer to previous element
        for (Element<T>* it = first->next; it != NULL; it = it->next){
            //Iterator starts at first, ends at last-1 and next it is next element
            if (*(e->index) < *(it->index)){
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

template <class T>
int Ordered_LinkedList<T>::Size(){
	return size;
}

template <class T>
Element<T>* Ordered_LinkedList<T>::GetElementN(int index){
	    for (Element<T>* it = first; it != NULL; it = it->next){
	        if (it->index == index){
	        	return it;
	        }
	    }
	}



#endif // STRUCUTUREUTILITIES_H_


