/***************************************************************************
          types.h  -  defines data types used in fiducial recognition
                      no implementation file
                             -------------------
    begin                : Wed Sep 18 2002
    copyright            : (C) 2002 by Enrico Costanza
    email                : ec142@york.ac.uk
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

#ifndef EC_TYPES
#define EC_TYPES

#include "listpool.h"
#include "point.h"
#include  "fiducialdata.h"

//typedef ListPool<DTPoint> Region;

/*
typedef struct{
	int type;
	DTPoint centre;
	int angle;
	double angled;
	int rootRegion;
	DTPoint v1;
	DTPoint v2;
	DTPoint branchCentre[6];

	int width;
	int height;

	DTPoint corner[4];
} FiducialData;
//*/

//#include <deque>

//define new types based on the List<T> template
typedef List<int> IntList;
typedef List<class DTPoint> PointList;

//typedef std::deque<DTPoint> Region;

//typedef Matrix<int> IntMatrix;
//typedef Matrix<bool> BoolMatrix;

#endif

