#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv)
{
	FILE* fin = NULL, *fout = NULL;
	if(argc <= 2) {
		printf("Usage: assembler [inputfile] [outputfile]");
		return 0;
	}

	if((fin = fopen(argv[1], "rt")) == NULL) {
		printf("Usage: assembler [inputfile] [outputfile]");
		return 0;
	}

	Parser* p = parser_create(fin);
	if(p == NULL) {
		fprintf(stderr, "Unable to create parser!\n");
		return 0;
	}

	if(parser_parse(p)) {
		if((fout = fopen(argv[2], "wb")) == NULL) {
			printf("Usage: assembler [inputfile] [outputfile]\n");
			return 0;
		}

		// Parse successful! Write the file
		fwrite(&p->ds->nBlocks, sizeof(int), 1, fout);
		DataBlock* db = p->ds->dataBlocks;
		for(int i = 0; i < p->ds->nBlocks; i++, db++) {
			fwrite(&db->start, sizeof(int), 1, fout);
			fwrite(&db->size, sizeof(int), 1, fout);
			if(db->size > 0)
				fwrite(db->data, sizeof(DATA_WORD), db->size, fout);
		}
		fwrite(&p->nData, sizeof(int), 1, fout);
		fwrite(p->data, sizeof(INSTR_WORD), p->nData, fout);

		fprintf(stderr, "Parse successful!\n");
	}
	else {
		fprintf(stderr, "Error on line %d!\n\n", p->lex->state.line);
	}

	fclose(fin);
	fclose(fout);

	return 1;
}
