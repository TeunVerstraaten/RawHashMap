#ifndef __CORPUS__H_
#define __CORPUS__H_

#include <string>
#include <string_view>
#include <vector>

struct Corpus {
    std::string                   buffer;
    std::vector<std::string_view> words;
};

Corpus load_corpus(const std::string& path);

#endif // !__CORPUS__H_
