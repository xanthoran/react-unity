/***************************************************************************
   fiducialdata.cpp  -  container class for fiducial recognition results
                             -------------------
    begin                : Sat Nov 27 2004
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

#include "fiducialdata.h"

using std::cout;
using std::endl;

FiducialData::FiducialData(void)
{
	memset(sequence,0,(maxBranches+1)*sizeof(int));
	memset(branchCentre,0,maxBranches*sizeof(int));
}

FiducialData::~FiducialData(void) {}

/*
FiducialData::FiducialData( const FiducialData& src ){
	sequence = new int[src.sequence[0]+1];
	memcpy(sequence,src.sequence,(src.sequence[0]+1)*sizeof(int));

	branchCentre = new DTPoint[src.sequence[0]];
	memcpy(branchCentre,src.branchCentre,sequence[0]*sizeof(DTPoint));

	memcpy(corner,src.corner,4*sizeof(DTPoint));

	angle = src.angle;
	centre = src.centre;
	height = src.height;
	rootRegion = src.rootRegion;
	type = src.type;
	width = src.width;
}

void FiducialData::operator=(FiducialData src)
{
	sequence = new int[src.sequence[0]+1];
	memcpy(sequence,src.sequence,(src.sequence[0]+1)*sizeof(int));

	branchCentre = new DTPoint[src.sequence[0]];
	memcpy(branchCentre,src.branchCentre,sequence[0]*sizeof(DTPoint));

	memcpy(corner,src.corner,4*sizeof(DTPoint));

	angle = src.angle;
	centre = src.centre;
	height = src.height;
	rootRegion = src.rootRegion;
	type = src.type;
	width = src.width;
}
//*/

void FiducialData::init( int *in_sequence )
{
	memcpy(sequence,in_sequence,(in_sequence[0]+1)*sizeof(int));
}

DTPoint FiducialData::getBranchCentre(const int& i){ 
	if( i>=0 && i<sequence[0] ){
		return branchCentre[i]; 
	}else{
		cout << "overrun!!" << endl;
		return DTPoint(-1,-1);
	}
}

void FiducialData::setBranchCentre(const int& i, const DTPoint& p){ 
	if( i>=0 && i<sequence[0] ){
		branchCentre[i] = p; 
	}else{
		cout << "overrun!!" << endl;
	}
}


DTPoint FiducialData::getCorner(const int& i){ 
	if( i>=0 && i<4 ){
		return corner[i]; 
	}else{
		cout << "overrun!!" << endl;
		return DTPoint(-1,-1);
	}
}

void FiducialData::setCorner(const int& i, const DTPoint& p){ 
	if( i>=0 && i<4 ){
		corner[i] = p; 
	}else{
		cout << "overrun!!" << endl;
	}
}

int FiducialData::getId() {
	id = fiducial_id_from_symbol_sequence();
	return id;
}

void FiducialData::setId(int in_id) {
	id=in_id;
}

int FiducialData::fiducial_id_from_symbol_sequence()
{
    const int length = 5;
    int id_seq[length];
    id_seq[0] = (int)(type/10000)-1;
    id_seq[1] = (int)((type - (1+id_seq[0])*10000)/1000)-1;
    id_seq[2] = (int)((type - (1+id_seq[0])*10000 - (1+id_seq[1])*1000)/100)-1;
    id_seq[3] = (int)((type - (1+id_seq[0])*10000 - (1+id_seq[1])*1000 - (1+id_seq[2])*100)/10)-1;
    id_seq[4] = (int)(type  - (1+id_seq[0])*10000 - (1+id_seq[1])*1000 - (1+id_seq[2])*100 - (1+id_seq[3])*10)-1;

    int id = 0;
    int factorial[8] = { 1, 1, 2, 6, 24, 120, 720, 5040 };
    int mapping[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    int i, j;

    for( i=1; i < length; ++i ){
        id += mapping[ (id_seq[i]-1) ] * factorial[(length-1) - i];
        for( j = (id_seq[i]-1) + 1; j < length; ++j ){
            mapping[j]--;
        }
    }

    return id;
}
