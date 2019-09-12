The steps for relevance are in report.odt

clean1.txt contains the input file. It's taken through a file descriptor.
code.cpp contains the functions
main.cpp takes the input and calls functions in code.cpp
query.txt contains the query.
stopwords.txt contains the stopwords

To execute,
g++ code.cpp main.cpp
./a.out < query.txt > op

First line of output is time for suffix array construction.
Followed by the results for questions 1 and 2.

The format of scores for document are:
Title of document:score
Paramater with highest score:score

The highest parameter index is the same as in report.odt