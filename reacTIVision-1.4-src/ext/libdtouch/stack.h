/***************************************************************************
                          stack.h  -  description
                             -------------------
    begin                : Fri Nov 28 2003
    copyright            : (C) 2003 by Enrico Costanza
    email                : e.costanza@ieee.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330                           *
 *  Boston, MA  02111-1307  USA                                            *
 *                                                                         *
 ***************************************************************************/


/*  Changes

	Code optimization by Jorge M Santiago

*/
//
#ifndef EC_STACK
#define EC_STACK

#include "list.h"
#include "pool.h"

template <class T> class Stack
{
protected:
	ListItem<T> *_first;

	int _size;

public:
	// constuctors
	Stack() { _first = NULL; _size=0; }

	// destructor
	virtual ~Stack() { 
		//if(_first!=NULL){ delete _first; }
		ListItem<T> *current = _first;
		while( current != NULL ){
			ListItem<T> *tmp = current;
			current = current->getNext();
			delete tmp;
		}
		_first = NULL; 
		_size=0;
	}

	// i/o methods

	int getSize(void) { return _size; }
	//int size(void) { return _size; }

	virtual void push( const T &data ){
		ListItem<T> *tmp = new ListItem<T>(data, _first);
		_first = tmp;
		_size++;
	}

	virtual T pop(){
		T result = _first->getData();
		ListItem<T> *tmp = _first;
		_first = _first->getNext();
		tmp->setNext( NULL );
		delete tmp;
		_size--;
		return result;
	}

	bool isEmpty( void ){ return( _first == NULL ); }

	virtual ListItem<T> *getPtr() { return _first; }

};


//*
template <class T> 
class StackPoolDynamic : public Stack<T>
{
protected:
	using Stack<T>::_first;
	using Stack<T>::_size;
	DynamicPool<T> pool;

	//StackPoolDynamic() { }

public:
	// constuctors
	StackPoolDynamic(int in_size=1000) : Stack<T>(), pool(in_size) { }

	// destructor
	~StackPoolDynamic() { 
		ListItem<T> *current = _first;
		while( current != NULL ){
			ListItem<T> *tmp = current;
			current = current->getNext();
			delete tmp;
		}
		_first = NULL; 
		_size=0;
	}

	// i/o methods

	void push( const T &data ){
		ListItem<T> *tmp = pool.getElement();
		tmp->setData( data );
		tmp->setNext( _first );
		_first = tmp;
		_size++;
	}

	T pop(){
		T result = _first->getData();
		ListItem<T> *tmp = _first;
		_first = _first->getNext();
		pool.pushElement( tmp );
		//tmp->next = NULL;
		//delete tmp;
		_size--;
		return result;
	}

	bool isEmpty( void ){ return( _first == NULL ); }

	ListItem<T> *getPtr() { return _first; }

};
//*/


#endif
