/***************************************************************************
             region.cpp  -  class representing image regions
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

#include "region.h"

DTRegion::DTRegion(void)
{
}

/*
List<DTPoint> *DTRegion::getFirstComponent(){
	List<DTPoint> *result = new List<DTPoint>;
	List<DTPoint> *region = new List<DTPoint>( *((List<DTPoint> *)this) );
	
	region->reset();
	DTPoint start = region->getData();
	result->append(start);
	region->removeCurrent();

	DTPoint next;
	DTPoint curr = start;
	do{
		for( region->reset(); !(region->isNull()); region->fwd() ){
			next = region->getData();
			if( next.is8Neighbour(curr) ){
				result->append(next);
				curr = next;
				region->removeCurrent();
			}
		}
		for( region->goToLast(); !(region->isNull()); region->bwd() ){
			next = region->getData();
			if( next.is8Neighbour(curr) ){
				result->append(next);
				curr = next;
				region->removeCurrent();
				region->fwd();
			}
		}
	}while( !(start.is8Neighbour(curr)) );

	delete region;

	return result;
}


//warning! we assume the region is a rectangle!!
DTPoint *DTRegion::getCorners(){
	int minX = _first->getData().x;
	DTPoint minXPoint = _first->getData();
	int minXcount = 1;

	int minY = _first->getData().y;
	DTPoint minYPoint = _first->getData();
	int minYcount = 1;

	int maxX = _first->getData().x;
	DTPoint maxXPoint = _first->getData();
	int maxXcount = 1;

	int maxY = _first->getData().y;
	DTPoint maxYPoint = _first->getData();
	int maxYcount = 1;

	for( reset(); !isNull(); fwd() ){
		DTPoint curr = getData();
		if( curr.x < minX ){
			minX = curr.x;
			minXPoint = curr;
			minXcount = 1;
		}else if( curr.x == minX ){
			minXcount++;
		}

		if( curr.y < minY ){
			minY = curr.y;
			minYPoint = curr;
			minYcount = 1;
		}else if( curr.y == minY ){
			minYcount++;
		}

		if( curr.x > maxX ){
			maxX = curr.x;
			maxXPoint = curr;
			maxXcount = 1;
		}else if( curr.x == maxX ){
			maxXcount++;
		}

		if( curr.y > maxY ){
			maxY = curr.y;
			maxYPoint = curr;
			maxYcount = 1;
		}else if( curr.y == maxY ){
			maxYcount++;
		}
	}

	DTPoint *result = new DTPoint[4];

	result[0] = minXPoint;
	result[1] = minYPoint;
	result[2] = maxXPoint;
	result[3] = maxYPoint;

	return result;
}
//*/
