/***************************************************************************
     fiducialdata.h  -  container class for fiducial recognition results
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

#ifndef EC_FIDUCIALDATA
#define EC_FIDUCIALDATA

#include <string.h>
#include "point.h"
#include "list.h"

class FiducialData
{
protected:
	const static int maxBranches = 10;
	int id;
	int type;
	int sequence[maxBranches+1];
	
	DTPoint centre;
	int width;
	int height;

	int angle;
	//double angled;
	
	int rootRegion;
	
	//DTPoint v1;
	//DTPoint v2;

	DTPoint branchCentre[maxBranches];

	DTPoint corner[4];

public:
	FiducialData(void);
	//FiducialData( const FiducialData& );
	~FiducialData(void);
	void init( int *in_sequence );
	//void operator=(FiducialData src);


	int getId();
	void setId(int in_id);

	inline int getType(){ return type; }
	inline void setType(const int& in_type){ type = in_type; }
	inline int& Type(){ return type; }

	inline DTPoint getCentre(){ return centre; }
	inline void setCentre(const DTPoint& in_centre){ centre = in_centre; }
	inline DTPoint& Centre(){ return centre; }
	
	inline int getWidth(){ return width; }
	inline void setWidth(const int& in_width){ width = in_width; }
	inline int& Width(){ return width; }

	inline int getHeight(){ return height; }
	inline void setHeight(const int& in_height){ height = in_height; }
	inline int& Height(){ return height; }

	inline int getAngle(){ return angle; }
	inline void setAngle(const int& in_angle){ angle = in_angle; }
	inline int& Angle(){ return angle; }
	
	inline int getRootRegion(){ return rootRegion; }
	inline void setRootRegion(const int& in_rootRegion){ rootRegion = in_rootRegion; }
	inline int& RootRegion(){ return rootRegion; }

	DTPoint getBranchCentre(const int& i);//{ return branchCentre[i]; }
	void setBranchCentre(const int& i, const DTPoint& p);//{ branchCentre[i] = p; }
	//DTPoint& BranchCentre(const int& i){ return branchCentre[i]; }

	DTPoint getCorner(const int& i);//{ return corner[i]; }
	void setCorner(const int& i, const DTPoint& p);//{ corner[i] = p; }
	//DTPoint& Corner(const int& i){ return corner[i]; }

private:
	int fiducial_id_from_symbol_sequence();
};

//typedef List<FiducialData> FiducialDataList;

#endif
