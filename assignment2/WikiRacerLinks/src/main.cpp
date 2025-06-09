#include <iostream>
#include <fstream>
#include <unordered_set>
#include <sstream>
#include <algorithm>

using std::cout;            using std::endl;
using std::string;          using std::unordered_set;

string getFileName() {
    cout << "Enter a file name: ";
    string filename;
    getline(std::cin, filename);
    return filename;
}

string readFileIntoString(const string& filename) {
    std::ifstream ifs("res/" + filename);
    if (!ifs.is_open()) {
        std::cout << "No such file" << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}
/*
 * Note that you have to pass in the file as a single string
 * into the findWikiLinks function!
 * So we have to turn the passed-in file into a single string...
 * does that sound familiar at all?
 */
unordered_set<string> findWikiLinks(const string& page_html) {
    unordered_set<string> pageNames;
    const std::string wikiPattern = "\"/wiki/";
    auto start = page_html.begin();
    while (start != page_html.end()) {
        auto startOfPattern = std::search(start, page_html.end(), wikiPattern.begin(), wikiPattern.end());
        if (startOfPattern == page_html.end()) {
            break;
        }
        auto startOfPageName = startOfPattern + wikiPattern.length();
        auto endOfPageName = std::find(startOfPageName, page_html.end(), '"');

        if (endOfPageName != page_html.end() && std::all_of(startOfPageName, endOfPageName, [](auto c) {
            return c != ':' && c != '#';
        })) {
            pageNames.insert(string (startOfPageName, endOfPageName));
        }
        start = endOfPageName;
    }
    return pageNames;
}

int main() {
    string page_html;
    while (true) {
        page_html = readFileIntoString(getFileName());
        if (!page_html.empty()) {
            break;
        }
    }
    auto pageNames = findWikiLinks(page_html);
    for (auto& pageName : pageNames) {
        cout << pageName << endl;
    }
    return 0;

}
