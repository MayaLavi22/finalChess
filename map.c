#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"

typedef struct node{
    MapKeyElement key;
    MapDataElement data;
    struct node* next;
} *Node;

struct Map_t
{
    struct node* head;
    struct node* i;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
};

Node createNode(Map map, MapKeyElement key,MapDataElement data)
{
    if(map== NULL||key==NULL||data==NULL){
        return NULL;
    }
    Node ptr=malloc(sizeof(*ptr));
    if(!ptr){
        return NULL;
    }
    MapKeyElement key_copy=map->copyKeyElement(key);
    MapDataElement data_copy=map->copyDataElement(data);
    if(key_copy==NULL || data_copy==NULL){
        map->freeDataElement(data_copy);
        map->copyKeyElement(key_copy);
        free(ptr);
        return NULL;
    }
    ptr->data=data_copy;
    ptr->key=key_copy;
    ptr->next=NULL;
    return ptr;
}
void destroyList(Node ptr)
{
	while(ptr) {
		Node toDelete = ptr;
		ptr = ptr->next;
		free(toDelete);
	}
}

Node createList(int size)
{
    Node new_list=NULL;
    Node tail=NULL;
    for (int i=0;i<size;i++){
        Node temp=malloc(sizeof(*temp));
        if(temp==NULL){
            destroyList(new_list);
            return NULL;
        }
        temp->next=NULL;
        if(new_list==NULL){ 
            new_list=temp;
            tail=new_list;
        }
        else{
            tail->next=temp;
            tail=tail->next;
        }
    }
    return new_list;
}

Map mapCreate(copyMapDataElements copyDataElement,copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,freeMapKeyElements freeKeyElement,compareMapKeyElements compareKeyElements)
              {
                    if(copyDataElement==NULL||copyKeyElement==NULL||
                        freeDataElement==NULL||freeKeyElement==NULL||compareKeyElements==NULL){
                            return NULL;
                    }
                    Map map=malloc(sizeof(*map));
                    if(map==NULL){
                        return NULL;
                    }
                    map->head=NULL;
                    map->i=NULL;
                    map->copyDataElement=copyDataElement;
                    map->copyKeyElement=copyKeyElement;
                    map->freeDataElement=freeDataElement;
                    map->freeKeyElement=freeKeyElement;
                    map->compareKeyElements=compareKeyElements;
                    return map;
}

int mapGetSize(Map map)
{
    if(map==NULL){
        return -1;
    }
    int count=0;
    map->i=map->head;
    while(map->i){
        count++;
        map->i=map->i->next;
    }
    return count;
}

MapKeyElement mapGetFirst(Map map)
{
    if(map==NULL){
        return NULL;
    }
    map->i=map->head;
    if(map->i==NULL){
        return NULL;
    }
    MapKeyElement copy=map->copyKeyElement(map->i->key);
    if (copy==NULL){
        return NULL;
    }
    return copy;
}

MapKeyElement mapGetNext(Map map)
{
    if(map==NULL){
        return NULL;
    }
    if (map->i==NULL){
        return NULL;
    }
    map->i=map->i->next;
    if(map->i==NULL){
        return NULL;
    }
    MapKeyElement copy=map->copyKeyElement(map->i->key);
    if (copy==NULL){
        return NULL;
    }
    return copy;
}

bool mapContains(Map map, MapKeyElement element)
{
    if(map==NULL||element==NULL){
        return false;
    }
    map->i=map->head;
    if(map->i==NULL){//map is empty
        return false;
    }
    while(map->i){
        if(map->i->key==NULL){
            map->i=map->i->next;
        }
        if (!(map->compareKeyElements(element,map->i->key))){
            return true;
        }
        map->i=map->i->next;
    }
    return false;
}

Map mapCopy(Map map)
{
    if (map==NULL){
        return NULL;
    }
    Map copyMap=mapCreate(map->copyDataElement,
              map->copyKeyElement,
              map->freeDataElement,
              map->freeKeyElement,
              map->compareKeyElements);
    if(copyMap==NULL){
        return NULL;
    }
    int size=mapGetSize(map);
    copyMap->head=createList(size);
    /*if (copyMap->head==NULL){
        return NULL;
    }*/
    map->i=map->head;
    copyMap->i=copyMap->head;
    for (int i=0;i<size;i++){
        copyMap->i->data=map->copyDataElement(map->i->data);
        copyMap->i->key=map->copyKeyElement(map->i->key);
        copyMap->i=copyMap->i->next;
        map->i=map->i->next;
    }
    return copyMap;
}
MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    Node curr_node=map->head;
    if(curr_node==NULL){
        return NULL;
    }
    while (curr_node){
        if(map->compareKeyElements(curr_node->key,keyElement)==0){
            return curr_node->data;
        }
        curr_node=curr_node->next;
    }
    return NULL; 
}

MapResult mapClear(Map map)
{
    if(map==NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(map->head==NULL){
        return MAP_SUCCESS;
    }
    int map_len = mapGetSize(map);
    Node ptr=map->head;
    for(int i=0;i<map_len;i++){
        Node toDelete = ptr;
        ptr = ptr->next;
        MapResult remove = mapRemove(map,toDelete->key);
        if (remove!=MAP_SUCCESS){
            return MAP_ERROR;
        }
    }
    map->head=NULL;
    map->i=NULL;
    return MAP_SUCCESS;
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if (map==NULL||keyElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    
    if (map->head==NULL)//empty list
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    map->i=map->head;
    if(map->compareKeyElements(map->i->key,keyElement)==0){//remove the first
        map->head=map->head->next;
        map->freeKeyElement(map->i->key);
        map->freeDataElement(map->i->data);
        free(map->i);
        return MAP_SUCCESS;
    }
    Node ptr=map->i;
    map->i=map->i->next;
    if(map->i==NULL){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    if(map->i->key==NULL){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    while(map->compareKeyElements(map->i->key,keyElement)!=0){
        ptr=ptr->next;
        map->i=map->i->next;
        if(map->i==NULL){
            return MAP_ITEM_DOES_NOT_EXIST;
        }
        if(map->i->key==NULL){
        return MAP_ITEM_DOES_NOT_EXIST;
        }
    }
    ptr->next=map->i->next;
    map->freeKeyElement(map->i->key);
    map->freeDataElement(map->i->data);
    free(map->i);
    return MAP_SUCCESS;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if (map==NULL||keyElement==NULL||dataElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(mapContains(map,keyElement)){
        map->i=map->head;
        while(map->i){
            if(map->compareKeyElements(map->i->key,keyElement)==0){
                map->freeDataElement(map->i->data);
                MapDataElement copy=map->copyDataElement(dataElement);
                if(copy==NULL){
                    return MAP_OUT_OF_MEMORY;
                }
                map->i->data=copy;
                return MAP_SUCCESS;
            }
            map->i=map->i->next;
        }
        return MAP_ERROR;
    }
    else{
        map->i=map->head;
        Node ptr=createNode(map,keyElement,dataElement);
        if(ptr==NULL){
            return MAP_OUT_OF_MEMORY;
        }
        if(map->i==NULL){ //its the first node
            map->head=ptr;
            return MAP_SUCCESS;
        }
        Node temp=map->i;
        if(map->compareKeyElements(ptr->key,map->i->key)<0){//ptr smaller than head
            ptr->next=map->i;
            map->head=ptr;
            //free(ptr);
            return MAP_SUCCESS;
        }
        map->i=map->i->next;
        while(map->i){
            if(map->compareKeyElements(ptr->key,map->i->key)<0){
                temp->next=ptr;
                ptr->next=map->i;
                return MAP_SUCCESS;
            }
            map->i=map->i->next;
            temp=temp->next;
        }
        temp->next=ptr;
        return MAP_SUCCESS;
    }
}

void mapDestroy(Map map)
{
    if (map==NULL){
        return;
    }
    mapClear(map);
    free(map); 
}
