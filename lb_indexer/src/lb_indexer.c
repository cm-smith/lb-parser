/* ========================================================================== */
/* File: lb_indexer.c - LeaderBoard Indexer
 *
 * Author: Charles Michael Smith
 * Date: 7/1/2015
 *
 * Input: example: ./crawler [seedURL] [webPageDirectory] [maxWebPageDepth]
 *
 * Command line options:
 *
 * Output:
 *
 * Error Conditions:
 *
 * Special Considerations:
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>                           // printf
#include <stdlib.h>
#include <string.h>			     // memset
#include <curl/curl.h>                       // curl functionality
#include <sys/stat.h>			     // stat function
#include <sys/types.h>
#include <ctype.h>                           // tolower, isalpha


// ---------------- Local includes  e.g., "file.h"
#include "common.h"                          // common functionality
#include "web.h"                             // curl and html functionality
#include "utils.h"                           // utility functions
#include "file.h"			     // file utility

// ---------------- Constant definitions
#define SITE "http://frenzy.sparklinlabs.com/leaderboard?date=20131022"
#define SCORE_TAG "td class=\"Score\">"
#define KEY "Rank"
#define KEY_SIZE strlen(KEY)

// ---------------- Macro definitions
#define SAFEFREE(a) free(a); a=NULL;

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes
int GetScoreBoard(const char* doc, int pos, char **score);
void ParseFile(char *file, char* new_file_name);
int GetNextScore(char *html, int pos, char **result);
void RemoveWhitespace(char* str);

/* ========================================================================== */

int main(int argc, char* argv[])
{
	// check command line arguments
	if(argc != 2)
	{
		printf("indexer: incorrect number of variables\n"
			"please provide file containing leaderboard html, i.e.,\n"
			"./lb_indexer ./../lb_crawler/res/lb-2015-07-05.dat\n");
		return (EXIT_FAILURE);
	}

	// check validity of data file
	char *dat_file = argv[1];
	if(!IsFile(dat_file))
	{
		printf("lb_indexer: |%s| is not a valid data file\n", dat_file);
		return (EXIT_FAILURE);
	}


	// load the contents of the file into a buffer
	char *doc = LoadDocument(dat_file);
	char *word;
	int pos = 0;

	while((pos = GetNextWord(doc, pos, &word)) > 0 && strncmp(word, KEY, KEY_SIZE) != 0);

	if(strncmp(word, KEY, KEY_SIZE) != 0)
	{
		printf("Leaderboard not present with KEY: |%s|\n", KEY);
		SAFEFREE(word);
		SAFEFREE(doc);
		return (EXIT_FAILURE);
	}

	// TODO: extract scores from XML
	char *new_file_name = (char *)malloc(sizeof(char)*20);
	if(strlen(argv[1]) > 35)
	{
		sprintf(new_file_name, "./scores%.11s", &(argv[1][22]));
	}
	else
	{
		sprintf(new_file_name, "./scores-unknown-date");
	}

	FILE* score_file;
	score_file = fopen(new_file_name, "w");
/*
	int word_num = 2;

	while((pos = GetScoreBoard(doc, pos, &word)) > 0)
	{
		// create a copy of the word to index in inverted index
		char *word_copy = malloc(strlen(word)+1);
		memset(word_copy, 0, strlen(word)+1);
		strcpy(word_copy, word);

		word_num++;

		// add word to the index, accompanied by document identifier
		fprintf(score_file, "%s\t", word);
		
		if(word_num > 3 && atoi(word) != 0)
		{
			word_num = 0;
			fprintf(score_file, "\n");
		}

		// free buffer
		SAFEFREE(word);
	}
*/

	// TODO: MAKE A LOG FILE FOR DISTRIBUTION, NOT JUST SCORES...
	
	int rank = 1;
	while((pos = GetNextScore(doc, pos, &word)) > 0)
	{
		//printf("Rank: |%d|\tScore: |%s|\n", rank, word);
		fprintf(score_file, "%d.\t%s\n", rank, word);
		rank++;
		SAFEFREE(word);
	}

	fclose(score_file);

	SAFEFREE(word);
	SAFEFREE(doc);
	SAFEFREE(new_file_name);

	return (EXIT_SUCCESS);
}

int GetScoreBoard(const char* doc, int pos, char **score)
{
	const char *beg;		// beginning of score
	const char *end;		// end of score

	if(!doc) {
		return -1;
	}

	while(doc[pos] && !isalnum(doc[pos])) {
		// if we find a tag, i.e., <...tag...>, skip it
		if(doc[pos] == '<') {
			end = strchr(&doc[pos], '>');	// find the close

			if(!end || !(++end)) {		// ran out of html
				return -1;
			}

			pos = end - doc;		// skip forward
			
			continue;
		}

		pos++;					// just move forward
	}

	// ran out of html
	if (!doc[pos]) {
		// TODO: adjust error message
		return -1;
	}

	// we're at the beginning of a word
	beg = &(doc[pos]);

	// consume word
	while (doc[pos] && isalnum(doc[pos])) {
		pos++;
	}

	// if we go too far, back up one
	if(!doc[pos]) {
		pos--;
	}

	// we're at the end of a word
	end = &(doc[pos]);

	// allocate new word + '\0'
	*score = calloc(end - beg + 1, sizeof(char));
	if(!score) {					// for any reason...
		return -1;
	}

	// copy the new word
	strncpy(*score, beg, end - beg);

	return pos;
}

int GetNextScore(char *html, int pos, char **result)
{
	int bad_score = 0;			// is this score ill formatted?
	char *lnk;			// score tags
	char *score;			// pointer to score
	char *end;			// end of score

	// make sure we have text
	if(!html) return -1;

	// condense html, makes parsing easier
	if(pos == 0)
		RemoveWhitespace(html);

	// parse for 'score' tag
	do {
		// find tag <td class="Score">
		lnk = strcasestr(&html[pos], SCORE_TAG);

		// no more scores on this page
		if(!lnk) { result = NULL; return -1; }

		// find next score after tag
		score = (lnk + strlen(SCORE_TAG));

		// no more scores on this page
		if(!score) { result = NULL; return -1; }

		// find end of score
		end = strchr(lnk, '<');

		// if the score we have is outside the current tag, continue
		if(end && (end < score)) {
			bad_score = 1; pos += 17; continue;
		}

		// if we don't know where to end the score, continue
		if(!end) {
			bad_score = 1; pos += 17; continue;
		}
	} while(bad_score);		// keep parsing

	// create a new buffer
	*result = calloc(end-score+1, sizeof(char));

	if(!*result) { return -2; }	// check memory

	// copy over score
	strncpy(*result, score, end - score);

	// return position at the end of the url
	//printf("New position: end|%p| - score|%p| = |%lu|\n", end, score, end-score);
	return end - html;
}

/*
 * RemoveWhitespace - removes whitespace from str
 * @str: char buffer to modify
 *
 * Eliminates whitespace by shifting and condensing all the non-whitespace
 * toward the beginning of the buffer. This does not alter the size of the
 * buffer.
 *
 * Should have no use outside of this file, thus declared static.
 */
void RemoveWhitespace(char* str)
{
    char *prev;                              // previous whitespace
    char *cur;                               // current non-whitespace

    // start at beginning of str
    cur = prev = str;

    do {
        while(isspace(*cur)) cur++;          // consume any whitespace
    } while ((*prev++ = *cur++));            // condense to front of str
}

