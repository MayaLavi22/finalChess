#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

MapKeyElement copyId(MapKeyElement id)
{
    if(id == NULL){
        return NULL;
    }

    int* copy = malloc(sizeof(*copy));
    if(copy==NULL){
        return NULL;
    }
    *copy = *(int*)id;
    return copy;
}

void freeId(MapKeyElement id)
{
    free(id);
}

int compareIds(MapKeyElement id1, MapKeyElement id2)
{
    return (*(int *) id1 - *(int *) id2);
}