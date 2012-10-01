#ifndef __DATASEG_H__
#define __DATASEG_H__


#include "../../include/types.h"



typedef struct _DataBlock {
	int size;			// size of datablock (in DATA_WORDs)
	int start;			// start position of datablock
	DATA_WORD* data;	// data contained in this data block
} DataBlock;

typedef struct _DataSegment {
	int nBlocks;			// the number of datablocks
	int currBlock;			// the currently referenced datablock
	DataBlock* dataBlocks;	// an array of datablocks
} DataSegment;


// Create a new data segment
DataSegment* ds_create();
// Destroy an existing datasegment
void ds_destroy(DataSegment* ds);
// Add data to the currently referenced datablock
int ds_add_data(DataSegment* ds, DATA_WORD* data, int size);
// Add a new data block. Note that if the start position
//   already exists for a datablock, that datablock will be used
void ds_add_block(DataSegment* ds, int start);


#endif // __DATASEG_H__
