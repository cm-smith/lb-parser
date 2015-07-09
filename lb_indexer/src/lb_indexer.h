/* ========================================================================== */
/* File: lb_indexer.h
 *
 * Project name: lb_indexer
 *
 */
/* ========================================================================== */
#ifndef LB_INDEXER_H
#define LB_INDEXER_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "./../../util/src/common.h"         // common functionality
#include "./../../util/src/web.h"            // curl and html functionality
#include "./../../util/src/utils.h"          // utility functions
#include "./../../util/src/file.h"	     // file utility

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

int checkDirs(char *s_dir, char *d_dir);

int GetScoreBoard(const char* doc, int pos, char **score);

void ParseFile(char *file, char* new_file_name);

int GetNextScore(char *html, int pos, char **result);

int RemoveWhitespace(char* str);

#endif //LB_INDEXER_H
