/*
 *  Columns.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 8/24/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "Column.h"

class Columns
{
public:
	
	//setup with default variables
	Columns(){
		setNumColumns( 5, 100.0f, 100.0f);		
	};
	
	int getNumColumns(){ return numColums; };	
	float getSpacing(){ return colSpacing; };
	float getBorder(){ return colBorder; };
		
	void setNumColumns( int number, float spacing, float border=0.){
		column_vect.clear();
		
		numColums	= number;
		colSpacing	= spacing;
		colBorder	= border;
		
		for (int i=0; i<number; i++){
			Column c = Column();
			c.x = border+spacing*i;
			c.y = 0;
			column_vect.push_back(c);
		}
	};
	
	//pretty self explanatory
	Column getClosestColumn( float x ){
		float dist = 99999999.9f;
		
		int closestIndex = -1;
		
		for (int i=0; i<column_vect.size(); i++){
			if (fabs(column_vect[i].x - x) < dist){
				closestIndex = i;
			};
		};
		return column_vect[closestIndex];
	};
	
protected:
	
	int numColums;
	float colSpacing;
	float colBorder;
	
	vector<Column> column_vect;
	
};