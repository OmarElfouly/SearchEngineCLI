#pragma once
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>
using namespace std;

class Webpages
{
public:
	Webpages();
	vector<string> urls;
	set<string> kwords;//added later for autocorrect implmentation is a set of keywords
	map<string, vector<string>> keywords;// is a map from urls to vector of strings of keywords
	map<string, vector<string>> hyperlinks;//what the page points at
	map<string, vector<string>> reverseHyperlinks;//who points at the page
	map<string, int> impressions;
	map<string, int> numClicks;

	void calculateCtrs();
	map<string, double> ctrs;

	void caculatePageRanks();
	map<string, double> pageRanks;
	bool isSink(string url);
	bool cmp(pair<string, double>& a, pair<string, double>& b);

	void updateImpressions(vector<string> appearingUrls) {
		for (auto x : appearingUrls) {
			impressions[x]= impressions[x]+1;
		}
	}
	void updateNumClicks(string url) {
		numClicks[url]= numClicks[url]+1;
	}

};

