#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iomanip>
#include <unordered_set>
#include <set>
#include <limits.h>

using namespace std;

#define FREQ 2.60e9

using namespace std;

void suffixArray(int *s, int *SA, int n, int K);
void make_lcp(int *sa, int *lcp, const vector<int> &text, int n);
void make_lcplr(int *lcp, int *lcplr, int i, int low, int high);
int find_rightposition(const string &ipstring, const string &query, int *SA, int *lcplr);
int find_leftposition(const string &ipstring, const string &query, int *SA, int *lcplr);
int find_rightposition(const string &ipstring, const string &query, int *SA, int *lcplr, int &max_m);
int find_leftposition(const string &ipstring, const string &query, int *SA, int *lcplr, int &max_m);

string normalize(const string &query, const unordered_set<string> &swords);
vector<pair<int, int>> inverse_map(string ipstring);

void q1query(const string &query, const unordered_set<string> &stopwords, const string &ipstring, int *SA, int *lcplr, const vector<pair<int, int>> &imap, const int &no_of_rows, string &longest_sub);



void doc_rank(const string &query, const unordered_set<string> &stopwords, const string &ipstring, int *SA, int *lcplr, const vector<pair<int, int>> &imap, const int &no_of_rows,  const vector<string> &title, const vector <pair<string,string>> &doc_string);

unsigned long long rdtsc();

int main()
{

    // ifstream file1("clean1.txt");
    ifstream file1("clean_test1.txt");
    // ofstream file2("output.txt");

    ifstream swords("stopwords.txt");

    unordered_set<string> stopwords;

    vector<pair<string, string>> doc_string;

    vector <string> title;
    vector<int> int_string;

    vector<int> num_words;
    // vector<string> arr;

    string temp1, temp2;

    string ipstring;

    while (getline(swords, temp1))
    {
        stopwords.insert(temp1);
    }

    for (int i = 0; i < 8294/2; i++)
    {
        getline(file1, temp1);
        getline(file1, temp2);

        title.push_back(normalize(temp1, stopwords));
        num_words.push_back(temp2.length());
        // cout << temp1 << "\n";
        // cout << title[title.size() - 1] << "\n";
        ipstring += temp2;
        ipstring += 1;
        doc_string.push_back(make_pair(temp1, temp2));
    }

    // for(int i=0;i<doc_string.size();i++){
    //     cout<<doc_string[i].first<<"\n"<<doc_string[i].second<<"\n\n";
    // }

    int no_of_rows = doc_string.size();

    int k = 1;
    for (int i = 0; i < no_of_rows; i++)
    {
        for (int j = 0; j < doc_string[i].second.length(); j++)
        {
            int_string.push_back(doc_string[i].second[j] + no_of_rows + 1);
        }
        int_string.push_back(k++);
    }

    vector<pair<int, int>> imap = inverse_map(ipstring);

    int alphabet = 256 + no_of_rows;

    int n = int_string.size();
    int *s = new int[n + 3];
    int *SA = new int[n + 3];
    for (int i = 0; i < n + 3; i++)
    {
        s[i] = int_string[i];
        SA[i] = 1;
    }
    s[n] = s[n + 1] = s[n + 2] = SA[n] = SA[n + 1] = SA[n + 2] = 0;
    unsigned long long int t0sa, t1sa, t0esa, t1esa;

    t0sa = rdtsc(); //start timer

    suffixArray(s, SA, n, alphabet);
    t1sa = rdtsc(); //stop timer

    cout << std::setprecision(9) << std::showpoint << std::fixed;
    // file2 << "Time to construct suffix array is: " << (t1sa - t0sa) / FREQ << "\n";

    int *lcp = new int[n];

    int temp = pow(2, ceil(log2(n - 1)) + 1);
    if (temp < 100)
    {
        temp = 100;
    }
    int *lcplr = new int[temp];
    for (int i = 0; i < temp; i++)
    {
        lcplr[i] = 0;
    }

    t0esa = rdtsc(); //start timer

    make_lcp(SA, lcp, int_string, n);
    make_lcplr(lcp, lcplr, 0, 0, n - 1);

    t1esa = rdtsc(); //stop timer

    cout << std::setprecision(9) << std::showpoint << std::fixed;
    // file2 << "Time to construct enhancements for suffix array is: " << (t1esa - t0esa) / FREQ << "\n";
    cout << ((t1esa - t0esa) + (t1sa - t0sa)) / FREQ << "\n";
    int t1;
    cin >> t1;
    string query;
    getline(cin, query);
    // for (int i3 = 0; i3 < t1; i3++)
    // {
    while (getline(cin, query))
    {
        
        //Removing spaces;
        while(query[query.length()-1]==' '){
            query.pop_back();
        }
        string longest_sub;
        q1query(query, stopwords, ipstring, SA, lcplr, imap, no_of_rows, longest_sub);

        doc_rank(query, stopwords, ipstring, SA, lcplr, imap, no_of_rows, title, doc_string);
    }

    delete[] s;
    delete[] SA;
    delete[] lcp;
    delete[] lcplr;

    return 0;
}
