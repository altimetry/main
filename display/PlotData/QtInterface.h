#ifndef QT_INTERFACE_H
#define QT_INTERFACE_H

#include "BratLookupTable.h"


bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName );

void SaveToFile( CBratLookupTable &lut, const std::string& fileName );



#endif		//QT_INTERFACE_H
