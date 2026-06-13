#ifndef __CORPUS__H_
#define __CORPUS__H_

#include <cctype>
#include <fstream>
#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

struct Corpus {
    std::string                   buffer;
    std::vector<std::string_view> words;
};

static Corpus load_corpus(const std::string& path) {
    Corpus corpus;

    std::ifstream file(path);
    corpus.buffer.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    std::string_view buf(corpus.buffer);

    size_t i = 0;
    while (i < buf.size()) {

        while (i < buf.size() && !std::isalnum(static_cast<unsigned char>(buf[i]))) {
            i++;
        }

        size_t start = i;

        while (i < buf.size() && std::isalnum(static_cast<unsigned char>(buf[i]))) {
            i++;
        }

        if (start < i) {
            corpus.words.emplace_back(buf.substr(start, i - start));
        }
    }

    return corpus;
}

#endif // !__CORPUS__H_
