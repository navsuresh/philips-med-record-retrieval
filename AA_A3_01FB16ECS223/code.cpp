#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include <set>
#include <limits.h>
#include <numeric>
#include <fstream>

using namespace std;

bool sort2(const pair<int, double> &a, const pair<int, double> &b)
{
    return a.second > b.second;
}
inline bool leq(int a1, int a2, int b1, int b2)
{
    return (a1 < b1 || a1 == b1 && a2 <= b2);
}
inline bool leq(int a1, int a2, int a3, int b1, int b2, int b3)
{
    return (a1 < b1 || a1 == b1 && leq(a2, a3, b2, b3));
}
void c_sort(int *indices, int *SA, int *text, int n, int alphabet_size)
{

    int *counts = new int[alphabet_size + 1];
    for (int i = 0; i <= alphabet_size; i++)
        counts[i] = 0;
    for (int i = 0; i < n; i++)
        counts[text[indices[i]]]++;
    int temp;
    int sum = 0;
    for (int i = 0; i <= alphabet_size; i++)
    {
        temp = counts[i];
        counts[i] = sum;
        sum += temp;
    }
    for (int i = 0; i < n; i++)
        SA[counts[text[indices[i]]]++] = indices[i];
    delete[] counts;
}

// find the suffix array SA of s[0..n-1] in {1..K}^n
// require s[n]=s[n+1]=s[n+2]=0, n>=2
void suffixArray(int *text, int *SA, int n, int alphabet_size)
{

    int n0 = (n + 2) / 3;
    int n1 = (n + 1) / 3;
    int n2 = (n) / 3;
    int n02 = n0 + n2;

    int *text_0 = new int[n0];
    int *SA_0 = new int[n0];

    int *text_12 = new int[n02 + 3];
    int *SA_12 = new int[n02 + 3];

    text_12[n02] = text_12[n02 + 1] = text_12[n02 + 2] = 0;
    SA_12[n02] = SA_12[n02 + 1] = SA_12[n02 + 2] = 0;

    int j;
    j = 0;
    for (int i = 0; i < n + (n0 - n1); i++)
    {
        if (i % 3 != 0)
        {
            text_12[j++] = i;
        }
    }
    c_sort(text_12, SA_12, text + 2, n02, alphabet_size);
    c_sort(SA_12, text_12, text + 1, n02, alphabet_size);
    c_sort(text_12, SA_12, text, n02, alphabet_size);
    // find lexicographic names of triples
    int name = 0;
    int temp0 = -1;
    int temp1 = -1;
    int temp2 = -1;

    for (int i = 0; i < n02; i++)
    {
        if (text[SA_12[i]] != temp0 || text[SA_12[i] + 1] != temp1 || text[SA_12[i] + 2] != temp2)
        {
            name++;
            temp0 = text[SA_12[i]];
            temp1 = text[SA_12[i] + 1];
            temp2 = text[SA_12[i] + 2];
        }
        if (SA_12[i] % 3 == 1)
        {
            text_12[SA_12[i] / 3] = name;
        } // left half
        else
        {
            text_12[SA_12[i] / 3 + n0] = name;
        } // right half
    }
    // recurse if names are not yet unique
    if (name < n02)
    {
        // cout << "Inside IF\n";
        suffixArray(text_12, SA_12, n02, name);
        // store unique names in s12 using the suffix array
        for (int i = 0; i < n02; i++)
            text_12[SA_12[i]] = i + 1;
    }
    else // generate the suffix array of s12 directly
    {
        for (int i = 0; i < n02; i++)
            SA_12[text_12[i] - 1] = i;
    }

    j = 0;
    for (int i = 0; i < n02; i++)
    {
        if (SA_12[i] < n0)
        {
            text_0[j++] = 3 * SA_12[i];
        }
    }

    c_sort(text_0, SA_0, text, n0, alphabet_size);
    // merge sorted SA0 suffixes and sorted SA12 suffixes
    int t = n0 - n1;
    int p = 0;
    for (int k = 0; k < n; k++)
    {

        int i;
        if (SA_12[t] < n0)
        {
            i = SA_12[t] * 3 + 1;
        }
        else
        {
            i = (SA_12[t] - n0) * 3 + 2;
        }
        int j = SA_0[p];
        if (SA_12[t] < n0 ? leq(text[i], text_12[SA_12[t] + n0], text[j], text_12[j / 3]) : leq(text[i], text[i + 1], text_12[SA_12[t] - n0 + 1], text[j], text[j + 1], text_12[j / 3 + n0]))
        {
            SA[k] = i;
            t++;
            if (t == n02)
            {
                for (k++; p < n0; p++, k++)
                {
                    SA[k] = SA_0[p];
                }
            }
        }
        else
        {
            SA[k] = j;
            p++;
            if (p == n0)
            {
                for (k++; t < n02; t++, k++)
                {
                    if (SA_12[t] < n0)
                    {
                        SA[k] = SA_12[t] * 3 + 1;
                    }
                    else
                    {
                        SA[k] = (SA_12[t] - n0) * 3 + 2;
                    }
                }
            }
        }
    }
    delete[] text_0;
    delete[] SA_0;
    delete[] text_12;
    delete[] SA_12;
}

void make_lcp(int *sa, int *lcp, const vector<int> &text, int n)
{
    int *rank = new int[n];
    for (int i = 0; i < n; i++)
    {
        rank[sa[i]] = i;
    }

    int k = 0;
    int j;
    //k-- to remove last character
    for (int i = 0; i < n; i++, k ? k-- : 0)
    {
        if (rank[i] == n - 1)
        {
            k = 0;
            continue;
        }

        j = sa[rank[i] + 1];

        while (i + k < n && j + k < n && text[i + k] == text[j + k])
        {
            k++;
        }
        lcp[rank[i]] = k;
    }
    delete[] rank;
}

void make_lcplr(int *lcp, int *lcplr, int i, int low, int high)
{

    if (low + 1 == high)
    {
        lcplr[i] = lcp[low];
        return;
    }

    int mid = low + (high - low) / 2;
    make_lcplr(lcp, lcplr, 2 * i + 1, low, mid);
    make_lcplr(lcp, lcplr, 2 * i + 2, mid, high);
    lcplr[i] = min(lcplr[2 * i + 1], lcplr[2 * i + 2]);
}

int find_rightposition(const string &ipstring, const string &query, int *SA, int *lcplr)
{

    int n = ipstring.size();
    int text_length = n;
    int sa_length = n;

    int l = 0;
    while (ipstring[SA[0] + l] == query[l])
    {
        l++;
        if (l == query.length() || SA[0] + l == sa_length)
            break;
    }

    int r = 0;
    while (ipstring[SA[text_length - 1] + r] == query[r])
    {
        r++;
        if (r == query.length() || SA[text_length - 1] + r == sa_length)
            break;
    }

    if (r == query.length())
    {
        return text_length - 1;
    }

    if (l < query.length() && ipstring[SA[0] + l] > query[l])
    {
        return -1;
    }
    if (r < query.length() && ipstring[SA[text_length - 1] + r] < query[r])
    {
        return -1;
    }

    int L = 0;
    int R = text_length - 1;
    int M = 0;
    int m = 0;

    int index = 0;
    while (R - L > 1)
    {
        M = (R + L) / 2;
        if (l >= r)
        {
            if (lcplr[2 * index + 1] >= l)
            {
                int lcp = 0;
                if (!(l + lcp == query.length() || SA[M] + l + lcp == sa_length))
                {
                    while (ipstring[SA[M] + l + lcp] == query[l + lcp])
                    {
                        lcp++;

                        if (l + lcp == query.length() || SA[M] + l + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                m = l + lcp;
            }
            else
            {
                m = lcplr[2 * index + 1];
            }
        }

        else
        {
            if (lcplr[2 * index + 2] >= r)
            {
                int lcp = 0;
                if (!(r + lcp == query.length() || SA[M] + r + lcp == sa_length))
                {
                    while (ipstring[SA[M] + r + lcp] == query[r + lcp])
                    {
                        lcp++;
                        if (r + lcp == query.length() || SA[M] + r + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                m = r + lcp;
            }
            else
            {
                m = lcplr[2 * index + 2];
            }
        }
        if (m < query.length() && query[m] < ipstring[SA[M] + m])
        {
            R = M;
            r = m;
            index = index * 2 + 1;
        }
        else
        {
            L = M;
            l = m;
            index = index * 2 + 2;
        }
    }
    // cout << "\nRight is \n";
    // cout << L << " " << R << "\n";
    return L;
}

int find_leftposition(const string &ipstring, const string &query, int *SA, int *lcplr)
{

    int n = ipstring.size();
    int text_length = n;
    int sa_length = n;

    int l = 0;
    while (ipstring[SA[0] + l] == query[l])
    {
        l++;
        if (l == query.length() || SA[0] + l == sa_length)
            break;
    }

    int r = 0;
    while (ipstring[SA[text_length - 1] + r] == query[r])
    {
        r++;
        if (r == query.length() || SA[text_length - 1] + r == sa_length)
            break;
    }

    if (l == query.length())
    {
        return 0;
    }

    if (l < query.length() && ipstring[SA[0] + l] > query[l])
    {
        return -1;
    }
    if (r < query.length() && ipstring[SA[text_length - 1] + r] < query[r])
    {
        return -1;
    }

    int L = 0;
    int R = text_length - 1;
    int M = 0;
    int m = 0;

    int index = 0;
    while (R - L > 1)
    {
        M = (R + L) / 2;
        if (l >= r)
        {
            if (lcplr[2 * index + 1] >= l)
            {
                int lcp = 0;
                if (!(l + lcp == query.length() || SA[M] + l + lcp == sa_length))
                {
                    while (ipstring[SA[M] + l + lcp] == query[l + lcp])
                    {
                        // cout<<"Values are "<<SA[M]+l+lcp<<" "<<l+lcp<<"\n";
                        lcp++;

                        if (l + lcp == query.length() || SA[M] + l + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                // cout<<"Inside this\n";
                // cout<<"Value of l and lcp are "<<l<<" "<<lcp<<"\n";
                m = l + lcp;
            }
            else
            {
                m = lcplr[2 * index + 1];
            }
        }

        else
        {
            if (lcplr[2 * index + 2] >= r)
            {
                int lcp = 0;
                if (!(r + lcp == query.length() || SA[M] + r + lcp == sa_length))
                {
                    while (ipstring[SA[M] + r + lcp] == query[r + lcp])
                    {
                        lcp++;
                        if (r + lcp == query.length() || SA[M] + r + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                m = r + lcp;
            }
            else
            {
                m = lcplr[2 * index + 2];
            }
        }
        if (m == query.length() || query[m] <= ipstring[SA[M] + m])
        {
            R = M;
            r = m;
            index = 2 * index + 1;
        }
        else
        {
            L = M;
            l = m;
            index = 2 * index + 2;
        }
    }
    // cout << "Left is \n";
    // cout << L << " " << R << "\n";
    return R;
}

int find_rightposition(const string &ipstring, const string &query, int *SA, int *lcplr, int &max_m)
{

    int n = ipstring.size();
    int text_length = n;
    int sa_length = n;

    int l = 0;
    while (ipstring[SA[0] + l] == query[l])
    {
        l++;
        if (l == query.length() || SA[0] + l == sa_length)
            break;
    }

    int r = 0;
    while (ipstring[SA[text_length - 1] + r] == query[r])
    {
        r++;
        if (r == query.length() || SA[text_length - 1] + r == sa_length)
            break;
    }

    if (r == query.length())
    {
        max_m = query.length();
        return text_length - 1;
    }

    if (l < query.length() && ipstring[SA[0] + l] > query[l])
    {
        return -1;
    }
    if (r < query.length() && ipstring[SA[text_length - 1] + r] < query[r])
    {
        return -1;
    }

    int L = 0;
    int R = text_length - 1;
    int M = 0;
    int m = 0;

    int index = 0;

    // cout<<"Right\n";
    while (R - L > 1)
    {
        M = (R + L) / 2;
        if (l >= r)
        {
            if (lcplr[2 * index + 1] >= l)
            {
                int lcp = 0;
                if (!(l + lcp == query.length() || SA[M] + l + lcp == sa_length))
                {
                    while (ipstring[SA[M] + l + lcp] == query[l + lcp])
                    {
                        lcp++;

                        if (l + lcp == query.length() || SA[M] + l + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                m = l + lcp;
            }
            else
            {
                m = lcplr[2 * index + 1];
            }
        }

        else
        {
            if (lcplr[2 * index + 2] >= r)
            {
                int lcp = 0;
                if (!(r + lcp == query.length() || SA[M] + r + lcp == sa_length))
                {
                    while (ipstring[SA[M] + r + lcp] == query[r + lcp])
                    {
                        lcp++;
                        if (r + lcp == query.length() || SA[M] + r + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                m = r + lcp;
            }
            else
            {
                m = lcplr[2 * index + 2];
            }
        }

        // cout<<"Left\n";
        // for(int i2=SA[L];i2<=SA[L]+5;i2++){
        //     cout<<(int)ipstring[i2]<<" ";
        // }
        // cout<<"\n";

        // cout<<"Middle\n";
        // for(int i2=SA[M];i2<=SA[M]+5;i2++){
        //     cout<<(int)ipstring[i2]<<" ";
        // }
        // cout<<"\n";

        // cout<<"Right\n";
        // for(int i2=SA[R];i2<=SA[R]+5;i2++){
        //     cout<<(int)ipstring[i2]<<" ";
        // }
        // cout<<"\n\n";
        if (m < query.length() && query[m] < ipstring[SA[M] + m])
        {
            R = M;
            r = m;
            index = index * 2 + 1;
        }
        else
        {
            L = M;
            l = m;
            index = index * 2 + 2;
        }
        // cout<<"Value of m is "<<m<<"\n";
        if (m > max_m)
        {
            max_m = m;
        }
    }
    // cout << "\nRight is \n";
    // cout << L << " " << R << "\n";
    return L;
}

int find_leftposition(const string &ipstring, const string &query, int *SA, int *lcplr, int &max_m)
{

    int n = ipstring.size();
    int text_length = n;
    int sa_length = n;

    int l = 0;
    while (ipstring[SA[0] + l] == query[l])
    {
        l++;
        if (l == query.length() || SA[0] + l == sa_length)
            break;
    }

    int r = 0;
    while (ipstring[SA[text_length - 1] + r] == query[r])
    {
        r++;
        if (r == query.length() || SA[text_length - 1] + r == sa_length)
            break;
    }

    if (l == query.length())
    {
        max_m = query.length();
        return 0;
    }

    if (l < query.length() && ipstring[SA[0] + l] > query[l])
    {
        return -1;
    }
    if (r < query.length() && ipstring[SA[text_length - 1] + r] < query[r])
    {
        return -1;
    }

    int L = 0;
    int R = text_length - 1;
    int M = 0;
    int m = 0;

    int index = 0;

    // cout<<"Left\n";
    while (R - L > 1)
    {
        M = (R + L) / 2;
        if (l >= r)
        {
            if (lcplr[2 * index + 1] >= l)
            {
                int lcp = 0;
                if (!(l + lcp == query.length() || SA[M] + l + lcp == sa_length))
                {
                    while (ipstring[SA[M] + l + lcp] == query[l + lcp])
                    {
                        // cout<<"Values are "<<SA[M]+l+lcp<<" "<<l+lcp<<"\n";
                        lcp++;

                        if (l + lcp == query.length() || SA[M] + l + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                // cout<<"Inside this\n";
                // cout<<"Value of l and lcp are "<<l<<" "<<lcp<<"\n";
                m = l + lcp;
            }
            else
            {
                m = lcplr[2 * index + 1];
            }
        }

        else
        {
            if (lcplr[2 * index + 2] >= r)
            {
                int lcp = 0;
                if (!(r + lcp == query.length() || SA[M] + r + lcp == sa_length))
                {
                    while (ipstring[SA[M] + r + lcp] == query[r + lcp])
                    {
                        lcp++;
                        if (r + lcp == query.length() || SA[M] + r + lcp == sa_length)
                        {
                            break;
                        }
                    }
                }
                m = r + lcp;
            }
            else
            {
                m = lcplr[2 * index + 2];
            }
        }
        if (m == query.length() || query[m] <= ipstring[SA[M] + m])
        {
            R = M;
            r = m;
            index = 2 * index + 1;
        }
        else
        {
            L = M;
            l = m;
            index = 2 * index + 2;
        }
        // cout<<"Value of m is "<<m<<"\n";
        if (m > max_m)
        {
            max_m = m;
        }
    }
    // cout << "Left is \n";
    // cout << L << " " << R << "\n";
    return R;
}

vector<pair<int, int>> inverse_map(string ipstring)
{

    vector<pair<int, int>> vec(ipstring.length());
    int k = 0;
    int pos = 0;
    for (int i = 0; i < ipstring.length(); i++)
    {
        if (ipstring[i] == 1)
        {

            vec[i] = make_pair(k, pos);
            k++;
            pos = 0;
            continue;
        }
        vec[i] = make_pair(k, pos);
        pos++;
    }
    return vec;
}

string normalize(const string &query, const unordered_set<string> &swords)
{
    string temp1;
    string temp4;
    string temp3;
    stringstream ss;

    std::remove_copy_if(query.begin(), query.end(),
                        std::back_inserter(temp1), //Store output
                        std::ptr_fun<int, int>(&std::ispunct));

    transform(temp1.begin(), temp1.end(), temp1.begin(), ::tolower);

    ss << temp1;
    while (!ss.eof())
    {
        ss >> temp4;
        if (swords.find(temp4) == swords.end())
        {
            temp3 += temp4;
            temp3 += ' ';
        }
    }
    if (temp3.length() > 0)
    {
        temp3.pop_back();
    }
    return temp3;
}

void q1query(const string &query, const unordered_set<string> &stopwords, const string &ipstring, int *SA, int *lcplr, const vector<pair<int, int>> &imap, const int &no_of_rows, string &longest_sub)
{

    string query1 = normalize(query, stopwords);
    string query2 = query1;

    cout<<"Query is \n"<<query1<<"\n";
    // cout<<"Query is "<<query<<"\n";
    // cout<<"Query1 is "<<query1<<"\n";
    // cout<<"Query2 is "<<query2<<"\n";
    int q1length = query1.length();
    vector<int> max_lv;
    vector<int> pos;
    // string longest_sub;
    vector<pair<int, int>> doc_i;
    int max_m = 0;
    int max_m1 = 0;
    int max_m2 = 0;
    int max_mt = 0;
    int foc = 0;
    unsigned long long int t0, t1;
    // t0 = rdtsc(); //start timer

    int r = 0;
    int l = 0;
    for (int i1 = 0; i1 < q1length; i1++)
    {
        max_m = 0;
        max_m1 = 0;
        max_m2 = 0;

        r = find_rightposition(ipstring, query1, SA, lcplr, max_m1);
        l = find_leftposition(ipstring, query1, SA, lcplr, max_m2);

        max_m = max_m1 > max_m2 ? max_m1 : max_m2;
        if (max_m > max_mt)
        {
            max_mt = max_m;
        }
        // cout << "Max m1 and m2 are " << max_m1 << " " << max_m2 << "\n";

        max_lv.push_back(max_m);

        if (max_m1 == query1.length() || max_m2 == query1.length())
        {
            break;
        }
        query1.erase(0, 1);
    }
    if (max_m == q1length)
    {
        r = find_rightposition(ipstring, query1, SA, lcplr);
        l = find_leftposition(ipstring, query1, SA, lcplr);

        pos.resize(r - l + 1);
        set<int> unique_ind;
        vector<int> min_v(no_of_rows, -1);

        for (int i1 = l, k = 0, ind = 0, doc = 0, off = 0; i1 <= r; i1++)
        {
            ind = SA[i1];
            doc = imap[ind].first;
            off = imap[ind].second;
            if (min_v[doc] == -1)
            {
                min_v[doc] = off;
            }
            else if (off < min_v[doc])
            {
                min_v[doc] = off;
            }
            unique_ind.insert(doc);
            // cout<<"Offset is "<<ind<<"\n";
        }
        // cout<<"Indices are \n";
        doc_i.push_back(make_pair(-1, -1));
        for (auto i1 : unique_ind)
        {
            doc_i.push_back(make_pair(i1, min_v[i1]));
            // cout<<i1<<" "<<min_v[i1]<<"\n";
        }
        // cout<<"Outside\n";
    }
    else if (max_m == 0)
    {
        doc_i.push_back(make_pair(-3, -3));
    }
    else
    {
        doc_i.push_back(make_pair(-2, -2));
        doc_i.push_back(make_pair(INT_MAX, INT_MAX));
        for (int i1 = 0; i1 < max_lv.size(); i1++)
        {
            // cout<<"Max lv is "<<max_lv[i1]<<"\n";
            if (max_lv[i1] == max_mt)
            {
                // cout<<"Query 2 is "<<query2<<"\n";
                // cout<<"i1 is "<<i1<<"\n";
                query1 = query2.substr(i1, max_mt);
                // cout<<"Query 1 is "<<query1<<" "<<query1.length()<<"\n";
                r = find_rightposition(ipstring, query1, SA, lcplr);
                l = find_leftposition(ipstring, query1, SA, lcplr);
                // cout<<"l and r are "<<l<<" "<<r<<"\n";
                for (int i2 = l; i2 <= r; i2++)
                {
                    if (SA[i2] < doc_i[1].second)
                    {
                        doc_i[1].first = imap[SA[i2]].first;
                        doc_i[1].second = imap[SA[i2]].second;
                        longest_sub = query1;
                    }
                }
            }
        }
    }

    if (doc_i[0].first == -1 && doc_i[0].second == -1)
    {
        cout << "Whole string found\n";
        for (int i2 = 1; i2 < doc_i.size(); i2++)
        {
            cout << doc_i[i2].first << " " << doc_i[i2].second << "\n";
        }
    }
    else if (doc_i[0].first == -2 && doc_i[0].second == -2)
    {
        cout << "First location of substring:" << longest_sub << "\n";
        cout << doc_i[1].first << " " << doc_i[1].second << "\n";
    }
    else if (doc_i[0].first == -3 && doc_i[0].second == -3)
    {
        cout << "No substring found\n";
    }
}

vector<pair<int, double>> title_rank(const vector<string> &query, const vector<string> &title)
{

    unordered_set<string> query_set;

    for (auto i : query)
    {
        query_set.insert(i);
    }

    double count1 = query.size();
    vector<pair<int, double>> scores(title.size());

    string temp1;
    for (int i = 0; i < title.size(); i++)
    {
        int count = 0;
        scores[i].first = i;
        stringstream ss;
        ss << title[i];
        while (!ss.eof())
        {
            ss >> temp1;
            if (query_set.find(temp1) != query_set.end())
            {
                scores[i].second += ((1 / count1) * 15);
                count += 1;
            }
        }
        scores[i].second += (count * 20);
    }
    return scores;
}
void doc_rank(const string &query, const unordered_set<string> &stopwords, const string &ipstring, int *SA, int *lcplr, const vector<pair<int, int>> &imap, const int &no_of_rows, const vector<string> &title, const vector<pair<string, string>> &doc_string)
{
    // ofstream debug1("debug1.txt");
    // cout<<"Query is "<<query<<"p\n";
    string query1 = normalize(query, stopwords);

    string temp1;
    vector<string> vec_query;
    stringstream ss;
    ss << query1;
    while (!ss.eof())
    {
        ss >> temp1;
        vec_query.push_back(temp1);
    }
    vector<int> vec_query_count(vec_query.size());
    vector<pair<int, double>> scores = title_rank(vec_query, title);
    vector<pair<int, pair<int, double>>> max_weight(title.size());
    for (int i = 0; i < scores.size(); i++)
    {
        if (scores[i].second != 0)
        {
            max_weight[i].second.first = 1;
            max_weight[i].second.second = scores[i].second;
        }
    }
    if (vec_query.size() > 1)
    {
        int r = find_rightposition(ipstring, query1, SA, lcplr);
        int l = find_leftposition(ipstring, query1, SA, lcplr);

        for (int i = l, doc, off, qlength = query1.length(); i <= r; i++)
        {
            // cout << "SA is " << SA[i] << "\n";
            if ((SA[i] == 0) ||
                (((ipstring[SA[i] - 1] == ' ') || (ipstring[SA[i] - 1] == 1)) &&
                 ((ipstring[SA[i] + qlength] == ' ') || (ipstring[SA[i] + qlength]) == 1)))
            {
                doc = imap[SA[i]].first;
                // off = imap[SA[i]].second;
                // doc_word_index[doc][i1].push_back(off);
                // cout<<"QUERY 1 SIZE IS "<<vec_query.size()<<"\n";
                scores[doc].second += (10 * vec_query.size());
                if ((10 * vec_query.size()) > max_weight[doc].second.second)
                {
                    max_weight[doc].second.first = 2;
                    max_weight[doc].second.second = (10 * vec_query.size());
                }
            }
        }
    }
    // cout << "Scores are\n";
    // for (auto i : scores)
    // {
    //     cout << i.second << "\n";
    // }

    vector<vector<vector<int>>> doc_word_index(title.size(), vector<vector<int>>(vec_query.size()));

    for (int i1 = 0; i1 < vec_query.size(); i1++)
    {
        vector<int> pos;
        int l, r;

        string query1 = vec_query[i1];
        //debug1 << "Query 1 is " << query << "\n";
        int qlength = query1.length();
        r = find_rightposition(ipstring, query1, SA, lcplr);
        l = find_leftposition(ipstring, query1, SA, lcplr);

        // cout << "l and r are " << l << " " << r << "\n";
        for (int i = l, doc, off; i <= r; i++)
        {
            // cout << "SA is " << SA[i] << "\n";
            if ((SA[i] == 0) ||
                (((ipstring[SA[i] - 1] == ' ') || (ipstring[SA[i] - 1] == 1)) &&
                 ((ipstring[SA[i] + qlength] == ' ') || (ipstring[SA[i] + qlength]) == 1)))
            {
                doc = imap[SA[i]].first;
                off = imap[SA[i]].second;
                doc_word_index[doc][i1].push_back(off);
                pos.push_back(SA[i]);
            }
        }

        vec_query_count[i1] = pos.size();
        //debug1 << "Positions are \n";
        for (int i = 0; i < pos.size(); i++)
        {
            //debug1 << pos[i] << " ";
        }
        //debug1 << "\n";
    }

    for (int i = 0; i < doc_word_index.size(); i++)
    {

        vector<double> avg_values;
        for (int j = 0; j < doc_word_index[i].size(); j++)
        {
            if (doc_word_index[i][j].size() > 0)
            {

                // printf("Before is %lf\n",scores[i].second);
                // cout << "Before is " << scores[i].second << "\n";
                // cout<<"Size is "<<doc_word_index[i][j].size()<<"\n";
                // cout<<"Count is "<<vec_query_count[j]<<"\n";

                //weight of each word
                scores[i].second += (((double)doc_word_index[i][j].size() / (double)vec_query_count[j]) * 30);

                if ((((double)doc_word_index[i][j].size() / (double)vec_query_count[j]) * 30) > max_weight[i].second.second)
                {
                    max_weight[i].second.first = 3;
                    max_weight[i].second.second = (((double)doc_word_index[i][j].size() / (double)vec_query_count[j]) * 30);
                }
                // cout << "After is " << scores[i].second << "\n";

                // printf("After is %lf\n",scores[i].second);
                // cout<<"Accumulate is "<<(double)accumulate(doc_word_index[i][j].begin(),
                // doc_word_index[i][j].end(), 0.0)<<"\n";
                // cout<<"Size is "<<(double)doc_word_index[i][j].size()<<"\n";
                avg_values.push_back(((double)accumulate(doc_word_index[i][j].begin(),
                                                         doc_word_index[i][j].end(), 0.0) /
                                      (double)doc_word_index[i][j].size()));

                // cout<<"Rubbish is "<<avg_values[avg_values.size()-1]<<"\n";
            }
        }

        //number of occuring words.

        // //debug1<<"Nonese "<<(double)doc_word_index[i].size()<<"\n";
        // //debug1<<"Avg values size is"<<(double)avg_values.size()<<" "<<(double)doc_word_index[i].size()<<"\n";
        scores[i].second += (((double)avg_values.size() / (double)doc_word_index[i].size()) * 3);

        if ((((double)avg_values.size() / (double)doc_word_index[i].size()) * 3) > max_weight[i].second.second)
        {
            max_weight[i].second.first = 4;
            max_weight[i].second.second = (((double)avg_values.size() / (double)doc_word_index[i].size()) * 3);
        }

        // cout << "After1 is " << scores[i].second << "\n";

        if (avg_values.size() > 1)
        {
            double sum = 0;
            for (int j = 1; j < avg_values.size(); j++)
            {
                // cout<<"Values are "<<avg_values[i]<<" "<<avg_values[i-1]<<"\n";
                sum += abs(avg_values[j] - avg_values[j - 1]);
                // cout<<"Sum1 is "<<sum<<"\n";
            }
            // cout<<"Sum is "<<sum<<"\n";
            // //debug1 << "SECOND IS " << doc_string[i].second << "\n";
            scores[i].second += ((doc_string[i].second.length() / (sum * 20)));

            if (((doc_string[i].second.length() / (sum * 20))) > max_weight[i].second.second)
            {
                max_weight[i].second.first = 5;
                max_weight[i].second.second = ((doc_string[i].second.length() / (sum * 20)));
            }

            // cout << "After2 is " << scores[i].second << "\n";
        }
    }

    for (int i = 0; i < doc_word_index.size(); i++)
    {
        for (int j = 0; j < doc_word_index[i].size(); j++)
        {
            if (doc_word_index[i][j].size() > 0)
            {
                //debug1 << "I and J are " << i << " " << j << "\n";
            }
            for (int k = 0; k < doc_word_index[i][j].size(); k++)
            {
                //debug1 << doc_word_index[i][j][k] << " ";
            }
            if (doc_word_index[i][j].size() > 0)
            {
                //debug1 << "\n";
            }
        }
    }

    sort(scores.begin(), scores.end(), sort2);
    if (scores[0].second == 0)
    {
        cout << "No relevant document\n";
    }
    else
    {

        cout << "Scores followed by paramter with most score are \n";
        for (int i = 0; i < scores.size(); i++)
        {
            if (scores[i].second == 0)
            {
                break;
            }
            cout << doc_string[scores[i].first].first << " " << scores[i].second << "\n";
            cout <<max_weight[scores[i].first].second.first <<" "<< max_weight[scores[i].first].second.second<<"\n";
        }
    }
    // for(int i)
}

unsigned long long rdtsc()
{
    unsigned long long int x;
    unsigned a, d;

    __asm__ volatile("rdtsc"
                     : "=a"(a), "=d"(d));

    return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}
