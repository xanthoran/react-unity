/***************************************************************************
                    fiducialrecognition.cpp  -  description
                             -------------------
    begin                : Tue Aug 27 2002
    copyright            : (C) 2002 by Enrico Costanza
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
	Small amount of further optimisation by Justen Hyde

*/

#include "fiducialrecognition.h"
#include "types.h"

#include "regionadjacencygraph.h"
#include "ragbuilder.h"
#include "listhash.h"

#include <iomanip>
#include <math.h>
#include <string.h>

using std::cout;
using std::cerr;
using std::endl;

//345678901234567890123456789012345678901234567890123456789012345678901234567890

FiducialRecognition::FiducialRecognition( int in_width, int in_height, 
			int in_noFiducialSequences, int **in_fiducialSequence, 
			bool *in_usingLinearFiducials ) :
width(in_width),
height(in_height),
m_uD(40),
m_uR(64),
//m_uS(m_uD/2)
m_uS(20),
//m_uM(in_width/m_uS),
//m_uN(in_height/m_uS),
m_uM(in_height/20),
m_uN(in_width/20)
{
	//for(int i=0;i<256;i++) s[i]=0;
	// !!! JMS - Does it have to be initialized
	memset(s,0,256*sizeof(int));
	
	// !!! JMS
	//minBuffer = new int [width/(d/2) * height/(d/2)];
	//maxBuffer = new int [width/(d/2) * height/(d/2)];
	m_vuMinBuffer = new unsigned int [(in_width/20) * (in_height/20)];
	m_vuMaxBuffer = new unsigned int [(in_width/20) * (in_height/20)];
	
	_graphSize = 4000;
	_regionsPool = new StaticPool<DTPoint>(width*height);
	_edges = new ListHash<int>*[_graphSize];
	_stored = new bool[width*height];
	
	_region = new DTRegion*[_graphSize];
	_labelsPtrPool = new DynamicPool<int *>(_graphSize);
	DTRegion ** l_Region = _region;
	ListHash<int>** l_Edges = _edges;
	for(int i=0;i<_graphSize;i++){
		*l_Edges++ = new ListHash<int>(_graphSize);
		//_region[i] = new DTRegion(_regionsPool, _labelsPtrPool);
		*l_Region++ = new DTRegion(_regionsPool, _labelsPtrPool);
	}

	_labelsMap = new int*[width*height];
	_labels = new int[width*height];
	
	//threshold = new unsigned char[width*height];
	//memset(threshold, 128, width*height);
	
	noFiducialSequences = in_noFiducialSequences;
	usingLinearFiducials = new bool[noFiducialSequences];
	fiducialSequence = new int * [noFiducialSequences];
	for(int i=0;i<noFiducialSequences;i++){
		usingLinearFiducials[i] = in_usingLinearFiducials[i];
		fiducialSequence[i] = new int[in_fiducialSequence[i][0]+1];
		memcpy( fiducialSequence[i], in_fiducialSequence[i], (in_fiducialSequence[i][0]+1)*sizeof(int) );
	}
}


FiducialRecognition::FiducialRecognition( int in_width, int in_height, 
						int *in_fiducialSequence, bool in_usingLinearFiducials ) :
width(in_width),
height(in_height),
m_uD(40),
m_uR(64),
//m_uS(m_uD/2)
m_uS(20),
//m_uM(in_width/m_uS),
//m_uN(in_height/m_uS),
m_uM(in_height/20),
m_uN(in_width/20)
{
	//for(int i=0;i<256;i++) s[i]=0;
	// !!! JMS - Does it have to be initialized
	memset(s,0,256*sizeof(int));
	
	// !!! JMS
	//minBuffer = new int [width/(d/2) * height/(d/2)];
	//maxBuffer = new int [width/(d/2) * height/(d/2)];
	m_vuMinBuffer = new unsigned int [(in_width/20) * (in_height/20)];
	m_vuMaxBuffer = new unsigned int [(in_width/20) * (in_height/20)];
	
	_graphSize = 4000;
	_regionsPool = new StaticPool<DTPoint>(width*height);
	_edges = new ListHash<int>*[_graphSize];
	_stored = new bool[width*height];
	
	_region = new DTRegion*[_graphSize];
	_labelsPtrPool = new DynamicPool<int *>(_graphSize);
	DTRegion ** l_Region = _region;
	ListHash<int>** l_Edges = _edges;
	for(int i=0;i<_graphSize;i++){
		*l_Edges++ = new ListHash<int>(_graphSize);
		//_region[i] = new DTRegion(_regionsPool, _labelsPtrPool);
		*l_Region++ = new DTRegion(_regionsPool, _labelsPtrPool);
	}
	
	_labelsMap = new int*[width*height];
	memset(_labelsMap,0, sizeof(int*)*width*height);

	_labels = new int[width*height];
	memset(_labels,0, sizeof(int)*width*height);
	
	//threshold = new unsigned char[width*height];
	//memset(threshold, 128, width*height*sizeof(unsigned char));	
	
	if( in_fiducialSequence==NULL ){
		noFiducialSequences = 1;
		usingLinearFiducials = new bool[noFiducialSequences];
		usingLinearFiducials[0] = false;
		fiducialSequence = new int * [noFiducialSequences];
		fiducialSequence[0] = new int[6];
		fiducialSequence[0][0] =  5;
		fiducialSequence[0][1] =  0;
		fiducialSequence[0][2] =  1;
		fiducialSequence[0][3] =  2;
		fiducialSequence[0][4] =  3;
		fiducialSequence[0][5] =  4;
	}else{
		noFiducialSequences = 1;
		usingLinearFiducials = new bool[noFiducialSequences];
		usingLinearFiducials[0] = in_usingLinearFiducials;
		fiducialSequence = new int *[noFiducialSequences];
		fiducialSequence[0] = new int[in_fiducialSequence[0]+1];
		memcpy( fiducialSequence[0], in_fiducialSequence, (in_fiducialSequence[0]+1)*sizeof(int) );
	}
}

FiducialRecognition::~FiducialRecognition()
{
	if (m_vuMinBuffer != NULL)
	{
		delete [] m_vuMinBuffer;
		m_vuMinBuffer = NULL;
	}

	if (m_vuMaxBuffer != NULL)
	{
		delete [] m_vuMaxBuffer;
		m_vuMaxBuffer = NULL;
	}

	delete [] usingLinearFiducials;
	for(int i=0;i<noFiducialSequences;i++){
		delete [] fiducialSequence[i];
	}
	delete [] fiducialSequence;

	//if(threshold!=NULL){ delete [] threshold; }

	for(int i=0;i<_graphSize;i++){
		delete _edges[i];
		delete _region[i];
	}

	delete _regionsPool;
	delete _labelsPtrPool;

	delete [] _edges;
	delete [] _region;
	delete [] _labelsMap;
	delete [] _labels;
	delete [] _stored;
}


// WARNING: process ASSUMES image is in grayscale format
int FiducialRecognition::process( unsigned char *img, unsigned char *thresh, FiducialData **fiducialData ){
	// for compatibility with NON-ANSI old MS compiler
	// index variable i is declared once for the entire
	// method
	// I do *not* like this, but I need compatibility
	// ..at least until mr JRH will decide that Linux is the way to go! :-)
	int i;
	threshold = thresh;

	// !!! JMS
	//memset(histogram,0,256*sizeof(int));
	memset(histogram,0,sizeof(histogram));
	
	unsigned char *ip = img;
	int limit = width*height;
	for(i=0;i<limit;i++){
		(*(histogram + *ip++))++;
	}
	
	setBernsenThreshold( img );

	RegionAdjacencyGraph sceneRAG( width, height, _graphSize, _regionsPool,
			_edges, _region, _labelsMap, _labels );
	
	RAGBuilder ragBuilder( img, width, height, threshold, &sceneRAG, _stored );
	
	// 
	//int ragBuilt = ragBuilder.buildRAG();
	//int ragBuilt = ragBuilder.buildRAGBorder();
	int ragBuilt = ragBuilder.buildRAGFullBorder(_graphSize/2);
	
	// update the graph size (stored in MyCameraFramework)
	// as the graph might have been expanded in buildRAG
	_graphSize = sceneRAG.getSize();
	_edges = sceneRAG.getEdges();
	_region = sceneRAG.getRegions();
	
	if( ragBuilt != 0 ){
		fiducialData = NULL;
		return 0;
	}
	
	//mk int noRegions = sceneRAG.getNoRegions();

	int noFiducials = 0;
	for(int k=0;k<noFiducialSequences;k++){
		FiducialData *tmpFiducialData;
		
		int tmpNoFiducials = sceneRAG.findFiducials( fiducialSequence[k], 
						&tmpFiducialData, usingLinearFiducials[k] );
		
		FiducialData *result = new FiducialData [tmpNoFiducials+noFiducials];
		memcpy( result, *fiducialData, noFiducials * sizeof(FiducialData) );
		memcpy( result+noFiducials, tmpFiducialData, tmpNoFiducials * sizeof(FiducialData) );
		
		delete [] *fiducialData;
		*fiducialData = result;
		delete [] tmpFiducialData;
		
		noFiducials += tmpNoFiducials;
	}
	
	#ifdef _EC_DEBUG
	cout << noFiducials << " fiducials found" << endl;
	for( i=0;i<noFiducials;i++){
		FiducialData *fiducialDataPtr = fiducialData[i];
		cout << "#" << i << ": type: " << fiducialDataPtr->Type() << " -> ";
		cout << ", angle: " << fiducialDataPtr->Angle();
		cout << endl;
	}
	#endif

	return noFiducials;
}

void FiducialRecognition::setMinMaxBuffersForThreshold(unsigned char *img)
{
	unsigned int *minBufferPtr = m_vuMinBuffer;
	unsigned int *maxBufferPtr = m_vuMaxBuffer;

	unsigned int m;
	unsigned int n;

	unsigned int x;
	unsigned int y;

	unsigned int max;
	unsigned int min;

	unsigned int nXs;
	unsigned int mXs;

	unsigned int yXwidth;

	unsigned char *ip;

	for(m = 0, mXs = 0; m < m_uM; ++m, mXs += m_uS )
	{
		for(n = 0, nXs = 0; n < m_uN; ++n, nXs += m_uS )
		{
			max = 0;
			min = 255;

			// for(y = mXs; y < ((m+1) * m_uS); ++y )
			for(y = mXs, yXwidth = (y * width); y < (mXs + m_uS); ++y, yXwidth += width )
			{
				//for(x = 0, ip = img + nXs + (y*width); x < m_uS; ++x, ++ip )
				for(x = 0, ip = img + nXs + yXwidth; x < m_uS; ++x, ++ip )
				{
					if( *ip > max )
					{
						max = *ip;
					}
					else if( *ip < min )
					{
						min = *ip;
					}
				}
			}

			*minBufferPtr++ = min;
			*maxBufferPtr++ = max;
		}
	}
}

void FiducialRecognition::setSubClustersMinMax(unsigned int uOstuThreshold)
{
	unsigned int m;
	unsigned int n;

	unsigned int mXn;
	unsigned int mXs;
	unsigned int nXs;

	unsigned int *minBufferCurrent;
	unsigned int *minBufferRight;
	unsigned int *minBufferBelow;
	unsigned int *minBufferBelowRight;

	unsigned int *maxBufferCurrent;
	unsigned int *maxBufferRight;
	unsigned int *maxBufferBelow;
	unsigned int *maxBufferBelowRight;

	unsigned int min;
	unsigned int max;

	unsigned char thresholdHere;

	unsigned int y;
	unsigned int yXwidth;

	unsigned char *tp;


	for(m = 0, mXn = 0, mXs = 0; m < (m_uM-1); ++m, mXn += m_uN, mXs += m_uS)
	{
		minBufferCurrent = m_vuMinBuffer + mXn;
		minBufferRight = m_vuMinBuffer + mXn + 1;
		minBufferBelow = m_vuMinBuffer + mXn + m_uN;
		minBufferBelowRight = m_vuMinBuffer + mXn + m_uN + 1;

		maxBufferCurrent = m_vuMaxBuffer + mXn;
		maxBufferRight = m_vuMaxBuffer + mXn + 1;
		maxBufferBelow = m_vuMaxBuffer + mXn + m_uN;
		maxBufferBelowRight = m_vuMaxBuffer + mXn + m_uN + 1;

		for(n = 0, nXs = 0; n < m_uN-1; ++n, nXs += m_uS )
		{
			//calculate cluster min
			min = *minBufferCurrent++;
			( min > *minBufferRight ) ? ( min = *minBufferRight++ ) : ( *minBufferRight++ );
			( min > *minBufferBelow ) ? ( min = *minBufferBelow++ ) : ( *minBufferBelow++ );
			( min > *minBufferBelowRight ) ? ( min = *minBufferBelowRight++ ) : ( *minBufferBelowRight++ );
			//cluster min ready

			//calculate cluster max
			max = *maxBufferCurrent++;
			( max < *maxBufferRight ) ? ( max = *maxBufferRight++ ) : ( *maxBufferRight++ );
			( max < *maxBufferBelow ) ? ( max = *maxBufferBelow++ ) : ( *maxBufferBelow++ );
			( max < *maxBufferBelowRight ) ? ( max = *maxBufferBelowRight++ ) : ( *maxBufferBelowRight++ );
			//cluster max ready

			if( (max - min) < m_uR )
			{
				thresholdHere = uOstuThreshold;
			}
			else
			{
				//thresholdHere = (min + max) / 2;
				thresholdHere = (min + max) >> 1;
			}

			//apply threshold to 1/4 of the pixel in this cluster (central ones)
			//for(y = m*m_uS+m_uS/2; y<(m+1)*m_uS+m_uS/2; y++ )
			for(y = mXs + (m_uS >> 1), yXwidth = (y*width); y < mXs + m_uS + (m_uS >> 1); ++y, yXwidth += width )
			{
				tp = threshold + nXs + (m_uS >> 1) + yXwidth;
				memset(tp, thresholdHere, m_uS);
			}
			//end of cluster
		}
	}
}



void FiducialRecognition::setBernsenThreshold( unsigned char *img )
{
	unsigned int min;
	unsigned int max;

	unsigned char thresholdHere;

	unsigned int x;
	unsigned int y;
	unsigned int yXwidth;

	unsigned int m;
	unsigned int n;

	unsigned int mXn;
	unsigned int mXs;
	unsigned int nXs;

	unsigned int sDIV2 = (m_uS >> 1);

	//calculate a global threshold, to be used when the std deviation is below a certain threshold
	int ostuT = thresholdOstu();

	//calculate 2 buffers with min and max of quarters of each cluster
	//this is used to reduce the number of operations (comparison) needed to calculate the mean
	setMinMaxBuffersForThreshold(img);

	//calculate actual min and max of each cluster, then contrast and threshold
	//for each cluster this is done combining 4 sub-clusters (i.e. the min and max values
	//of 4 sub-clusters are compared)
	setSubClustersMinMax(ostuT);

	//need to handle the borders of the image separately

	//top left corner
	min = m_vuMinBuffer[0];
	max = m_vuMaxBuffer[0];

	if( (max - min) < m_uR )
	{
		thresholdHere = ostuT;
	}
	else
	{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//apply threshold to 1/4 of the pixel in this cluster (central ones)
	//for( y = 0, yXwidth = 0; y < m_uS/2; ++y, yXwidth += width )
	for( y = 0, yXwidth = 0; y < sDIV2; ++y, yXwidth += width )
	{
		memset(threshold + yXwidth, thresholdHere, sDIV2);
	}
	//end of cluster

	//top row
	for(n = 0, nXs = 0; n < m_uN-1; ++n, nXs += m_uS)
	{
		//calculate cluster min
		min = m_vuMinBuffer[n];
		if( min > m_vuMinBuffer[n+1] )
		{
			min = m_vuMinBuffer[n+1];
		}
		//cluster min ready

		//calculate cluster max
		max = m_vuMaxBuffer[n];
		if( max < m_vuMaxBuffer[n+1] )
		{
			max = m_vuMaxBuffer[n+1];
		}
		//cluster max ready

		if( (max - min) < m_uR )
		{
			thresholdHere = ostuT;
		}
		else
		{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//apply threshold to 1/4 of the pixel in this cluster (central ones)
		//for(y = 0, yXwidth = 0; y < m_uS/2; ++y, yXwidth += width)
		x = nXs + sDIV2;
		for(y = 0, yXwidth = 0; y < sDIV2; ++y, yXwidth += width)
		{
			memset(threshold + x + yXwidth, thresholdHere, sDIV2);
		}
	//end of cluster
	}



	//top right corner
	min = m_vuMinBuffer[m_uN-1]; //m_uN-1+0*m_uN
	max = m_vuMaxBuffer[m_uN-1]; //m_uN-1+0*m_uN

	if( (max - min) < m_uR )
	{
		thresholdHere = ostuT;
	}
	else
	{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//apply threshold to 1/4 of the pixel in this cluster (central ones)
	x = (m_uN-1) * m_uS;
	for( y = 0, yXwidth = 0; y < sDIV2; ++y, yXwidth += width )
	{
		memset(threshold + x + yXwidth, thresholdHere, sDIV2);
	}
	//end of cluster


	n = m_uN-1;
	nXs = n * m_uS;
	for(m = 0, mXn = 0, mXs = 0; m < m_uM-1; ++m, mXn += m_uN, mXs += m_uS )
	{
		//calculate cluster min
		min = m_vuMinBuffer[n + mXn];
		//if( min > m_vuMinBuffer[n + (m+1)*m_uN] )
		if( min > m_vuMinBuffer[n + (mXn + m_uN)] )
		{
			min = m_vuMinBuffer[n + (mXn + m_uN)];
		}
		//cluster min ready

		//calculate cluster max
		max = m_vuMaxBuffer[n + mXn];
		if( max < m_vuMaxBuffer[n + (mXn + m_uN)] )
		{
			max = m_vuMaxBuffer[n + (mXn + m_uN)];
		}
		//cluster max ready

		if( (max - min) < m_uR )
		{
			thresholdHere = ostuT;
		}
		else
		{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//apply threshold to 1/4 of the pixel in this cluster (central ones)
		//for(y = mXs + sDIV2, yXwidth = y*width; y < (m+1) * m_uS + sDIV2; ++y, yXwidth += width)
		x = nXs + sDIV2;
		for(y = mXs + sDIV2, yXwidth = y*width; y < (mXs + m_uS) + sDIV2; ++y, yXwidth += width)
		{
			memset(threshold + x + yXwidth, thresholdHere, sDIV2);
		}
	//end of cluster
	}




	//bottom right corner
	mXn = m_uN*m_uM;
	//calculate cluster min
	min = m_vuMinBuffer[mXn-1];
	//cluster min ready

	//calculate cluster max
	max = m_vuMaxBuffer[mXn-1];
	//cluster max ready

	if( (max - min) < m_uR )
	{
		thresholdHere = ostuT;
	}
	else
	{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//apply threshold to 1/4 of the pixel in this cluster (central ones)
	x = (m_uN-1) * m_uS + sDIV2;
	for(y = (m_uM-1) * m_uS + sDIV2, yXwidth = y*width; y<height; ++y, yXwidth += width )
	{
		memset(threshold + x + yXwidth, thresholdHere, sDIV2);
	}
	//end of cluster




	//bottom row
	m = m_uM-1;
	mXs = m * m_uS;
	for(n = 0, mXn = 0, nXs = 0; n < m_uN-1; ++n, mXn += m_uM, nXs += m_uS )
	{
		//calculate cluster min
		min = m_vuMinBuffer[n + mXn];
		if( min > m_vuMinBuffer[(n+1) + mXn] )
		{
			min = m_vuMinBuffer[(n+1) + mXn];
		}
		//cluster min ready

		//calculate cluster max
		max = m_vuMaxBuffer[n + mXn];
		if( max < m_vuMaxBuffer[(n+1) + mXn] )
		{
			max = m_vuMaxBuffer[(n+1) + mXn];
		}
		//cluster max ready

		if( (max - min) < m_uR )
		{
			thresholdHere = ostuT;
		}
		else
		{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//apply threshold to 1/4 of the pixel in this cluster (central ones)
		x = mXs + sDIV2;
		for(y = mXs + sDIV2, yXwidth = y*width; y<height; ++y, yXwidth += width )
		{
			memset(threshold + x + yXwidth, thresholdHere, sDIV2);
		}

	//end of cluster
	}




	//bottom left corner
	min = m_vuMinBuffer[(m_uM-1) * m_uN]; //0+(m_uM-1)*m_uN
	max = m_vuMaxBuffer[(m_uM-1) * m_uN]; //0+(m_uM-1)*m_uN

	if( (max - min) < m_uR )
	{
		thresholdHere = ostuT;
	}
	else
	{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//apply threshold to 1/4 of the pixel in this cluster (central ones)
	for(y = (m_uM-1)*m_uS + sDIV2, yXwidth = y*width; y < height; ++y, yXwidth += width )
	{
		memset(threshold + yXwidth, thresholdHere, sDIV2);
	}
	//end of cluster




	//leftmost column
	for(m = 0, mXn = 0, mXs = 0; m < m_uM-1; ++m, mXn += m_uN, mXs += m_uS )
	{
		//calculate cluster min
		min = m_vuMinBuffer[mXn];
		if( min > m_vuMinBuffer[mXn + m_uN] )
		{
			min = m_vuMinBuffer[mXn + m_uN];
		}
		//cluster min ready

		//calculate cluster max
		max = m_vuMaxBuffer[mXn];
		if( max < m_vuMaxBuffer[mXn + m_uN] )
		{
			max = m_vuMaxBuffer[mXn + m_uN];
		}
		//cluster max ready

		if( (max - min) < m_uR )
		{
			thresholdHere = ostuT;
		}
		else
		{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//apply threshold to 1/4 of the pyxel in this cluster (central ones)
		for(y = mXs + sDIV2, yXwidth = y*width; y < mXs + m_uS + sDIV2; ++y, yXwidth += width )
		{
			memset(threshold + yXwidth, thresholdHere, sDIV2);
		}
		//end of cluster
	}
}


int FiducialRecognition::thresholdOstu()
{
	int t=0;
	int histogramArea=0;
	int * l_Hist = histogram;
	for( int i=0; i< 256; i++ ) {
		t+=*l_Hist * i;
		histogramArea+=*l_Hist++;
	}
	t/=histogramArea;
	if( t<1 ) t++;
	if( t>254 ) t--;
	s[t] = sigma(histogram, t);
	s[t-1] = sigma(histogram, t-1);
	s[t+1] = sigma(histogram, t+1);

	if( !((s[t] > s[t+1]) && (s[t] > s[t-1])) ) {
		int delta = 0;
		if( (s[t] <= s[t+1]) ) delta = +1;
		if( (s[t] <= s[t-1]) ) delta = -1;
		if( (s[t] < s[t+1]) ) delta = +1;
		if( (s[t] < s[t-1]) ) delta = -1;
		while( (t>1 && t<253) && (s[t] <= s[t+delta]) ) {
			t += delta;
			s[t+delta] = sigma(histogram, t+delta);
		}
	}
	return t;
}


int FiducialRecognition::sigma( int *histo, int t )
{
	int i;
	long xAvg = 0;
	long x1Avg = 0;
	long w = 0; //= 32640; // 255 * 256 / 2
	long w1 = 0; //= t * (t+1) / 2;

	int * l_Hist = histo;
	for( i=0; i<t; i++ ) {
		x1Avg += i * *l_Hist;
		w1 += *l_Hist++;
	}
	xAvg = x1Avg;
	w = w1;
	l_Hist = histo;
	for( i=t; i<256; i++ ) {
		xAvg += i * *l_Hist;
		w += *l_Hist++;
	}


	double x1d = x1Avg;
	double xd = xAvg;
	double wd = w;
	double w1d = w1;

	double termA = (x1d/w1d - xd/wd);
	double termB = w1d/(wd-w1d);

	return (int) ( termA*termA*termB );
}

// end of file
