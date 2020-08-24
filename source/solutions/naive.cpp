#include "naive.h"

naive::~naive() {
    stream.clear();
    stream.shrink_to_fit();
}

int naive::make_string(std::vector<int> &word) {
    stream.push_back(word);
    return stream.size() - 1;
}

int naive::concat(int label1, int label2) {
    auto concatenation = stream[label1];
    concatenation.insert(concatenation.end(), stream[label2].begin(),
                         stream[label2].end());
    stream.push_back(concatenation);
    return stream.size() - 1;
}

std::pair<int, int> naive::split(int label, int position) {
    stream.push_back(std::vector<int>(stream[label].begin(),
                                      stream[label].begin() + position));
    stream.push_back(std::vector<int>(stream[label].begin() + position,
                                      stream[label].end()));
    return {stream.size() - 2, stream.size() - 1};
}

bool naive::equals(int label1, int label2) {
    return stream[label1] == stream[label2];
}

bool naive::smaller(int label1, int label2) {
    return stream[label1] < stream[label2];
}

int naive::longest_common_prefix(int label1, int label2) {
    size_t shorter_word_length =
        std::min(stream[label1].size(), stream[label2].size());
    for (size_t i = 0; i < shorter_word_length; i++) {
        if (stream[label1][i] != stream[label2][i])
            return i;
    }
    return shorter_word_length;
}