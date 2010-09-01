/*
 *  TwitterEntry.h
 *  SJTwitterSensor
 *
 *  Created by brenfer on 7/1/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

struct authorStruct {
	string name;
	string url;
};

class TwitterEntry
{
public:	
	TwitterEntry(){
		id = "";
		entry = "";
		published = "";
		author.name = "";
		author.url = "";
	}
	
	string id;
	string entry;
	string published;
	authorStruct author;
};