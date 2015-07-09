/* ========================================================================== */
/* File: lb_indexer.c - LeaderBoard Indexer
 *
 * Author: Charles Michael Smith
 * Date: 7/1/2015
 *
 * Input: example: ./lb_crawler [leaderboard HTML file]
 *
 * Command line options:
 *
 * Output:
 *
 * Error Conditions:
 *
 * Special Considerations:
 * 	TODO: check to see if the file exists. if it does, provide message and bail...or ask the user if they would like to continue
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
#include <getopt.h>			     // switch options


// ---------------- Local includes  e.g., "file.h"
#include "lb_indexer.h"

// ---------------- Constant definitions
/*
#define SITE "http://www.limasky.com/doodlejump/leaderboard/?d=1"
#define SCORE_TAG "td align=\"right\" style=\"padding-left:4px\">"
#define KEY ""
*/
#define SITE "http://frenzy.sparklinlabs.com/leaderboard?date="//20131022"
#define SCORE_TAG "td class=\"Score\">"
#define KEY "Rank"

#define S_TAR_DIR "s_res"		// target directory for score files
#define D_TAR_DIR "d_res"		// target directory for distrib files
#define KEY_SIZE strlen(KEY)		// size of KEY
#define DISTRIB_RANGE 1000		// range for distribution

// ---------------- Macro definitions
#define SAFEFREE(a) free(a); a=NULL;

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

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

	// check for a results target directory
	// TODO: make name of target directory optional
	char *s_dir = S_TAR_DIR;
	char *d_dir = D_TAR_DIR;
	if(checkDirs(s_dir, d_dir))
	{
		return (EXIT_FAILURE);
	}

	// load the contents of the file into a buffer
	char *doc = LoadDocument(dat_file);
	char *word;
	int pos = 0;

	// parse through to the place in the HTML where the leaderboard begins
	while((pos = GetNextWord(doc, pos, &word)) > 0 && strncmp(word, KEY, KEY_SIZE) != 0);

	// if the leaderboard is not on this site, bail
	if(strncmp(word, KEY, KEY_SIZE) != 0)
	{
		printf("Leaderboard not present with KEY: |%s|\n", KEY);
		SAFEFREE(word);
		SAFEFREE(doc);
		return (EXIT_FAILURE);
	}

	// make file_names corresponding to leaderboard date
	char *score_file_name = (char *)malloc(sizeof(char)*30+strlen(s_dir));
	memset(score_file_name, 0, sizeof(char)*30);
	char *distr_file_name = (char *)malloc(sizeof(char)*30+strlen(d_dir));
	memset(distr_file_name, 0, sizeof(char)*30);

	//printf("Size of file_name is |%lu|\n", strlen(argv[1]));
	// check to see if files can concatenate with date of leaderboard parsed
	if(strlen(argv[1]) >= 35)
	{
		sprintf(score_file_name, "./%s/scores%.11s.txt", s_dir, &(argv[1][strlen(argv[1])-15]));
		sprintf(distr_file_name, "./%s/dist%.11s.txt", d_dir, &(argv[1][strlen(argv[1])-15]));
	}
	else
	{
		sprintf(score_file_name, "./scores-unknown-date");
		sprintf(distr_file_name, "./dist-unknown-date");
	}

	// keep a score file with just scores and ranks
	FILE* score_file;
	score_file = fopen(score_file_name, "w");
	
	// keep a distribution file with distribution of scores
	FILE* distr_file;
	distr_file = fopen(distr_file_name, "w");

	// keep track of the rank for score file
	int rank = 1;

	// to make distribution file
	int curr_distrib = -1, distrib = 1;
	int this;

	// to convert score from string to long
	long curr_score = 0;
	char* ptr;

	// while there is a score to retrieve
	while((pos = GetNextScore(doc, pos, &word)) > 0)
	{
		// if the word contains whitespace, ignore it
		// **REQUIRED FOR DOODLEJUMP LEADERBOARD PARSING**
		if(RemoveWhitespace(word))
		{
			continue;
		}

		//printf("Rank: |%d|\tScore: |%s|\n", rank, word);
		// print rank and score to score file
		fprintf(score_file, "%d.\t%s\n", rank, word);
		rank++;

		// while the word is a long (zero is invalid conversion)
		curr_score = strtol(word, &ptr, 10);
		if(curr_score != 0)
		{
			// see what distribution THIS score is in
			this = curr_score/DISTRIB_RANGE;
			//printf("The distribution for this score is in rank |%d|\n", this);
			
			// if THIS is not in the current distribution
			if(this != curr_distrib)
			{
				// and if THIS is not the first conversion
				if(curr_distrib != -1)
				{
					// print distribution range and amount in that range to file
					fprintf(distr_file, "%d %d\n", curr_distrib, distrib);
				}
				// adjust current distribution to this new range
				curr_distrib = this;
				distrib = 1;
			}
			// otherwise increment the distribution in this range
			else
			{
				distrib++;
			}
		}

		SAFEFREE(word);
	}

	// add the last distribution range to the file
	if(curr_distrib != -1)
	{
		fprintf(distr_file, "%d %d\n", curr_distrib, distrib);
	}

	// close the files
	fclose(score_file);
	fclose(distr_file);

	// cleanup
	SAFEFREE(word);
	SAFEFREE(doc);
	SAFEFREE(score_file_name);
	SAFEFREE(distr_file_name);

	return (EXIT_SUCCESS);
}

/* checkDirs- validates target directories for score and distribution 
 * resultant files 
 * @s_dir: the target directory for the score file
 * @d_dir: the target directory for the distrib file
 * Returns 0 if the directories are valid, and returns 1 if invalid
 */
int checkDirs(char *s_dir, char *d_dir)
{
	// initially, the status is zero
	int status = 0;

	// used in determining directory status
	struct stat buf1, buf2;

	// if target directory for score file is invalid
	if((stat(s_dir, &buf1) == -1) || !(S_ISDIR(buf1.st_mode)))
	{
		// provide a helpful message and change the return status
		printf("lb_indexer: please make a valid '%s' directory for score output files\n", s_dir);
		status = 1;
	}

	// if target directory for distrib file is invalid
	if((stat(d_dir, &buf2) == -1) || !(S_ISDIR(buf2.st_mode)))
	{
		// provide a helpful message and change the return status
		printf("lb_indexer: please make a valid '%s' directory for distribution output files\n", d_dir);
		status = 1;
	}
	
	// return the status of the directories
	return status;
}

/* GetScoreBoard- parses HTML and returns the position of the score board
 * in the document 
 * @doc: the HTML document to parse
 * @pos: the current position in the document
 * @score: the score board key to find in the document
 * Returns a positive number (pos) if a score board was successfully
 * found, and returns -1 otherwise
 */
int GetScoreBoard(const char* doc, int pos, char **score)
{
	const char *beg;		// beginning of score
	const char *end;		// end of score

	// validate that the document exists
	if(!doc) {
		return -1;
	}

	// while there is a non-alphanumeric character in the document
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

	// return the ending position in the HTML document
	return pos;
}

/* GetNextScore- parses HTML and returns the next score in the document
 * @doc: the HTML document to parse
 * @pos: the current position in the document
 * @score: the score found in the document
 * Returns a positive number (pos) if a score was successfully
 * found, and returns -1 otherwise
 */
int GetNextScore(char *html, int pos, char **result)
{
	int bad_score = 0;		// is this score ill formatted?
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
int RemoveWhitespace(char* str)
{
    char *prev;                              // previous whitespace
    char *cur;                               // current non-whitespace

    int status = 0;

    // start at beginning of str
    cur = prev = str;

    do {
        while(isspace(*cur))
	{
		status = 1;
		cur++;          // consume any whitespace
	}
    } while ((*prev++ = *cur++));            // condense to front of str

    return status;
}

