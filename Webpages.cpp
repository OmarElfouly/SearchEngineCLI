#include "Webpages.h"

Webpages::Webpages()
{
	//open files
	//open Web Graph File
	fstream fin;
	fin.open("WebGraphFile.csv", ios::in);
	string temp,word;
	string delimiter = ",";

	while (fin >> temp) {
		string w1 = temp.substr(0, temp.find(delimiter));
		temp.erase(0, temp.find(delimiter) + 1);
		string w2 = temp.substr(0, temp.find(delimiter));
		hyperlinks[w1].push_back(w2);
		reverseHyperlinks[w2].push_back(w1);
	}

	//this completes hyperlinks
	//next we will open keyword file
	fin.close();

	fin.open("KeyWordsFile.csv", ios::in);
	while (fin >> temp) {

		string w = temp.substr(0, temp.find(delimiter));
		temp.erase(0, temp.find(delimiter) + 1);
		urls.push_back(w);//adding urls. ASSUME ALL URLS NEED KEYWORDS
		//we have the website now we must pushback its keywords
		size_t p = 0;
		string kword;
		while ((p = temp.find(delimiter)) != string::npos) {
			kword = temp.substr(0, p);
			kwords.insert(kword);
			keywords[w].push_back(kword);
			temp.erase(0, p + 1);
		}
		keywords[w].push_back(temp);
		kwords.insert(temp);
	}
	fin.close();
	//this completes keywords file
	//Finally Number of impressions
	fin.open("NumberOfImpressionsFile.csv", ios::in);
	while (fin >> temp) {
		string w = temp.substr(0, temp.find(delimiter));
		temp.erase(0, temp.find(delimiter) + 1);
		string number = temp.substr(0, temp.find(delimiter));
		impressions[w]=stoi(number);
	}
	fin.close();


	fin.open("NumberOfClicks.csv", ios::in);
	while (fin >> temp) {
		string w = temp.substr(0, temp.find(delimiter));
		temp.erase(0, temp.find(delimiter) + 1);
		string number = temp.substr(0, temp.find(delimiter));
		numClicks[w] = stoi(number);
	}
	fin.close();


	//now we intialise clicks to zero
	/*for (map<string, vector<string>>::iterator itr = keywords.begin(); itr != keywords.end(); itr++) {
		numClicks[itr->first] = 0;
	}*/
	//loop to ensure that even urls with no links are in map
	for (int i = 0; i < urls.size(); i++) {
		if (hyperlinks.find(urls[i]) == hyperlinks.end()) {
			hyperlinks[urls[i]] = vector<string>(0);
		}
		if (reverseHyperlinks.find(urls[i]) == reverseHyperlinks.end()) {
			reverseHyperlinks[urls[i]] = vector<string>(0);
		}
	}
	//tested up till here
	caculatePageRanks();

}

void Webpages::calculateCtrs()
{
	for (map<string,int>::iterator itr = numClicks.begin(); itr != numClicks.end(); itr++) {
		ctrs[itr->first] = (double)itr->second/(double)impressions[itr->first];
	}
}

bool Webpages::cmp(pair<string, double>& a, pair<string, double>& b)
{
	return (a.second < b.second);
}

void Webpages::caculatePageRanks()//untested
{
	map<string, double> prev, cur;
	double d = 0.85;
	//find sinks and make them point at every other node
	vector<string> sinks;
	for (int i = 0; i < urls.size(); i++) {
		if (isSink(urls[i])) {
			//mark as sink
			sinks.push_back(urls[i]);
		}
	}
	//for every sink add hyperlink to every site and update reverse hyperlinks
	for (int i = 0; i < sinks.size(); i++) {
		hyperlinks[sinks[i]] = urls;
		for (int j = 0; j < urls.size(); j++) {
			reverseHyperlinks[urls[j]].push_back(sinks[i]);
		}
	}
	//intialise ranks as 1/n
	int n = (int)urls.size();
	for (auto x:urls) {
		prev[x] = 1.0 / (double)n;
	}
	//repeated iterations. Stop at 100 or when change for each rank is <=0.01
	int count = 0;
	double diference = 99999;
	while (count < 100 &&diference>0.0000001) {
		double maxdif = 0;
		for (auto i:urls) {
			cur[i] = (double)(1.0 - d) / (double) n;
			double sum = 0;
			vector<string> pagesLinkToI = reverseHyperlinks[i];
			for (auto j:pagesLinkToI) {
				sum += prev[j] / (double) hyperlinks[j].size();
			}
			sum *= d;
			cur[i] += sum;
			double temp = abs(cur[i] - prev[i]);
			if (temp > maxdif) {
				maxdif = temp;
			}
		}
		count++;
		diference = maxdif;
		prev = cur;
	}
	//now normalise by ordering cur then give largest n, then n-1 e.t.c
	//we must sort map cur
	vector < pair<double, string>> vec;
	for (auto& x : cur) {
		vec.push_back(make_pair(x.second,x.first));
	}

	sort(vec.rbegin(),vec.rend());
	int rank = vec.size();
	for (int i = 0; i < vec.size(); i++) {
		pageRanks[vec[i].second] = rank;
		rank--;
	}	
}

bool Webpages::isSink(string url)
{
	if (hyperlinks[url].size() == 0) {
		return true;
	}
	else {
		return false;
	}
}

