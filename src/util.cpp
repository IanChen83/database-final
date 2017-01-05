#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>
#include <vector>
#include "util.h"

using namespace std;

// trim from start (in place)
static inline void
ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void
rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void
trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

std::vector<std::string>
tokenize(const std::string& s, char c, bool trim_space) {
    auto end = s.cend();
    auto start = end;

    std::vector<std::string> v;
    for( auto it = s.cbegin(); it != end; ++it ) {
        if( *it != c ) {
            if( start == end )
                start = it;
            continue;
        }
        if( start != end ) {
            v.emplace_back(start, it);
            start = end;
        }
    }
    if( start != end )
        v.emplace_back(start, end);

    if(trim_space) {
        for(auto& x: v)
            trim(x);
    }
    return v;
}

// Use a state machine to split texts
std::vector<std::string>
tokenizeEscape(const std::string& s, char c, bool trim_space) {
    bool inQuote = false;
    auto it = s.cbegin(), start = it;
    auto end = s.cend();

    std::vector<std::string> v;

    while(it != end) {
        if(*it == '"') {
            inQuote = !inQuote;
            ++it;
            continue;
        }

        if(*it == c && !inQuote) {
            if(start != it) {
                v.emplace_back(start, it);
            }
            start = ++it;
        } else {
            ++it;
        }
    }

    if(start != end) {
        v.emplace_back(start, end);
    }

    if(trim_space) {
        for(auto& x: v)
            trim(x);
    }
    return v;
}
