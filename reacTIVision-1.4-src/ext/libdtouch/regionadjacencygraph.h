/***************************************************************************
                          regionadjacencygraph.h  -  description
                             -------------------
    begin                : Wed Nov 26 2003
    copyright            : (C) 2003 by Enrico Costanza
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

#ifndef EC_REGIONADJACENCYGRAPH
#define EC_REGIONADJACENCYGRAPH

#include "region.h"
#include "types.h"
#include "stack.h"
#include "listhash.h"

DTPoint getRegionCentre( DTRegion *reg );

class RAGBuilder;

class RegionAdjacencyGraph{
protected:
	int _width;
	int _height;
	int _size;
	int _used;


	StaticPool<DTPoint> *_regionsPool;
	ListHash<int> **_edges;
	DTRegion **_region;

	int **_labelsMap;
	int *_labels;
	int _usedLabelPtrs;
	StackPoolDynamic<int> _availableLabels;

	bool leavesMarked;
  
	void expand();

	RegionAdjacencyGraph() {}
public:

	RegionAdjacencyGraph( int in_width, int in_height, int in_size,
		StaticPool<DTPoint> *in_regionsPool, ListHash<int> **in_edges, DTRegion **in_region,
		int **in_labelsMap, int *in_labels );

	~RegionAdjacencyGraph();

	void link( int node1, int node2 ){ _edges[node1]->append(node2); _edges[node2]->append(node1); } 

	bool check( int node1, int node2 ) {
		if(_edges[node1]->getSize() > _edges[node2]->getSize()){
			return _edges[node2]->dtcheck(node1);
		}else{
			return _edges[node1]->dtcheck(node2);
		}
	}

	int merge( int node1, int node2 );

	void appendToRegion(DTPoint p, int region) { _region[region]->append(p); }

	DTRegion *getRegion( int i ){
		if( (i<0) || (i>=_used) ){ return NULL; }
		return _region[i];
	}

	void setWhiteRegion( int i ){ _region[i]->_white = true; }

	int getSize() { return _size; }
	ListHash<int> **getEdges() { return _edges; }
	DTRegion **getRegions() { return _region; }

	int getNoRegions(){ return _used; }
	int getDegree( int i ) { return _edges[i]->getSize(); }
	int getWhite( int i ) { return _region[i]->_white; }
	int getNoLeaves( int i ) { return _region[i]->_noLeaves; }

	ListHash<int> *getEdges(int i){ return _edges[i]; }

	int *getNewLabel(){
		if( _availableLabels.isEmpty() ){
			if( _used >= _size - 1 ){
				// expand graph
				expand();
			}
			_labels[_usedLabelPtrs] = _used++;
			return _labels + _usedLabelPtrs++;;
		}
		_labels[_usedLabelPtrs] = _availableLabels.pop();
		return _labels + _usedLabelPtrs++;
	}

	int *findFiducialVectorLeaves( const int *connections, int tolerance );
	int *findFiducialRootDown( const int *connections, int tolerance );
	void markLeavesAndBranches();
	void markLeaves();

	int* findBranches( const int root, const int *connections );
	int* findBranchesGeneric( const int root, const int *connections );

  
	void decodeFiducialLinear( int root, int *fiducialSequence, FiducialData &fd );
	void decodeFiducial4( int root, int *fiducialSequence, FiducialData &fd );
	void decodeFiducial5( int root, int *fiducialSequence, FiducialData &fd );
	void decodeFiducial6( int root, int *fiducialSequence, FiducialData &fd );
	DTPoint decodeFiducialGeneric( int root, int *fiducialSequence, int &type, int &angle );

	int findFiducials( int *fiducialSequence, FiducialData **fiducialData, bool usingLinearDecoding=true );




	friend class RAGBuilder;
};


#endif

