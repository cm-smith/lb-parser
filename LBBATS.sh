#!/bin/bash
#
# File: LBBATS.sh
#
# Description: Executes both lb_crawler and lb_indexer, then creates a
# graph using the distribution file from lb_indexer
#
# Input: Date for the leaderboard to parse in format yyyy-mm-dd
#
# Output: A png graph file of distributions for leaderboard in d_graphs
# target directory, e.g., 'graph-2014-06-15.png'

export make=/usr/bin/make
export lb_crawler=./lb_crawler/lb_crawler
export lb_indexer=./lb_indexer/lb_indexer

# validate that a date was passed for the program
if [ $# -gt 1 ]
then
	echo "User: please only provide one date in format yyyy-mm-dd" 1>&2
	exit 0
fi

# validate a target directory to put resultant graph
if [ ! -e "d_graphs" ]
then
	echo "Target directory 'd_graphs' must exist for resultant files"
	exit 1
fi

# validate that the target directory is a directory
if [ ! -d "d_graphs" ]
then
	echo "'d_graphs' must be a directory for resultant files"
	exit 1
fi

# make sure there is a file to make the graph
if [ ! -e "plot.gnu" ]
then
	echo "File 'plot.gnu' must be accessible in this directory for plotting a distribution graph"
	exit 2
fi

# use the passed argument as the leaderboard date
if [ $# -eq 1 ]
then
	lbdate=`echo $@`
# or, if nothing was passed, parse today's leaderboard
elif [ $# -eq 0 ]
then
	lbdate=`date "+%Y-%m-%d"`
	echo "The default leaderboard will be for today ${lbdate}"
	#echo "The default leaderboard will be for today `date \"+%Y-%m-%d\"`"
fi

# execute the lb_crawler program with the passed date
cd ./lb_crawler && make && ./lb_crawler -d $lbdate && make clean && cd ..

# check to see if the program succeeded
if [ $? -ne 0 ]
then
	echo "Was not able to make a distribution graph– error in lb_crawler"
	exit 3
fi

# execute the lb_indexer program
cd ./lb_indexer && make && ./lb_indexer ../lb_crawler/res/lb-${lbdate}.dat && make clean && cd ..

# check to see if the program succeeded
if [ $? -ne 0 ]
then
	echo "Was not able to make a distribution graph– error in lb_indexer"
	exit 4
fi

# plot the new distribution file using Gnuplot and plot.gnu script
gnuplot -e "filename='./lb_indexer/d_res/dist-${lbdate}.txt';ofilename='./d_graphs/graph-${lbdate}.png'" plot.gnu

# provide success message
echo "Distribution graphed for ${lbdate}"

exit 0
