/***************************************************************************
                          pool.h  -  description
                             -------------------
    begin                : Tue Nov 25 2003
    copyright            : (C) 2003 by Enrico Costanza
    email                : enrico@localhost.localdomain
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
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

#ifndef EC_POOL
#define EC_POOL

#include "list.h"


template <class T>
class Pool
{
protected:
	ListItem<T> *_first;
	int _size;

	Pool() {}
public:
	// constuctor
	Pool( int in_size ) : _size(in_size){
		ListItem<T> *ptr = new ListItem<T>;
		_first = ptr;
		for(int i=1; i<_size; i++){
			ptr->setNext( new ListItem<T> );
			ptr = ptr->getNext();
		}
	}

	// destructor
	virtual ~Pool() { 
		//if(_first!=NULL) { delete _first; }
		ListItem<T> *current = _first;
		//_size;
		while( current != NULL ){
			ListItem<T> *tmp = current;
			current = current->getNext();
			delete tmp;
			_size--;
		}
		_first = NULL;
		_size = 0;
	}

	virtual ListItem<T> *getElement() = 0;
	virtual void pushElement( ListItem<T> *element ) = 0;
	virtual void pushList( ListItem<T> *firstElement, ListItem<T> *lastElement, const int& in_size ) = 0;
};

template <class T>
class StaticPool : public Pool<T>
{
private:
	using Pool<T>::_first;
	using Pool<T>::_size;
	
	//ListItem<T> *_getElementResult;
public:
	StaticPool( int in_size ) : Pool<T>(in_size) { }

	inline ListItem<T> *getElement(){
		ListItem<T> *result = _first;
		_first = _first->getNext();
		result->setNext( NULL );
		#ifdef _DEBUG
		_size--;
		#endif
		return result;
	}

	void pushElement( ListItem<T> *element ){
		element->setNext( _first );
		_first = element;
		#ifdef _DEBUG
		_size++;
		#endif
	}

	void pushList( ListItem<T> *firstElement, ListItem<T> *lastElement, const int& in_size ){
		lastElement->setNext( _first );
		_first = firstElement;
		#ifdef _DEBUG
		_size+=in_size;
		#endif
	}
};

template <class T>
class DynamicPool : public Pool<T>
{
protected:
	using Pool<T>::_first;
	using Pool<T>::_size;
	
	int _available;
public:
	DynamicPool(int in_size) : Pool<T>(in_size), _available(in_size) { }

	inline ListItem<T> *getElement(){
		if( _available < 1 ){
			ListItem<T> *ptr = new ListItem<T>;
			_first = ptr;
			for(int i=1; i<_size; i++){
				ptr->setNext( new ListItem<T> );
				ptr = ptr->getNext();
			}
 			_available = _size;
			_size *= 2;
		}
		ListItem<T> *result = _first;
		_first = _first->getNext();
		_available--;
		result->setNext( NULL );
		return result;
	}

	void pushElement( ListItem<T> *element ){
		element->setNext( _first );
		_first = element;
		_available++;
	}

	void pushList( ListItem<T> *firstElement, ListItem<T> *lastElement, const int& in_size ){
		lastElement->setNext( _first );
		_first = firstElement;
		_available+=in_size;
	}

};


#endif

