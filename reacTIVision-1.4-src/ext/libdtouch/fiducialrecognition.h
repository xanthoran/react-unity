/***************************************************************************
                       fiducialrecognition.h  -  description
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

*/

#ifndef EC_FIDUCIALRECOGNITION
#define EC_FIDUCIALRECOGNITION

#include "pool.h"
#include "listhash.h"
#include "point.h"
#include "region.h"

class FiducialRecognition{
protected:

	unsigned int width;
	unsigned int height;

	int histogram[256];
	//int histogramSum;

	// !!! JMS - BEGIN
	void setMinMaxBuffersForThreshold(unsigned char *img);
	void setSubClustersMinMax(unsigned int uOstuThreshold);

	unsigned int uHistogramAccum[256];
	unsigned int uHistogramAvg[256];

	unsigned int *m_vuMinBuffer;
	unsigned int *m_vuMaxBuffer;

	unsigned int m_uD;
	unsigned int m_uR;
	unsigned int m_uS;

	unsigned int m_uM;
	unsigned int m_uN;

	// !!! JMS - END


	int s[256];
	int thresholdOstu();
	int sigma( int *histo, int t );

	int maskSize;
	int k;
	int R;

	unsigned char *threshold;

	int noFiducialSequences;
	int **fiducialSequence;
	bool *usingLinearFiducials;

	void setBernsenThreshold( unsigned char *img);
  
	int _graphSize;
	StaticPool<DTPoint> *_regionsPool;
	ListHash<int> **_edges;
	DTRegion **_region;
	int **_labelsMap;
	int *_labels;
	bool *_stored;
	Pool<int *> *_labelsPtrPool;

	FiducialRecognition() { }

public:
	FiducialRecognition( int in_width, int in_height, 
		int *in_fiducialSequence=NULL, bool in_usingLinearFiducials=true );

	FiducialRecognition( int in_width, int in_height, 
		int in_noFiducialSequences, int **in_fiducialSequence, 
		bool *in_usingLinearFiducials );
	
	~FiducialRecognition();
	int process( unsigned char *img, unsigned char *thresh, FiducialData **fiducialData );
};

#endif


