#include <stdlib.h>
#include <memory.h>
#include "dataseg.h"


DataSegment* ds_create() {
	DataSegment* ds = (DataSegment*)malloc(sizeof(DataSegment*));
	if(ds) {
		ds->nBlocks = 0;
		ds->currBlock = 0;
		ds->dataBlocks = NULL;
	}
	return ds;
}

void ds_destroy(DataSegment* ds) {
	if(ds == NULL) return;

	for(int i = 0; i < ds->nBlocks; i++)
		free(ds->dataBlocks[i].data);
	free(ds->dataBlocks);
	free(ds);
}

int ds_add_data(DataSegment* ds, DATA_WORD* data, int size) {
	// No blocks to add to, something is wrong...
	if(ds->nBlocks == 0)	return -1;

	// Reallocing each time decreases efficiency but
	//   makes life a whole lot simpler
	DataBlock* db = &ds->dataBlocks[ds->currBlock];
	db->data = (DATA_WORD*)realloc(db->data, sizeof(DATA_WORD) * (db->size + size));
	memcpy(&db->data[db->size], data, size * sizeof(DATA_WORD));
	int loc = db->start + db->size;
	db->size += size;
	return loc;
}

void ds_add_block(DataSegment* ds, int start) {
	// Before adding a brand new block, maybe there already
	//   exists a block that starts at the same location. If so,
	//   there is no need to malloc more memory
	for(int i = 0; i < ds->nBlocks; i++) {
		if(ds->dataBlocks[i].start == start) {
			ds->currBlock = i;
			return;
		}
	}

	ds->dataBlocks = (DataBlock*)realloc(ds->dataBlocks, sizeof(DataBlock) * (ds->nBlocks + 1));
	ds->currBlock = ds->nBlocks;
	DataBlock* db = &ds->dataBlocks[ds->nBlocks++];
	db->data = NULL;
	db->size = 0;
	db->start = start;
}
