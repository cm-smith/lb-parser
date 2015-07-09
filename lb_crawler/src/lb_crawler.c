/* ========================================================================== */
/* File: lb_crawler.c - LeaderBoard Crawler
 *
 * Author: Charles Michael Smith
 * Date: 7/1/2015
 *
 * Input: example: ./lb_crawler -d 2014-03-23
 * 
 * Command line options:
 * 	-d [date of leaderboard]: which leaderboard to parse yyyy-mm-dd
 * 	TODO: -t [target directory]: where to put resultant files
 *
 * Output: '.dat' file that contains html with leaderboard data obtained
 * from the online game 'Frenzy', e.g., 'lb-2013-08-19.dat'. The file
 * will be placed in current directory.
 *
 * Error Conditions:
 *
 * Special Considerations:
 * 	o TODO: Currently there's no way to check if date is in range
 * 	(URL exists for every date combination for some reason...)
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>                           // printf
#include <stdlib.h>
#include <string.h>			     // strtok
#include <curl/curl.h>                       // curl functionality
#include <sys/stat.h>			     // stat function
#include <sys/types.h>
#include <getopt.h>			     // getopt for switches
#include <time.h>			     // date specifics

// ---------------- Local includes  e.g., "file.h"
#include "lb_crawler.h"

// ---------------- Constant definitions
#define SITE "http://frenzy.sparklinlabs.com/leaderboard?date="
//#define SITE "http://www.limasky.com/doodlejump/leaderboard/?d=1"

#define TAR_DIR "res"			// target directory

// ---------------- Macro definitions
#define SAFEFREE(a) free(a); a=NULL;

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* ========================================================================== */

int main(int argc, char* argv[])
{

	// setup seed page
	char *seed = malloc(sizeof(char)*strlen(SITE)+13);
	memset(seed, 0, sizeof(char)*strlen(SITE)+13);
	strcpy(seed, SITE);

	// setup date string
	char *lb_date = malloc(sizeof(char)*13);
	memset(lb_date, 0, sizeof(char)*13);

	// use switches for user input (date of leaderboard)
	int c;
	char *date;
	while((c = getopt(argc, argv, "d:")) != -1)
	{
		switch(c)
		{
			case 'd':
				// parse through the user input string
				date = strtok(optarg, "-");
				
				// concatenate the correct URL to parse and build the date
				while(date != NULL)
				{
					strcat(lb_date, date);

					date = strtok(NULL, "-");
				}
				
				// check authenticity of date
				// TODO: perform this better
				if(strlen(lb_date) != 8)
				{
					printf("lb_crawler: please have date input in following format: yyyy-mm-dd\n");
					SAFEFREE(seed);
					SAFEFREE(lb_date);
					return (EXIT_FAILURE);
				}

				break;
			case '?':
				helpStatus();
				SAFEFREE(seed);
				SAFEFREE(lb_date);
				return (EXIT_FAILURE);
		}
	}

	// make sure the user gave a date for the leaderboard
	if(strlen(lb_date) == 0 || lb_date == NULL)
	{
		defaultDate(lb_date);
		
	}

	// check whether the html requires concatenation
	if(strncmp(SITE, "http://frenzy.", 14) == 0)
	{
		strcat(seed, lb_date);
	}

	// check for a results target directory
	// TODO: make name of target directory optional
	char *target_dir = TAR_DIR;
	struct stat buf;
	if((stat(target_dir, &buf) == -1) || !(S_ISDIR(buf.st_mode)))
	{
		printf("lb_crawler: please make a valid 'res' target directory for resultant files\n");
		SAFEFREE(seed);
		SAFEFREE(lb_date);
		return(EXIT_FAILURE);
	}

	// init curl
	curl_global_init(CURL_GLOBAL_ALL);

	// setup seed page
	WebPage *seedURL = createWebPage(seed);

	// TODO: Need a Prefix checker?
	
	// check validity of seed url and get seed webpage
	if(!GetWebPage(seedURL))
	{
		printf("lb_crawler: there was a problem retrieving the site html\n"
			"check internet connection or |%s| may not be a valid url\n", seed);
		SAFEFREE(seed);
		SAFEFREE(lb_date);
		return (EXIT_FAILURE);
	}
	
	// make a file name formatted with the date	
	char *file_name = (char *)malloc(sizeof(char)*30 + strlen(target_dir));
	memset(file_name, 0, sizeof(char)*30 + strlen(target_dir));
	sprintf(file_name, "./%s/lb-%.4s-%.2s-%s.dat", target_dir, lb_date, &(lb_date[4]), &(lb_date[strlen(lb_date)-2]));

	// write resulting html file
	writeFile(file_name, seedURL);

	// cleanup
	SAFEFREE(file_name);
	SAFEFREE(lb_date);
	SAFEFREE(seedURL->html);
	SAFEFREE(seedURL);

	// cleanup curl
	curl_global_cleanup();


	return (EXIT_SUCCESS);
}

/* give a helpful status to user on input */
void helpStatus()
{
	printf("lb_crawler: please pass the date of the leaderboard to pass\n"
		"the range of dates begins on August 11, 2013 up to the current date\n"
		"use the '-d' switch followed by the date, like so:\n"
		"./lb_crawler -d 2013-08-11\n");
	
	return;
}

/* make the default leaderboard for today's date */
void defaultDate(char *lb_date)
{
	// find current year, month, day
	time_t now;
	struct tm *tp;
	time(&now);
	tp = gmtime(&now);
	int year = 1900 + tp->tm_year;
	int month = 1 + tp->tm_mon;
	int day = tp->tm_mday;
	
	// add zeros accordingly
	if(day < 10 && month < 10)
	{
		sprintf(lb_date, "%d0%d0%d", year, month, day);
	}
	else if(day < 10 && month >= 10)
	{
		sprintf(lb_date, "%d%d0%d", year, month, day);
	}
	else if(day >= 10 && month < 10)
	{
		sprintf(lb_date, "%d0%d%d", year, month, day);
	}
	else
	{
		sprintf(lb_date, "%d%d%d", year, month, day);
	}

	printf("The default leaderboard will be for today %.4s-%.2s-%s\n", lb_date, &(lb_date[4]), &(lb_date[strlen(lb_date)-2]));

	return;
}

/* create a WebPage struct for given URL */
WebPage *createWebPage(char *URL)
{

	WebPage* wp = (WebPage *)malloc(sizeof(WebPage));
	memset(wp, 0, sizeof(WebPage));
	wp->url = URL;
	return wp;
}

/* write the file to target directory using specified file_name and webpage */
void writeFile(char *file_name, WebPage *page)
{
	FILE *outfile;
	outfile = fopen(file_name, "w");
	fprintf(outfile, "%s\n\n%s", page->url, page->html);
	fclose(outfile);
	return;
}

