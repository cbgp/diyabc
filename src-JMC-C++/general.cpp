/*
 * general.cpp
 *
 *  Created on: 25 jan. 2010
 *      Author: cornuet
 */

#include <fstream>
#include <iostream>
#include "simuldatC.cpp"
#include <string.h>


char *dir,*headerfilename,*datafilename,*logfilename;


int main(int argc, char *argv[])
{
	std::cout << argv[1]<<"\n";
	headerfilename = new char[strlen(argv[1])+12];
	strcpy(headerfilename,argv[1]);
	strcat(headerfilename,"header.txt");
	std::cout << headerfilename<<"\n";
	HeaderC header;
	header.readHeader(headerfilename);
	cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";
	return 0;
}
