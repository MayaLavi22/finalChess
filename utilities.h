#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "map.h"

MapKeyElement copyId(MapKeyElement id);

void freeId(MapKeyElement id);

int compareIds(MapKeyElement id1, MapKeyElement id2);
    
#endif /* UTILITIES */

