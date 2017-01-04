#ifndef UTIL_HEADER
#define UTIL_HEADER

#include <string>
#include <vector>

static inline void ltrim(std::string& s);
static inline void rtrim(std::string& s);
static inline void trim(std::string& s);
std::vector<std::string> tokenize(const std::string&, char, bool = false);

#endif
