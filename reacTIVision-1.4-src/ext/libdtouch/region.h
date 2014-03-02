/***************************************************************************
               region.h  -  class representing image regions
                             -------------------
    begin                : Wed Dec 01 2004
    copyright            : (C) 2004 by Enrico Costanza
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

#ifndef EC_REGION
#define EC_REGION

#include "listpool.h"
#include "types.h"

class DTRegion : public ListPool<DTPoint>
{
	Pool<int *> *_labelPtrPool;
	ListPool<int *> *_labelPtrList;

	bool _labelPtrListAllocated;
	bool _labelPtrPoolAllocated;

	DTRegion(void);
public:
	bool _white;
	int _noLeaves;
	bool _potentialRoot;

	DTRegion( Pool<DTPoint> *in_pointPool ) : ListPool<DTPoint>(in_pointPool)
	{ 
		_white = _potentialRoot=false; 
		_noLeaves = 0;
		_labelPtrPool = new DynamicPool<int *>(300);
		_labelPtrPoolAllocated = true;
		_labelPtrList = new ListPool<int *>( _labelPtrPool );
		_labelPtrListAllocated = true;
	}

	DTRegion( Pool<DTPoint> *in_pointPool,
		Pool<int *> *in_labelPtrPool) : ListPool<DTPoint>(in_pointPool)
	{ 
		_white = _potentialRoot=false; 
		_noLeaves = 0;
		_labelPtrPool = in_labelPtrPool;
		_labelPtrPoolAllocated = false;
		_labelPtrList = new ListPool<int *>( _labelPtrPool );
		_labelPtrListAllocated = true;
	}

	DTRegion( Pool<DTPoint> *in_pointPool, DynamicPool<int *> *in_labelPtrPool, 
		ListPool<int *> *in_labelPtrList) : ListPool<DTPoint>(in_pointPool)
	{ 
		_white = _potentialRoot=false; 
		_noLeaves = 0;
		_labelPtrPool = in_labelPtrPool;
		_labelPtrPoolAllocated = false;
		_labelPtrList = in_labelPtrList;
		_labelPtrListAllocated = false;
	}

	DTRegion( Pool<DTPoint> *in_pointPool, ListPool<int *> *in_labelPtrList) 
		: ListPool<DTPoint>(in_pointPool)
	{ 
		_white = _potentialRoot=false; 
		_noLeaves = 0;
		_labelPtrPool = NULL;
		_labelPtrPoolAllocated = false;
		_labelPtrList = in_labelPtrList;
		_labelPtrListAllocated = false;
	}

	~DTRegion(void)
	{
		_white = false;
		_noLeaves = 0;
		if( _labelPtrPoolAllocated ){
			delete _labelPtrPool;
		}
		if( _labelPtrListAllocated ){
			delete _labelPtrList;
		}else{
			_labelPtrList->empty();
		}
	}

	/*inline*/ void empty(){
		if( _first != NULL ){
			pool->pushList(_first, _last,_size);
			_first = _last = _current = NULL;
			_white = _potentialRoot = false;
			_noLeaves = 0;
			_size = 0;
			_labelPtrList->empty();
		}
	}

	bool isLabelPtrListNull( ){ return _labelPtrList->isNull(); }
	void emptyLabelPtrList( ){ _labelPtrList->empty(); }
	void appendToLabelPtrList( int *data ){ _labelPtrList->append(data); }
	void appendToLabelPtrList( ListPool<int *> *data ){ _labelPtrList->append(data); }
	void resetLabelPtrList(){ _labelPtrList->reset(); }
	void fwdLabelPtrList(){ _labelPtrList->fwd(); }
	ListPool<int *> *getLabelPtrList( ){ return _labelPtrList; }

	int *getLabelPtrListData( ){ return _labelPtrList->getData(); }
	
	//List<DTPoint> *getFirstComponent();
	//DTPoint *getCorners();
};

#endif
