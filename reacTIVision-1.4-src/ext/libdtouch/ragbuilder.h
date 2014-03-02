/***************************************************************************
                          RAGBuilder.h  -  description
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
#ifndef EC_RAGBUILDER
#define EC_RAGBUILDER

#include "regionadjacencygraph.h"

class RAGBuilder{
protected:
	unsigned char *_image;

	int _width;
	int _height;

	unsigned char *_threshold;

	RegionAdjacencyGraph *_rag;
	bool *_stored;
  

	RAGBuilder() {}
public:
	RAGBuilder( unsigned char *in_image, int in_width, int in_height,
	unsigned char *in_threshold, RegionAdjacencyGraph *in_rag, bool *in_stored );
	~RAGBuilder();

	int buildRAG( const int &maxRegions=3000 );
	int buildRAGBorder( const int &maxRegions=3000 );
	int buildRAGFullBorder( const int &maxRegions=3000 );
};

#endif

