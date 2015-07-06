/* ========================================================================== */
/* File: common.h
 *
 * Project name: LeaderBoard Crawler
 *
 * This file contains the common defines and data structures.
 *
 */
/* ========================================================================== */
#ifndef COMMON_H
#define COMMON_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include <stddef.h>                          // size_t

// ---------------- Constants
#define INTERVAL_PER_FETCH 1                 // seconds between fetches

// limit crawling to only this domain
#define URL_PREFIX "http://old-www.cs.dartmouth.edu/~cs50/tse"

// ---------------- Structures/Types

typedef struct WebPage {
    char *url;                               // url of the page
    char *html;                              // html code of the page
    size_t html_len;                         // length of html code
} WebPage;

// ---------------- Public Variables

// ---------------- Prototypes/Macros

#endif // COMMON_H
