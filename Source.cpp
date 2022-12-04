#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "Webpages.h"

using namespace std;

vector<string> singleSearch(string w, Webpages& webs);

void updateCTR(Webpages& webs);

vector<string> searchFunc(vector<string> words, Webpages& webpages);

bool compare(pair<string, double>& a, pair<string, double>& b);

vector<string> sortPages(vector<string> unorderedUrls, Webpages& webs);

void searchBar(Webpages& webs);

void menu2(vector<string> listOfPages, Webpages& webs);

void menu(Webpages& webs);

int hammingDist(string one, string two);

int main()
{
    cout << "Welcome!\n";
    Webpages webs;
    menu(webs);
    updateCTR(webs);

}
vector<string> singleSearch(string w, Webpages& webs) {
    //search for w to produce a vector of strings of the websites that contain w
    vector<string> results;

    for (map<string, vector<string>>::iterator itr = webs.keywords.begin(); itr != webs.keywords.end(); itr++) {
        for (int i = 0; i < itr->second.size(); i++) {
            if (w == itr->second[i]) {
                results.push_back(itr->first);
            }
        }//same as using vector.find() as both are O(n)
    }
    if (results.size() == 0) {
        //try to suggest correction
        // Calculate hemming for everykeyword
        set<string >::iterator itr1;
        map<int, string> replace;
        int smallest = 9999;
        for (itr1 = webs.kwords.begin(); itr1 != webs.kwords.end(); itr1++) {
            int hm = hammingDist(w, *itr1);
            replace[hm] = *itr1;
            if (hm < smallest) {
                smallest = hm;
                //replace.push_back(*itr1);
                replace[hm] = *itr1;
            }
            else if (hm == smallest) {
                //replace.push_back(*itr1);
                replace[hm] = *itr1;
            }
        }
        auto itr = replace.begin();
        for (; itr != replace.end();) {
            if (itr->first != smallest) {
                itr=replace.erase(itr);
            }
            else {
                ++itr;
            }
        }
        //map has been filterd
        // store map in vector
        vector<string> replaceV;
        for (auto it = replace.begin(); it != replace.end(); ++it) {
            replaceV.push_back(it->second);
        }

        cout << "\n\nNo match for keyword " << w << " was found. The nearest matches are:\n";
        for (int i = 0; i < replaceV.size(); i++) {
            cout << i + 1 << ". " << replaceV[i] << "\n";
        }
        cout << "\n";
        cout << "\nWould you like to:\n1. Replace " << w << " with a suggestion\n2. Continue with " << w << "\nEnter your choice: ";
        int choice;
        cin >> choice;
        while (choice < 1 || choice>2) {
            if (cin.fail()) {
                cin.clear();
                cin.ignore();
            }
            cout << "\nInvalid choice!\nPlease input a number between 1 and 3: ";
            cin >> choice;
        }
        if (choice == 1) {
            cout << "\nInput the number for the suggestion you which to use:";
            int c;
            cin >> c;
            while (c < 1 || c>replaceV.size()) {
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore();
                }
                cout << "\nInvalid choice!\nPlease input a number between 1 and "<<replaceV.size()<<": ";
                cin >> c;
            }
            cout << "\n";
            results = singleSearch(replaceV[c-1], webs);
        }
        else if (choice == 2) {
            cout << "\nYou chose to continue.\n\n";
        }
    }

    return results;
}
void updateCTR(Webpages& webs)
{
    // Open file  NumberOfImpressionsFile.csv
    // update file with new impressions
    // url, impressions
    // close file
    fstream file;
    file.open("NumberOfImpressionsFileNEW.csv",ios::out);
    for (auto x : webs.impressions) {
        file << x.first << "," << x.second<<"\n";
    }
    file.close();
    remove("NumberOfImpressionsFile.csv");
    rename("NumberOfImpressionsFileNEW.csv", "NumberOfImpressionsFile.csv");
    
    //Open file NumberOfClicks.csv
    //url, clicks
    //close file
    file.open("NumberOfClicksNEW.csv", ios::out);
    for (auto x : webs.numClicks) {
        file << x.first << "," << x.second << "\n";
    }
    file.close();
    remove("NumberOfClicks.csv");
    rename("NumberOfClicksNEW.csv", "NumberOfClicks.csv");
}
vector<string> searchFunc(vector<string> words, Webpages& webpages) {
    map<int, vector<string> > searchResults;
    map<int, string > operators;
    for (int i = 0; i < words.size(); i++) {
        if (words[i] == "AND" || words[i] == "OR") {
            operators[i] = words[i];
        }
        else {
            searchResults[i] = singleSearch(words[i], webpages);
        }
    }
    //now we have a map of the search results and a map of the operators between them
    //tracing and fixing the code was a nightmare
    map<int, string >::iterator itr = operators.begin();
    while(operators.size()>0&&itr!=operators.end()){
        if (itr->second == "AND") {
            vector<string> result;
            int left = itr->first - 1;
            while (searchResults.find(left) == searchResults.end()) {
                left--;
            }
            int right = itr->first + 1;
            while (searchResults.find(right) == searchResults.end()) {
                right++;
            }
            /*for (int j = 0; j < searchResults[left].size(); j++) {
                for (int k = 0; k < searchResults[right].size(); k++) {
                    if (searchResults[left][j] == searchResults[right][k]) {
                        result.push_back(searchResults[left][j]);
                    }
                }
            }old and ineffeceint*/

            sort(searchResults[left].begin(), searchResults[left].end());
            sort(searchResults[right].begin(), searchResults[right].end());
            auto it = set_intersection(searchResults[left].begin(), searchResults[left].end(), searchResults[right].begin(), searchResults[right].end(), back_inserter(result));
            searchResults[left] = result;
            searchResults.erase(right);
            itr=operators.erase(itr);
            //itr--;
            /*if (itr != operators.begin() && operators.size() > 0) {
                cout << "test";
                itr--;
            }breaks code as itr has been deleted*/
            // a possible fix would be to store itr-- before del
            //itr = operators.begin();//necessary evil
        }
        else {
            itr++;
        }
    }
    //now that there are no ands we can combine whats left using the or statments
    while (searchResults.size() > 1) {
        vector<string> result = searchResults.begin()->second;
        for (int i = 0; i < next(searchResults.begin())->second.size(); i++) {
            if (find(result.begin(), result.end(), next(searchResults.begin())->second[i]) == result.end()) {
                result.push_back(next(searchResults.begin())->second[i]);
            }
        }
        searchResults.begin()->second = result;
        searchResults.erase(next(searchResults.begin()));

    }
    

    auto it = searchResults.begin();

    return it->second;
}//tested

bool compare(pair<string, double>& a, pair<string, double>& b) {
    return a.second > b.second;
}

vector<string> sortPages(vector<string> unorderedUrls, Webpages& webs) {
    vector<string> result;
    //page rank is preformed by constructor to produce normailised page rank
    //this means all that is left is to caclulate ctr, page rank was done in constructor, then score
    // ctr = number of clicks/number of show up *100
    webs.calculateCtrs();
    //score = 0.4(Page rank)+((1-(0.1(impressions))/(1+0.1(impressions)))(page rank)+(0.1(impressions))/(1+0.1(impressions))(ctr))
    map<string, double> scores;
    for (auto x : unorderedUrls) {
        scores[x] = 0.4 * webs.pageRanks[x];
        scores[x] += ((1.0 - ((0.1 * webs.impressions[x]) / (1.0 + 0.1 * webs.impressions[x]))) * webs.pageRanks[x] + ((0.1 * webs.impressions[x]) / (1.0 + 0.1 * webs.impressions[x])) * webs.ctrs[x]) * 0.6;
    }
    //sort unordered urls by score
    //create vector of given urls and score
    vector<pair<string, double>> urlsAndScore;
    for (auto x : unorderedUrls) {
        pair<string, double> temp;
        temp.first = x;
        temp.second = scores[x];
        urlsAndScore.push_back(temp);
    }
    sort(urlsAndScore.begin(), urlsAndScore.end(), compare);
    for (int i = 0; i < urlsAndScore.size();i++) {
        result.push_back(urlsAndScore[i].first);
    }
    return result;
}

void searchBar(Webpages& webs) {
    cout << "Search bar:\n";
    string search;
    cin.ignore();
    getline(cin,search);

    vector<string> words;
    words.push_back("");
    int count = 0;
    for (auto x : search) {
        if (x == ' ') {
            words.push_back("");
            count++;
        }
        else {
            words[count] = words[count] + x;
        }
    }
    string qoute = "\"";
    for (int i = 0; i < words.size(); i++) {
        size_t found = words[i].find(qoute);
        if (found == 0) {
            words[i].erase(found, 1);

            if (words[i].find(qoute) != string::npos) {
                //we have "cat"
                words[i].erase(words[i].find(qoute), 1);//erase the final qoute
            }
            else {
                words.insert(words.begin() + i + 1, "AND");
            }
        }
        else if (found == words[i].size() - 1) {
            words[i].erase(found, 1);
        }
    }
    for (int i = 0; i+1 < words.size() ; i++) {
        if ((words[i] != "AND" && words[i] != "OR") && (words[i + 1] != "AND" && words[i + 1] != "OR")) {
            words.insert(words.begin() + i + 1, "OR");
        }
    }
    //We have a list of words with their relevant commands i.e. Omar AND Elfouly OR Ahmed ...
    //logically if we conver A OR B AND C OR D into A OR BC OR D we can just preform the ORs. This is true for all combinations
    // i.e. preforming all and commands leaves us with only OR which can be done in any order
    //we must pass the websites with their Keywords so first we will read the files


    vector<string> results = searchFunc(words, webs);
    if (results.size() == 0) {
        cout << "No websites match the search!\n\n";
        menu(webs);
    }
    else {
        // now we must rank the results
        vector<string> sortedWebpages = sortPages(results, webs);
        webs.updateImpressions(sortedWebpages);
        //now we must display the websites
        cout << "\nSearch Results:\n";
        for (int i = 0; i < sortedWebpages.size(); i++) {
            cout << i + 1 << ". " << sortedWebpages[i] << "\n";
        }
        //must call new menu2
        cout << "\n";
        menu2(sortedWebpages, webs);
    }

}

void menu2(vector<string> listOfPages, Webpages& webs) {
    cout << "\nWould you like to\n1. Choose a webpage to open\n2. New search\n3. Exit\n";
    int choice;
    cout << "\nType in your choice: ";
    cin >> choice;
    while (choice < 1 || choice>3) {
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
        }
        cout << "Invalid choice.\nPlease make sure to input either 1 or 2 or 3\n";
        cin >> choice;
    }
    if (choice == 1) {
        cout << "\nYou chose to open a website\n";
        cout << "Please type in number of website: ";
        int webChoice;
        cin >> webChoice;
        while (webChoice<1 || webChoice>listOfPages.size()) {
            if (cin.fail()) {
                cin.clear();
                cin.ignore();
            }
            cout << "Invalid choice!\nPlease input a number between 1 and " << listOfPages.size() << ": ";
            cin >> webChoice;
        }
        webs.updateNumClicks(listOfPages[webChoice - 1]);
        cout << "\nYou're now viewing " << listOfPages[webChoice - 1] << ".\nWould you like to\n1. Back to search results\n2. New search\n3. Exit\nType in your choice: ";
        int fchoice;
        cin >> fchoice;
        while (fchoice < 1 || fchoice>3) {
            cout << "Invalid choice!\nPlease input a number between 1 and 3: ";
            cin >> fchoice;
        }
        if (fchoice == 1) {
            cout << "\nSearch Results:\n";
            for (int i = 0; i < listOfPages.size(); i++) {
                cout << i + 1 << ". " << listOfPages[i] << "\n";
            }
            //must call menu2
            menu2(listOfPages, webs);
        }
        else if (choice == 2) {
            cout << "\nYou chose to preform a new search.\n";
            searchBar(webs);
        }
        else if (choice == 3) {
            cout << "\n\nGoodbye!\n";

        }
    }
    else if (choice == 2) {
        cout << "\nYou chose to preform a new search.\n";
        searchBar(webs);
    }
    else {
        cout << "\nYou chose to exit.\n";
    }

}

void menu(Webpages& webs) {
    int choice;
    cout << "\nWhat would you like to do?\n1. New search\n2. Exit\n";
    cout << "\nType in your choice (1 or 2): ";
    cin >> choice;
    while (choice < 1 || choice>2) {
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
        }
        
        cout << "Invalid choice.\nPlease make sure to input either 1 or 2\n";
        cin >> choice;
    }
    if (choice == 1) {
        cout << "\nYou chose to search for a new item.\n";
        searchBar(webs);
    }
    else if (choice == 2) {
        cout << "\nYou chose to exit.\n";
    }
}

int hammingDist(string one, string two)//modified for different lengths
{
    int i = 0;
    int count = 0;
    int dif = one.size() - two.size();
    if (dif <= 0) {
        for (int i = 0; i < one.size(); i++) {
            if (one[i] != two[i]) {
                count++;
            }
        }
        dif = abs(dif);
    }
    else {
        for (int i = 0; i < two.size(); i++) {
            if (one[i] != two[i]) {
                count++;
            }
        }
    }
    int hm = count + dif;
    
    return hm;
}
