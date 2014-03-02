/***************************************************************************
                         regionadjacencygraph.cpp  -  description
                             -------------------
    begin                : Wed Nov 26 2003
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
 ***************************************************************************/

#include "regionadjacencygraph.h"
#include "listhash.h"
#include <string.h>

using std::cout;
using std::endl;
using std::cerr;

int collinear( const DTPoint &a, const DTPoint &b, const DTPoint &c ){
  return abs(a.x * b.y + b.x * c.y + c.x * a.y - c.x * b.y - b.x * a.y - a.x * c.y);
}

int factorial( int n ){
  int result = 1;
  while( n>1 ){
    result *= n--;
  }
  return result;
}

DTPoint getRegionCentre( DTRegion *reg ){
	DTPoint result(0,0);
	for(reg->reset();!(reg->isNull());reg->fwd()){
		result += reg->getData();
	}

  result /= reg->getSize();
	return result;
}

DTPoint getRegionCentreRound( DTRegion *reg ){
	DTPoint result(0,0);
	for(reg->reset();!(reg->isNull());reg->fwd()){
		result += reg->getData();
	}

	double x = (double) result.x;
	double y = (double) result.y;
	//result.x = (int) round( x / (double) reg->getSize() );
	result.x = (int) floor( 0.5 + (x / (double) reg->getSize()) );
	//result.y = (int) round( y / (double) reg->getSize() );
	result.y = (int) floor( 0.5+ (y / (double) reg->getSize()) );
	return result;
}


using std::cout;
using std::endl;

RegionAdjacencyGraph::RegionAdjacencyGraph( int in_width, int in_height,
	int in_size, StaticPool<DTPoint> *in_regionsPool, ListHash<int> **in_edges,
	DTRegion **in_region, int **in_labelsMap, int *in_labels )	:
  _width (in_width), _height (in_height),
	_size(in_size), _regionsPool(in_regionsPool), _edges(in_edges),
	_region(in_region), _labelsMap(in_labelsMap), _labels(in_labels) {

	_used = 0;

	_usedLabelPtrs = 0;
	memset(_labels,0,_width*_height*sizeof(int *));

	/*for(int i=0;i<_size;i++){
		//_edges[i] = new List<int>;
		//_edges[i] = new ListHash<int>(_size);
		//_region[i] = new DTRegion(_regionsPool);
	}*/

	leavesMarked = false;
}

void RegionAdjacencyGraph::expand(void){

	int oldSize = _size;
	_size *= 2;

	//ListHash<int> **tmpEdges = new (ListHash<int> *)[_size];
	ListHash<int> **tmpEdges = new ListHash<int>*[_size];

	for(int i=0;i<oldSize;i++){
		tmpEdges[i] = _edges[i];
		_edges[i]->expand();
	}
	delete [] _edges;
	_edges = tmpEdges;

	//DTRegion **tmpRegion = new (DTRegion *)[_size];
	DTRegion **tmpRegion = new DTRegion*[_size];

	for(int i=0;i<oldSize;i++){
		tmpRegion[i] = _region[i];
	}
	delete [] _region;
	_region = tmpRegion;

	for(int i=oldSize;i<_size;i++){
		//_edges[i] = new List<int>;
		_edges[i] = new ListHash<int>(_size);
		_region[i] = new DTRegion(_regionsPool);
	}
}

RegionAdjacencyGraph::~RegionAdjacencyGraph(){
	// TO DO: reset edges and region!!!

 	for(int i=0;i<_size;i++){
		_edges[i]->empty();
		_region[i]->empty();
		//delete _region[i];
	}

	/*
 	for(int i=0;i<_size;i++){
		for(_edges[i]->reset();!_edges[i]->isNull();_edges[i]->fwd()){
			_edges[i]->getData();
		}
		for(_region[i]->reset();!_region[i]->isNull();_region[i]->fwd()){
			_region[i]->getData();
		}
		
	}
	//*/

	//delete [] _labels;
	//delete _edges;
	//delete _region;
	//delete _labelsMap;
}

int RegionAdjacencyGraph::merge(int node1, int node2){
	int src, dest;
	int result;

	if( _region[node1]->isEmpty() || _region[node2]->isEmpty()  ){
		return 0;
	}

	// set the source to be the node with smaller degree
	if( _edges[node1]->getSize() > _edges[node2]->getSize() ){
		src = node2;
		dest = node1;
		result = 1;
	}else{
		src = node1;
		dest = node2;
		result = 2;
	}

	#ifdef _DEBUG
	int dbgCounter=0;
	#endif
	// merge the edges of source in the edges of destination
	for( _edges[src]->reset(); !(_edges[src]->nullTest()); _edges[src]->fwd() ){
		int currentEdge = _edges[src]->getData();

		// if the current edge is already in dest
		if( _edges[dest]->dtcheck( currentEdge ) ){
			// just remove it from the target node
			_edges[currentEdge]->remove(src);
		}else{
			_edges[dest]->append(currentEdge);
			_edges[currentEdge]->replace(dest,src);
		}
		#ifdef _DEBUG
		dbgCounter++;
		#endif
	}

	// make the source edges list empty!
	_edges[src]->empty();

	// change the labels of source
	// this actually changes the pointers 
	for( _region[src]->resetLabelPtrList();
		!(_region[src]->isLabelPtrListNull()); _region[src]->fwdLabelPtrList() ){
		*(_region[src]->getLabelPtrListData()) = dest;
	}

	_region[dest]->appendToLabelPtrList( (_region[src]->getLabelPtrList()) );
	_region[src]->emptyLabelPtrList();
	
	// add the pixels of source to the destination list
	_region[dest]->append(_region[src]);

	// add the source label to the stack of available labels
	_availableLabels.push( src );

	return result;
}



/*****************************************
 *****************************************
 **                                     **
 **   Methods for finding fiducials     **
 **   (several different versions)      **
 **                                     **
 *****************************************
 *****************************************/
 
 

int *RegionAdjacencyGraph::findFiducialVectorLeaves( const int *connections, int tolerance )
/*connections is an array that rapresents a small graph, it has only 2 levels,
the first element is the "root" it indicates the number of subnodes that it has,
so it is also the size of the array
the following (connections[0]) elements indicates the number of connections that each node has,
but the subconnections are not included in this rapresentation
In this way the confrontation can (and have to) stop at the second step
*/
{
	int *result;
	result = new int[1];

	result[0] = 0;
	int *connectionsCopy = new int[connections[0] + 2];
	connectionsCopy[0] = connections[0] + 1;
	connectionsCopy[1] = -1;

  //the counter has as many elements as the subnodes are
	int *chkCounter = new int[ connectionsCopy[0] ];

	int counter = 0;		//debug code

	//for&if combination: look among the BLACK nodes
	for( int i=0; i < _used; i++ ){
		if( !_region[i]->_white ){

			if( _edges[i]->getSize() == connectionsCopy[0] ){
				//this might be the external black region of the fiducial

				//debug line
				counter++;

				memcpy(connectionsCopy+2,connections+1,connections[0]*sizeof(int));
				memset(chkCounter,0,connectionsCopy[0]*sizeof(int));

				//cout << "looking for this sequence: ";
				//for( int i3=0; i3<connections[0]; i3++ )
				//{
				//	chkCounter[i3]=0;
				//	connectionsCopy[i3+1] = connections[i3+1];
				//	//cout << connections[i3+1] << ", ";
				//}
				//cout << endl;

				//record the number of connections to LEAVES of the subnodes of i in chkCounter
				int j=0;
				for( _edges[i]->reset(); (j < connectionsCopy[0]) && (!_edges[i]->nullTest()) ; _edges[i]->fwd() ){
					// scan the subnodes of i..
					int subnode = _edges[i]->getData();
					for( _edges[subnode]->reset(); (j < (connectionsCopy[0]) ) &&
							 (!_edges[subnode]->nullTest()) ; _edges[subnode]->fwd() ){
						// ..and scan the subnodes of the subnodes of i..
						// ..looking for leaves (i.e. only one egde) ..
						if( _edges[_edges[subnode]->getData()]->getSize() <=2 ){
							// ..looking for leaves, i.e. only one egde, but with some tolerance ..
							// ..and count the no of leaves with chkCounter
							chkCounter[j]++;
						}
					}
					j++;
				}

				int chkAccumulator = 0;

				int m;
				// the data in the vector is stored from place [1] to place [connections[0]+1]
				for( int n=0; n<connectionsCopy[0]; n++ ){
					// I need to check that all (but at most tolerance) of the subnodes of i
					// have the same cardinality as the target
          bool flag=false;
					for( m=1; (m<(connectionsCopy[0]+1))&&(flag==false); m++ ){
						if( chkCounter[n] == connectionsCopy[m] ){
							chkAccumulator++;
							connectionsCopy[m] = -1; //if the value is found I remove it from the list of targets,
							//so that it can not be hit twice
							flag = true;
							//I have to break, as if I find the value connections[m] I want it only once
							//so I go to connections[m+1]
						}
					}
				}
				// if (noConnections - tolerance) regions match the criteria I assume this is the fiducial!
				// so tmp is appended to the result array
				// this means that the array is reallocated
				if( chkAccumulator >= (connectionsCopy[0] - tolerance) ){
					int *tmp;
					result[0]++;
					tmp = new int[ result[0] + 1 ];
					for( int i5 = 0; i5< (result[0] +1); i5++ ){
						tmp[i5] = result[i5];
					}
					delete [] result;
					result = tmp;
					result[ result[0] ] = i;
				}
			}
		}
	}

  if(connectionsCopy!=NULL){
    delete [] connectionsCopy;
    connectionsCopy=NULL;
  }
  if(chkCounter!=NULL){
    delete [] chkCounter;
    chkCounter=NULL;
  }

	return( result );
}


void RegionAdjacencyGraph::markLeavesAndBranches()
/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method performs a preparatory step needed for the fiducial         **
 **  detection "from leaves up (to root)".                                   **
 **                                                                          **
 **  The graph (or rather tree) is transversed and all the one-connected     **
 **  black nodes are marked as leaves. Subsequently, all the non-leaf nodes  **
 **  are visited, and the number of leaf-children is counted and stored.     **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
{
  if( leavesMarked ){
    return;
  }
	// the following defines the tolerance on the degree
	// for a node to be marked as a leaf
  int tolerance = 1;

  // NOTE: would it be faster to use a pointer to region
  // and increment it each time rather than accessing _region[n] all the time?
  // similarly for _edges[k]

  // count and store how many black leaves in each white node
  // for+if combination that means: "for each white node n"
  for(int n=0; n<_used; n++){
    if( _region[n]->_white ){
      for( _edges[n]->reset(); !_edges[n]->nullTest(); _edges[n]->fwd() ){
				//for each subnode s of n
        int s = _edges[n]->getData();
        if( _edges[s]->getSize() <= tolerance){
					//if s is a leaf increment the number of leaves in n
          _region[n]->_noLeaves++;
        }
      }
    }
  }

  //for and if combination to get all the black nodes
  for(int n=0; n<_used; n++){
    if( !_region[n]->_white ){
      for( _edges[n]->reset(); (!_region[n]->_potentialRoot)&&(!_edges[n]->nullTest()); _edges[n]->fwd() ){
				// for each subnode s of n (and as long as n
				// is not recognised as a potential root)
        //mk int s = _edges[n]->getData();
        if( _region[n]->_noLeaves > 0 ){
					//if s contains leaves mark n as a potential root
          _region[n]->_potentialRoot = true;
        }
      }
    }
  }

  leavesMarked = true;

  return;
}

void RegionAdjacencyGraph::markLeaves()
/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method performs a preparatory step needed for the fiducial         **
 **  detection "from root down (to leaves)".                                 **
 **                                                                          **
 **  The graph (or rather tree) is transversed and all the one-connected     **
 **  black nodes are marked as leaves.                                       **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
{
  if( leavesMarked ){
    return;
  }
  // the following defines the tolerance on the degree
	// for a node to be marked as a leaf
  int tolerance = 1;

  // NOTE: would it be faster to use a pointer to region
  // and increment it each time rather than accessing _region[n] all the time?
  // similarly for _edges[k]

  // count and store how many black leaves in each white node
  // for+if combination that means: "for each white node n"
  for(int n=0; n<_used; n++){
    if( _region[n]->_white ){
      for( _edges[n]->reset(); !_edges[n]->nullTest(); _edges[n]->fwd() ){
				//for each subnode s of n
        int s = _edges[n]->getData();
        if( _edges[s]->getSize() <= tolerance){
					//if s is a leaf increment the number of leaves in n
          _region[n]->_noLeaves++;
        }
      }
    }
  }

  leavesMarked = true;
  
  return;
}

int compareOneToMany( const int data, const int *array, const int tolerance)
/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This function checks if 'data' is contained in 'array' with a given     **
 **  tolerance                                                               **
 **  If so, it returns the position of the first element in 'array' that     **
 **  matches 'data', else it returns -1.                                     **
 **  'array' is a variable lenght array. Its dimension must be at least 1,   **
 **  the value of the first element always indicates the number of other     **
 **  elements.                                                               **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
{
  for(int i=1; i<=array[0]; i++){
    if( abs(data - array[i])<=tolerance ){
      return i;
    }
  }
  return -1;
}

int *RegionAdjacencyGraph::findFiducialRootDown( const int *connections, int tolerance )
/******************************************************************************
 ******************************************************************************
 **                                                                          **
connections is an array that rapresents a small graph, it has only 2 levels,
the first element is the "root" it indicates the number of subnodes that it has,
so it is also the size of the array
*/
{
	int branchTolerance = tolerance +1 ;
	int *result;
	result = new int[1];

	result[0] = 0;
	int *connectionsCopy = new int[connections[0] +2];
	connectionsCopy[0] = connections[0] + 1;
	connectionsCopy[1] = -1;

	// for&if combination: look among the BLACK nodes
	for(int i=0;i<_used;i++){
		if( _edges[i]->getSize() == connectionsCopy[0] ){
			// this might be the external black region of the fiducial  
			memcpy(connectionsCopy+2, connections+1, connections[0]*sizeof(int));
			int branchMatches=0;
			for( _edges[i]->reset(); !_edges[i]->nullTest(); _edges[i]->fwd() ){
				// scan the subnodes (i.e. children ) of i..
				// NOTE: this could be made faster by stopping
				// when the number of matches is big enough
				int subnode = _edges[i]->getData();
				int leavesInSubnode = _region[subnode]->_noLeaves;
				int branchIndex = compareOneToMany( leavesInSubnode, connectionsCopy, 0);
				if( branchIndex > 0 ){
					// if this number of leaves is the same as the number of leaves in one
					// of the fiducial branches..
					// then increment the number of branch matches
					branchMatches++;
					// and remove this branch from the fiducial sequence
					// to avoid matching (i.e. counting) more than once
					connectionsCopy[branchIndex] = -1 ;
				}
			}
			if( branchMatches >= (connectionsCopy[0] - branchTolerance) ){
				// node i is a fiducial root, append i to the results
				// this implies the result array being reallocated
				result[0]++;
				int *tmp = new int[ result[0] + 1 ];
				memcpy(tmp,result, sizeof(int)*result[0]);
				delete [] result;
				result = tmp;
				result[ result[0] ] = i;
			}
		}
	}
	
	delete [] connectionsCopy;
	//connectionsCopy = NULL;

	return( result );
}

/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method receives the index of a know fiducial root and a pointer    **
 **  to an array containing the fiducial tree (sequence).                    **
 **  It returns a pointer to an array containing the indexes of the branch   **
 **  regions corresponding to root. The first element of the array           **
 **  indicates how many branches have been found. This could be less than    **
 **  connections[0] because tolerance on the number of branches can be used  **
 **  for fiducial recognition. If a branch is not found the correspoing      **
 **  element in the array is set to -1                                       **
 **                                                                          **
 **  Note that this works only when the fiducial has no branches with the    **
 **  same number of leaves (i.e. for any k there is only (or at most)        **
 **  one branch with k leaves).                                              **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
int* RegionAdjacencyGraph::findBranches( const int root, const int *connections ){
	int *result = new int[connections[0]+1];
	result[0] = 0;

	for( int k=1; k<=connections[0];k++ ){
		result[k] = -1;
		for( _edges[root]->reset(); (!_edges[root]->nullTest())&&(result[k]==-1); _edges[root]->fwd() ){
			int subnode = _edges[root]->getData();
			// if subnodes has currentLeaves leaves
			// and the degree of subnode is equal to currentLeaves+1
			if(  (_region[subnode]->_noLeaves == connections[k]) 
				&& ( getDegree(subnode) == (connections[k]+1) ) 
				//&& ( subnode > root )  
				){
				// if we are looking for an empty branch
				// make sure that the current subnode is a leaf
				//if( connections[k] == 0 ){
				//	if( getDegree(subnode) > 1 ){
				//		// this is the case..
				//	}
				//}
				result[k] = subnode;
				result[0]++;
			}
		}
	}

	return result;
}
//*/


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method receives the index of a know fiducial root and a pointer    **
 **  to an array containing the fiducial tree (sequence).                    **
 **  It returns a pointer to an array containing the indexes of the branch   **
 **  regions in the same order as the sequence describing the fiducial       **
 **  tree                                                                    **
 **                                                                          **
 **  If more than one branch has the same number of leaves the               **
 **  corresponding results are (obviously) in random order.                  ** 
 **  For example, if the sequence is 4-1-1-2-3 the two 1-branches will       **
 **  appear in random order.                                                 **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
int* RegionAdjacencyGraph::findBranchesGeneric( const int root, const int *connections ){
	int *result = new int[connections[0]+1];
	result[0] = connections[0];
	memset(result+1,0,connections[0]*sizeof(int));

	int *connectionsCopy = new int[connections[0]];
	memcpy(connectionsCopy,connections+1,connections[0]*sizeof(int));

	for( _edges[root]->reset(); !_edges[root]->nullTest(); _edges[root]->fwd() ){
		int subnode = _edges[root]->getData();
		// for this subnode to be a  branch all of its other subnodes 
		// must be leaves this means that the degree of subnode minus 
		// the number of leaves must be equal to 1
		// NOTE THIS IS NOT 100% foolproof!!
		if( ( getDegree(subnode) - getNoLeaves(subnode) ) == 1 ){
			for(int j=0;j<=connections[0];j++){
				if( getNoLeaves(subnode) == connectionsCopy[j] ){
					connectionsCopy[j] = -1;
					result[j+1] = subnode;
					break;
				}
			}
		}
	}

	delete [] connectionsCopy; 
	return result;
}

void RegionAdjacencyGraph::decodeFiducialLinear( int root, int *fiducialSequence, FiducialData &fd ){
	DTPoint result = getRegionCentre( getRegion(root) );
	//int type = -1;
	int angle = -1;

	// store the branch region indexes in branch
	// in this way branch[n] corresponds to the branch with sequence[n] leaves
	int *branch = findBranchesGeneric( root, fiducialSequence );

	// calculate the centre of the fiducial and of each branch
	DTPoint *centre = new DTPoint[branch[0]+1];
	centre[0] = DTPoint(0,0);
	for(int j=1;j<=branch[0];j++){
		centre[j] = getRegionCentreRound( getRegion(branch[j]) );
		centre[0] += centre[j];
		fd.setBranchCentre( (j-1), centre[j]);
	}
	centre[0] /= branch[0];
	fd.Centre() = centre[0];

	// find the branch with the smallest number of leaves
	// (if more than one exist, just take the first)
	int minLeaves = getNoLeaves(branch[1]);
	int ref = 1;
	for(int i=2;i<=fiducialSequence[0];i++){
		if( getNoLeaves( branch[i] ) < minLeaves ){
			minLeaves = getNoLeaves( branch[i] );
			ref = i;
		}
	}
	
	char *seq = new char[ fiducialSequence[0]+1 ];
	//int k=1;
	//seq[0] = getNoLeaves(ref) + 48;
	for(int i=0;i<fiducialSequence[0];i++){
		seq[i] = fiducialSequence[i+1] + 48;
		//if( fiducialSequence[i] != getNoLeaves(ref) ){
		//	seq[k++] = fiducialSequence[i] + 48;
		//}
	}
	//seq[k] = 0;
	seq[ fiducialSequence[0] ] = 0;

	int *di = new int[fiducialSequence[0]];
	for(int i=0;i<fiducialSequence[0];i++){
		di[i] = centre[ref].dist(centre[i+1]);
	}

	// sort the rest of the branches by increasing distance 
	// from the branch with smallest number of leaves
	for(int j=0;j<fiducialSequence[0];j++){
		for(int i=0;i<(fiducialSequence[0]-1);i++){
			//if( (i!=ref)&&((i+1)!=ref) ){
				/*mk int dA* =*/ centre[ref].dist( centre[i] );
				/*mk int dB* =*/ centre[ref].dist( centre[i+1] );
				if( di[i] > di[i+1] ){
					// swap the distances
					int swpInt = di[i];
					di[i] = di[i+1];
					di[i+1] = swpInt;

					// swap seq[i-1] and seq[i]
					char swp = seq[i];
					seq[i] = seq[i+1];
					seq[i+1] = swp;
				}
			//}
		}
	}

	//cout << "seq: " << seq << endl;
	fd.Type() = atoi(seq);

	// !!! JMS - Fix memory leak
	delete [] seq;

	delete [] di;

	// cannot find corners in this case!
	fd.setCorner( 0, centre[0] );
	fd.setCorner( 1, centre[0] );
	fd.setCorner( 2, centre[0] );
	fd.setCorner( 3, centre[0] );

	int dx = centre[0].x - centre[1].x;
	int dy = centre[0].y - centre[1].y;
	if(dx==0){ dx = 1; }

	double angleD = atan(-(double)dy/(double)dx) + 0.8853;

	//mk int sign = ((dx<0)?(-1):(1));
		angle = - ((int) ( angleD*180.0/3.14159265 ) ) ;
	if( angle > 180 ){
		angle -= 360;
	}
	fd.Angle() = angle;
	
	delete [] centre;

	delete [] branch;
	fd.Centre() = result;

	return;
}


void RegionAdjacencyGraph::decodeFiducial4( int root, int *fiducialSequence, FiducialData &fd ){
	DTPoint result = getRegionCentre( getRegion(root) );
	int type = -1;
	int angle = -1;

	int *branch = findBranches( root, fiducialSequence );
	if( branch[0] >= fiducialSequence[0] ){
		// calculate the centre of the fiducial and of each branch
		DTPoint *centre = new DTPoint[branch[0]+1];
		centre[0].x = 0;
		centre[0].y = 0;
		for(int j=1;j<=branch[0];j++){
			centre[j] = getRegionCentreRound( getRegion(branch[j]) );
			centre[0] += centre[j];
			fd.setBranchCentre( j-1, centre[j]);
		}
		centre[0] /= branch[0];
		result = centre[0];

		DTPoint *imgCentre = new DTPoint[branch[0]+1];
		memcpy( imgCentre,centre,(branch[0]+1)*sizeof(DTPoint) );

		// transform the branch coordinates into a coordinate system centred on the
		// branch with one leaf and having the root centre in (5,15)
		// (this is needed to decode square fiducial sequences)
		// calculate the transfrom matrix M
		double M[2][3];
		{
			int dx = centre[0].x - centre[1].x;
			int dy = centre[0].y - centre[1].y;
			if(dx==0){ dx = 1; }

			double angleD = atan(-(double)dy/(double)dx) + 0.8853;
			int sign = ((dx<0)?(-1):(1));

			angle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
			if( angle > 180 ){
				angle -= 360;
			}

			M[0][0] = sign * cos(angleD);
			M[0][1] = sign * sin(-angleD);

			M[1][0] = - M[0][1];
			M[1][1] = M[0][0];

			#ifdef _DBG
			cout << "angle: " << angleD;
			cout << "\t\tsign: " << sign << endl << endl;
			#endif
		}

		DTPoint offset(centre[0]);
		// apply the transformation
		for(int i=0;i<=branch[0];i++){
			DTPoint tmpPoint = centre[i] - offset;
			//centre[i].x = (int) round(M[0][0]*tmpPoint.x + M[0][1]*tmpPoint.y );
			centre[i].x = (int) floor(0.5 + M[0][0]*tmpPoint.x + M[0][1]*tmpPoint.y );
			//centre[i].y = (int) round(M[1][0]*tmpPoint.x + M[1][1]*tmpPoint.y );
			centre[i].y = (int) floor(0.5 + M[1][0]*tmpPoint.x + M[1][1]*tmpPoint.y );
		}

		#ifdef NO_DEBUG
		display->plotSegment( (centre[1]+DTPoint(100,100)), 
			(centre[0]+DTPoint(100,100)),   0, 255, 255 );
		display->plotSegment( (centre[1]+DTPoint(100,100)), 
			(centre[2]+DTPoint(100,100)), 255,   0,   0 );
		display->plotSegment( (centre[1]+DTPoint(100,100)), 
			(centre[3]+DTPoint(100,100)),   0, 255,   0 );
		display->plotSegment( (centre[1]+DTPoint(100,100)), 
			(centre[4]+DTPoint(100,100)),   0,   0, 255 );
		#endif


		DTPoint d1;
		d1 |= (centre[1] - centre[0]);
		DTPoint d12;
		d12 |= (centre[1] - centre[2]);
		DTPoint d13;
		d13 |= (centre[1] - centre[3]);

		if( (d12.x>d1.x) && (d12.y<d1.y) ){
			if( d13.x < d1.x ){ type = 234; } else { type = 243; }
		}else if( (d13.x>d1.x) && (d13.y<d1.y) ){
			if( d12.x < d1.x ){ type = 324; } else { type = 342; }
		}else{
			if( d12.x < d1.x ){ type = 423; } else { type = 432; }
		}

		fd.Type() = type;

		DTPoint *corner = new DTPoint[branch[0]+1];
		corner[0] = imgCentre[0];
		fd.setCorner(0, imgCentre[0]);
		for(int i=1;i<=branch[0];i++){
			DTPoint tmp = imgCentre[i] - imgCentre[0];
			tmp *= 22;
			tmp /= 10;
			corner[i] = imgCentre[0] + tmp;
		}

		fd.setCorner(0, corner[1]);
		fd.setCorner(1, corner[type/100]);
		fd.setCorner(2, corner[type%10]);
		fd.setCorner(3, corner[(type/10)%10]);

		delete [] centre;
		delete [] imgCentre;

		delete [] corner;
	}

	delete [] branch;

	fd.Centre() = result;

	return;
}

/***
 * this method works with the sequence { 5, 0, 1, 2, 3, 4 } ONLY
 * not any permutation of it
 * and expecially NOT { 5, 1, 2, 3, 4, 0 } 

*/
void RegionAdjacencyGraph::decodeFiducial5( int root, int *fiducialSequence, FiducialData &fd ){
	//DTPoint result = getRegionCentre( getRegion(root) );
	DTPoint result = getRegionCentreRound( getRegion(root) );
	fd.Centre() = result;
  
	fd.Type() = 12345;
	fd.Angle() = 0;

	int *branch = findBranches( root, fiducialSequence );

	if( branch[0] >= fiducialSequence[0] ){
		// calculate the centre of the fiducial and of each branch
		DTPoint *centre = new DTPoint[branch[0]+1];
		centre[0].x = 0;
		centre[0].y = 0;

		// handle the empty branch separately
		centre[1] = getRegionCentreRound( getRegion(branch[1]) );
		fd.setBranchCentre(0, centre[1]);
		for(int j=2;j<=branch[0];j++){
			centre[j] = getRegionCentreRound( getRegion(branch[j]) );
			centre[0] += centre[j];
			fd.setBranchCentre(j-1, centre[j]);
		}
		centre[0] /= 4;
		result = centre[0];

		fd.Centre() = centre[0];

		int seq[4];

		// find the two branches collinear with the empty one
		int permTable[6][2] = {	{2,3}, {2,4}, {2,5}, {3,4}, {3,5}, {4,5} };
		// start from the first possibility: {2,3}
		int minCollinear = collinear( centre[1], centre[2], centre[3] );
		seq[0] = 2;
		seq[2] = 3;
		for(int i=1;i<6;i++){
			int j1 = permTable[i][0];
			int j2 = permTable[i][1];
			int currCollinear = collinear( centre[1], centre[j1], centre[j2] );
			if( currCollinear < minCollinear ){
				minCollinear = currCollinear;
				seq[0] = j1;
				seq[2] = j2;
			}
		}

		// sort seq[0] and seq[2]
		if( centre[1].dist(centre[seq[0]]) > centre[1].dist(centre[seq[2]]) ){
			// swap the two
			int tmp = seq[0];
			seq[0] = seq[2];
			seq[2] = tmp;
		}
            
		// between the two branches left over, the closest to
		// the empty branch is the second in the sequence
		int minDist = 1000000;
		for(int i=2;i<=branch[0];i++){
			if( (i!=seq[0]) && (i!=seq[2]) ){
				int currDist = centre[1].dist( centre[i] );
				if( currDist < minDist ){
					minDist = currDist;
					seq[1] = i;
				}
			}
		}

		// find the only other branch
		// that's the last of the sequence
		for(int i=2;i<=branch[0];i++){
			if( (i!=seq[0]) && (i!=seq[1]) && (i!=seq[2]) ){
				seq[3] = i;
			}
		}

		char seqChar[5];
		for(int k=0;k<4;k++){
			seqChar[k] = '0' + seq[k];
		}
		seqChar[4] = 0;

		//fd.Type() = seq[3]*1 + seq[2]*10 + seq[1]*100 + seq[0]*1000;
		fd.Type() = atoi( seqChar ) + 10000;
	        
		// the angle can be calculated as atan( (y[0] - y[4]) / (x[0] - x[4]) )
		DTPoint p0 = centre[seq[0]];
		DTPoint p1 = centre[seq[1]];

		int dx = p1.x - p0.x;
		int dy = p1.y - p0.y;

		double angleD;
		if( dx!= 0 ){
			angleD = atan(-(double)dy/(double)dx);// + 0.8853;
		}else{
			if( dy>0 ){
				dx = 1;
			}else{
				dx = -1;
			}
			angleD = -acos(0.0);
		}

		int sign = ( (dx < 0)?(-1):(1) );

		int tmpAngle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
		if( tmpAngle > 180 ){
			tmpAngle -= 360;
		}
		fd.setAngle(tmpAngle);

		for(int i=0;i<4;i++){
			DTPoint tmp = centre[seq[i]] - fd.Centre();
			tmp *= 42;
			tmp /= 10;
			fd.setCorner( i, fd.Centre() + tmp );
		}
		
		DTPoint tmp = fd.getCorner(2);
		fd.setCorner(2, fd.getCorner(3));
		fd.setCorner(3, tmp);

		delete [] centre;
	}

	delete [] branch;


	return;
}


void RegionAdjacencyGraph::decodeFiducial6( int root, int *fiducialSequence, FiducialData &fd ){
	//DTPoint result = getRegionCentre( getRegion(root) );
	DTPoint result = getRegionCentreRound( getRegion(root) );
	fd.Type() = -1;
	fd.Angle() = -1;

	int *branch = findBranches( root, fiducialSequence );

	//cout << "branch[0]: " << branch[0] << endl;
	if( branch[0] >= fiducialSequence[0] ){
		// calculate the centre of the fiducial and of each branch
		DTPoint *centre = new DTPoint[branch[0]+1];
		centre[0].x = 0;
		centre[0].y = 0;
		for(int j=1;j<=branch[0];j++){
			//cout << "branch["<<j<<"]:" << branch[j]; cout.flush();
			//centre[j] = getRegionCentre( getRegion(branch[j]) );
			centre[j] = getRegionCentreRound( getRegion(branch[j]) );
			centre[0] += centre[j];
			//cout << " (" << centre[j].x << "," << centre[j].y << ")" << endl;
		}
		centre[0] /= branch[0];
		result = centre[0];
    
		int counter = 0;
		int collinearity[10];
		int combination[10][2];
		//type = 9;
		for(int i=2;i<=5;i++){
			for(int j=i+1;j<=6;j++){
				int a = i;
				int b = j;
  				int c = 1;

				collinearity[counter] = collinear( centre[a], centre[b], centre[c] );
				combination[counter][0] = a;
				combination[counter][1] = b;
				counter++;
			}
		}
		int min = 1000;
		int minIndex = 0;
		for(int i=0;i<10;i++){
			if( min > collinearity[i] ){
				min = collinearity[i];
				minIndex = i;
			}
		}

		int s[5];

		if( centre[1].dist(centre[combination[minIndex][0]]) < centre[1].dist(centre[combination[minIndex][1]]) ){
			s[1] = combination[minIndex][0];
			s[3] = combination[minIndex][1];
		}else{
			s[1] = combination[minIndex][1];
			s[3] = combination[minIndex][0];
		}

		for(int i=0;i<6;i++){
			collinearity[i] = collinear( centre[1], centre[0], centre[i+2] );
			//cout << i <<": "<< collinearity[i] << endl;
		}
		min = 1000;
		minIndex = 0;
		for(int i=0;i<6;i++){
			if( min > collinearity[i] ){
				min = collinearity[i];
				minIndex = i;
			}
		}
		minIndex+=2;
		// branch[minIndex] is the last in sequence
		s[4] = minIndex;
    
		int cnt=0;
		for(int n=2;n<7;n++){
			if( ( n != s[1] )&&( n != s[3] )&&( n != s[4] ) ){
				s[cnt] = n;
				cnt += 2;
			}
		}

		// look for the branch closer to the last among branch[s[0]] and branch[s[2]] 
		if( centre[ s[4] ].dist(centre[ s[0] ]) < centre[ s[4] ].dist(centre[ s[2] ]) ){
			//swap 0 and 2 in sequence
			int tmp = s[0];
			s[0] = s[2];
			s[2] = tmp;
		}

		fd.Type() = s[4]*1 + s[3]*10 + s[2]*100 + s[1]*1000 + s[0]*10000 + 100000;

		fd.setBranchCentre(0, centre[1]);
		for(int i=1;i<6;i++){
			fd.setBranchCentre(i, centre[ s[i-1] ]);
		}
    
		// the angle can be calculated as atan( (y[0] - y[4]) / (x[0] - x[4]) )
		DTPoint p0 = fd.getBranchCentre(1);
		DTPoint p1 = fd.getBranchCentre(5);

		double angleD = atan(-(double)(p1.y - p0.y)/(double)(p1.x - p0.x));// + 0.8853;

		int sign = (((p1.x - p0.x)<0)?(-1):(1));

		int tmpAngle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
		if( tmpAngle > 180 ){
			tmpAngle -= 360;
		}
		fd.setAngle(tmpAngle);

		//delete [] yOrder;
		//delete [] yDist;
		delete [] centre;

	}

	delete [] branch;

	fd.Centre() = result;
  
	return;
}


DTPoint RegionAdjacencyGraph::decodeFiducialGeneric( int root, int *fiducialSequence, int &type, int &angle ){
	DTPoint result;
	type = -2;
	angle = -1;

	int *branch = findBranchesGeneric( root, fiducialSequence );

	//cout << "branch[0]: " << branch[0] << endl;
	//cout << "fiducialSequence[0]: " << fiducialSequence[0] << endl;
  
	if( branch[0] == fiducialSequence[0] ){
		// calculate the centre of the fiducial and of each branch
		DTPoint *centre = new DTPoint[branch[0]+1];
		centre[0].x = 0;
		centre[0].y = 0;

    for(int j=1;j<=branch[0];j++){
			centre[j] = getRegionCentre( getRegion(branch[j]) );
			centre[0] += centre[j];
 		}
		centre[0] /= branch[0];
		result = centre[0];

    cout << "centre: " << result << endl;

		delete [] centre;

	}else{
    result = getRegionCentre( getRegion(root) );
  }

	delete [] branch;

	return result;
}

// fiducialData: type, x, y, angle, region#
int RegionAdjacencyGraph::findFiducials( int *fiducialSequence, 
						 FiducialData **fiducialData, bool usingLinearDecoding )
{
	int noFiducials = 0;
	markLeaves();
	int *result = findFiducialRootDown(fiducialSequence, 0);

	noFiducials = result[0];
	if( result[0] == 0 ){
		// !!! JMS - Fix memory leak
		delete [] result;
		*fiducialData = NULL;
		return noFiducials;
	}

	FiducialData *fd = new FiducialData[result[0]];
	*fiducialData = fd;

	for(int i=0;i<result[0];i++){
		fd[i].init(fiducialSequence);
		fd[i].RootRegion() = result[i+1];

		if( usingLinearDecoding ){
			decodeFiducialLinear(result[i+1],fiducialSequence, fd[i] );
		}else{
			// use one of the specific methods
			if( fiducialSequence[0] == 6 ){
  				decodeFiducial6(result[i+1],fiducialSequence, fd[i] );
			}else if( fiducialSequence[0] == 5 ){
  				decodeFiducial5(result[i+1],fiducialSequence, fd[i] );
			}else if( fiducialSequence[0] == 4 ){
				decodeFiducial4(result[i+1],fiducialSequence, fd[i] );
			}else{ 
				// some other fiducial sequence for which 
				// geometric decoding is not supported
				// fall back on linear decoding
				decodeFiducialLinear(result[i+1],fiducialSequence, fd[i] );
			}
		}    
	}
	delete [] result;

	return noFiducials;
}

//
