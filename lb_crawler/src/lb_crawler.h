/* ========================================================================== */
/* File: lb_indexer.h
 *
 * Project name: lb_indexer
 *
 */
/* ========================================================================== */
#ifndef LB_CRAWLER_H
#define LB_CRAWLER_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "./../../util/src/common.h"         // common functionality
#include "./../../util/src/web.h"            // curl and html functionality
#include "./../../util/src/utils.h"          // utility functions

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

void helpStatus();

void defaultDate(char *lb_date);

WebPage *createWebPage(char *URL);

void writeFile(char *file_name, WebPage *page);

void ParseFile(char *file, char* new_file_name);

#endif //LB_CRAWLER_H
