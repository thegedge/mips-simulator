#ifndef __TOKENS_H__
#define __TOKENS_H__


#define TOKEN_ERROR		-1	// unknown token
#define TOKEN_STRING	1	// instruction or label argument
#define TOKEN_REGISTER	2	// register
#define TOKEN_IMMEDIATE	3	// immediate data
#define TOKEN_LABELDEF	4	// label definition
#define TOKEN_DIRECTIVE	5	// directive
#define TOKEN_COMMA		6	// comma separator
#define TOKEN_SDATA		7	// string data
#define TOKEN_WDATA		8	// word list data
#define TOKEN_EOF		101	// no more scanning possible


#endif // __TOKENS_H__
