#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

struct Course {
    string number;
    string title;
    vector<string> prereqs; // codes only
};

static string trim(string s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
static string upper(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return (char)toupper(c); });
    return s;
}

// split on TAB or comma
static vector<string> splitFields(const string& line) {
    vector<string> out; string cur;
    for (char ch : line) {
        if (ch == '\t' || ch == ',') { out.push_back(trim(cur)); cur.clear(); }
        else cur.push_back(ch);
    }
    out.push_back(trim(cur));
    return out;
}

static bool parseLine(const string& line, Course& c) {
    auto f = splitFields(line);
    if (f.size() < 2) return false;
    c.number = upper(f[0]);
    c.title  = f[1];
    c.prereqs.clear();
    for (size_t i = 2; i < f.size(); ++i)
        if (!f[i].empty()) c.prereqs.push_back(upper(f[i]));
    return true;
}

class Catalog {
public:
    map<string, Course> byCode; // sorted alphanumerically

    void clear() { byCode.clear(); }

    bool load(const string& filename, string& err) {
        clear();
        ifstream fin(filename);
        if (!fin) { err = "Error: Could not open file \"" + filename + "\"."; return false; }
        string line; size_t ln = 0, loaded = 0;
        while (getline(fin, line)) {
            ++ln; line = trim(line);
            if (line.empty()) continue;
            Course c;
            if (!parseLine(line, c)) { err = "Error: Malformed line " + to_string(ln) + "."; clear(); return false; }
            byCode[c.number] = c;
            ++loaded;
        }
        if (loaded == 0) { err = "Error: No courses found in file."; clear(); return false; }
        return true;
    }

    void printCourseList() const {
        cout << "Here is a sample schedule:\n";
        for (const auto& kv : byCode) {
            cout << kv.first << ", " << kv.second.title << "\n";
        }
    }

    bool printCourse(const string& code) const {
        string key = upper(trim(code));
        auto it = byCode.find(key);
        if (it == byCode.end()) {
            cout << key << " is not a course we offer.\n";
            return false;
        }
        const Course& c = it->second;
        cout << c.number << ", " << c.title << "\n";
        cout << "Prerequisites: ";
        if (c.prereqs.empty()) cout << "None\n";
        else {
            for (size_t i = 0; i < c.prereqs.size(); ++i) {
                if (i) cout << ", ";
                cout << c.prereqs[i];
            }
            cout << "\n";
        }
        return true;
    }
};

static void printMenu() {
    cout << "1. Load Data Structure.  \n";
    cout << "2. Print Course List.  \n";
    cout << "3. Print Course. \n";
    cout << "9. Exit \n";
    cout << "What would you like to do? ";
}

// Helper: if not loaded, prompt for filename, try to load.
static bool ensureLoaded(Catalog& catalog, bool& loaded) {
    if (loaded) return true;
    cout << "Please enter the file name to load: ";
    string fname; if (!getline(cin, fname)) return false;
    fname = trim(fname);
    if (fname.empty()) { cout << "File name cannot be empty.\n"; return false; }
    string err;
    if (catalog.load(fname, err)) {
        loaded = true;
        cout << "Data loaded successfully.\n";
        return true;
    } else {
        cout << err << "\n";
        return false;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Welcome to the course planner.\n";

    Catalog catalog;
    bool loaded = false;

    while (true) {
        printMenu();
        string opt;
        if (!getline(cin, opt)) break;
        opt = trim(opt);

        if (opt == "1") {
            cout << "Please enter the file name to load: ";
            string fname; getline(cin, fname); fname = trim(fname);
            if (fname.empty()) { cout << "File name cannot be empty.\n"; continue; }
            string err;
            if (catalog.load(fname, err)) {
                loaded = true;
                cout << "Data loaded successfully.\n";
            } else {
                loaded = false;
                cout << err << "\n";
            }

        } else if (opt == "2") {
            if (!ensureLoaded(catalog, loaded)) continue;
            catalog.printCourseList();

        } else if (opt == "3") {
            if (!ensureLoaded(catalog, loaded)) continue;
            cout << "What course do you want to know about? ";
            string code; getline(cin, code);
            catalog.printCourse(code);

        } else if (opt == "9") {
            cout << "Thank you for using the course planner!\n";
            break;

        } else {
            cout << opt << " is not a valid option.\n";
        }
    }
    return 0;
}

