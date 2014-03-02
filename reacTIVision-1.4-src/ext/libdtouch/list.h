/***************************************************************************
          list.h  -  simple linked list defined with a template
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Enrico Costanza
    email                : e.costanza@ieee.org
 ***************************************************************************/


/***************************************************************************
 *                                                                         *
 *	This program is free software; you can redistribute it and/or modify   *
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
 
#ifndef EC_LIST
#define EC_LIST
#define EC_LIST_GRAPH

#include <iostream>
#include <fstream>

using namespace std;

#include <assert.h>

// warning: the template definition seems to be different on visual c++ from Linux
// (not 100% sure)
template<class T> class List;

template <class T> class ListItem{
private:
   ListItem *next;
   T data;

   //experimental: use at your own risk!
   ListItem *prev;

public:
	inline T getData(){
		//assert(deleted==false);
		return data;
	}
   
	inline void setData(T const &d){
		//assert(deleted==false);
		data = d;
	}

	inline ListItem<T> *getNext(){
		//assert(deleted==false);
		return next;
	}

	inline void setNext( ListItem<T> *n ){
		//assert(deleted==false);
		next = n;
	}

	inline ListItem<T> *getPrev(){
		//assert(deleted==false);
		return prev;
	}

	inline void setPrev( ListItem<T> *p ){
		//assert(deleted==false);
		prev = p;
	}
   //constructors
   ListItem() { next=NULL; /*deleted=false;*/ }
   ListItem( T in_data) { next=NULL; data=in_data; /*deleted=false;*/ }
   ListItem( T in_data, ListItem *in_next ) { next=in_next; data=in_data; /*deleted=false;*/ }

   ~ListItem() { 
	   //if(next!=NULL) delete next; next=NULL; 
   }

	//bool deleted;

	/*
	void deleteCheck() {
		if( NULL!=next && false==next->deleted ){
			next->deleteCheck();
		}
		deleted = true;
	}
	//*/

   friend class List<T>;   

};

template <class T> class List
{
protected:
	ListItem<T> *_first;
	ListItem<T> *_current;
	ListItem<T> *_last;

	int _size;

	// moveTo moves the current pointer to the first element in the List with value equal to target
	bool moveTo( const T target ){
		if( _first == NULL ){ return false; }
		_current = _first;
		while( _current != NULL )
		{
			if( _current->data == target ){ return true; }
			_current = _current->next;
		}
		return false;
	}

	// seek returns the pointer to the first element in the List with value equal to target
	ListItem<T> *seek( const T target ){
		if( _first == NULL ){ return NULL; }
		ListItem<T> *ptr = _first;
		while( ptr != NULL ){
			if( ptr->data == target ){ return ptr; }
			ptr = ptr->next;
		}
		return NULL;
	}


public:
	// constuctors


	List() { _current = _first = _last = NULL; _size=0; }

	// copy constructor
	List( const List<T>& source );

	// destructor
	virtual ~List() {
		//if(_first!=NULL) delete _first; 
		_current = _first;
		while( _current != NULL ){
			ListItem<T> *tmp = _current;
			_current = _current->next;
			delete tmp;
		}
		_current = _first = _last = NULL; 
		_size=0;
	}

	virtual void empty() { 
		//if(_first!=NULL) delete _first; 
		_current = _first;
		while( _current != NULL ){
			ListItem<T> *tmp = _current;
			_current = _current->next;
			delete tmp;
		}
		_current = _first = _last = NULL; 
		_size=0;
	}

	// i/o methods

	//void expand() { }
	inline int getSize(void) { return _size; }
	//int size(void) { return _size; }

	inline T getData(void){
		return( _current->data ); 
		//if(_current!=NULL){ return( _current->data ); }
		//else{ throw("\n\ngetData() called on a NULL pointer!!!\n\n"); }
	}
	
	T getFirstData(void){
		return( _first->data );
		//if(_current!=NULL){ return( _current->data ); }
		//else{ throw("\n\ngetData() called on a NULL pointer!!!\n\n"); }
	}

	inline void reset(void){ _current = _first; }

	inline void fwd(void){
		if(_current != NULL){ _current = _current->next; }
	}

	T fwdGet(void){
		T result =_current->data; _current = _current->next; return result;
		//if(_current!=NULL){ _current = _current->next; return _current->data;  }
		//else{ throw("\n\nfwdGet() called on a NULL pointer!!!\n\n"); }
	}

	inline bool nullTest( void ) const {
			return( _current == NULL ); }
	inline bool isNull( void ) const {
			return( _current == NULL ); }
	inline bool isEmpty( void ){ return( _first == NULL ); }

	virtual inline void append( const T &in_data ){
		if( _first == NULL ){
			_first = new ListItem<T>(in_data);
			_current = _first;

			_last = _first;
		}
		else{
			_last->next = new ListItem<T>( in_data, NULL );
			//if( _last->next == NULL ){ throw( "\n\nList<T>::append: allocation failed!\n\n" ); }
			_last = _last->next;
			// not too sure about the following
			//_current = _last;
		}
		_size++;
		return;
	}

	virtual inline void append( List<T> *src ){
		if( src->isEmpty() ){
			// do nothing
			return;
		}
		if( _first == NULL ){
			_first = src->_first;
			_current = _first;
			_last = src->_last;

		}
		else{
			_last->next = src->_first;
			_last = src->_last;
			// not too sure about the following
			//_current = _last;
		}
		_size += src->_size;

		src->_size = 0;
		src->_first = NULL;
		src->_current = NULL;
		src->_last = NULL;
		return;
	}

	virtual void remove( const T &data ){
		if( _first == NULL ) { return; }

		if( _first->data == data ){
			ListItem<T> *toBeDeleted = _first;
			_first = _first->next;
			toBeDeleted->next = NULL;
			if( _current == toBeDeleted ){ _current = _first; }
			if( _last == toBeDeleted ){
				_last = _first;
			}
			delete toBeDeleted;
			_size--;
			return;
		}

		ListItem<T> *prev = _first;
		ListItem<T> *toBeDeleted = _first->next;

		while( toBeDeleted!=NULL ){
			if( toBeDeleted->data == data ){
				prev->next = toBeDeleted->next;
				toBeDeleted->next = NULL;
				if( _current == toBeDeleted ){ _current = prev; }
				if( _last == toBeDeleted ){
					_last = prev;
				}
				delete toBeDeleted;
				_size--;
				return;
			}

			prev = toBeDeleted;
			toBeDeleted = toBeDeleted->next;
		}
		return;
	}

	inline void replace( const T &data_in, const T &data_out ){
		ListItem<T> *ptr = seek(data_out);
		// this is checked externally
		if( ptr!= NULL ){
			ptr->data = data_in;
		}
	}

	inline bool dtcheck( const T &target ){
		if( _last!=NULL && _last->data == target ){ return true; }
		ListItem<T> *ptr = _first;
		while( ptr != _last ){
			if( ptr->data == target ){ return true; }
			ptr = ptr->next;
		}
		return false;
	}



	void copy( const List<T> &source );

	List<T>& operator = ( const List<T> &source) {
		cout << "list = operator ovld called\n";
		_current = _first = NULL;
		_size = source._size;

		ListItem<T> *tmp;
		for( tmp = source._first; tmp!=NULL; tmp=tmp->next ) {
			this->append( tmp->getData() );
		}
		return *this;
	}

};


template <class T>
List<T>::List( const List<T>& source ) {
	cout << "list copy constructor called" << endl;
	_current = _first = NULL;
	_size = source._size;

	ListItem<T> *tmpItem;
	for( tmpItem = source._first; tmpItem!=NULL; tmpItem=tmpItem->next ) {
		this->append( tmpItem->getData() );
	}
}

template <class T>
void List<T>::copy( const List<T> &source )
{
	cout << "copy method called!\n";
	cout.flush();
	_current = _first = NULL;
	_size = source._size;

	for( source.reset(); source.nullTest(); source.fwd() ) {
		this->append( source.getData() );
	}
}


#endif


