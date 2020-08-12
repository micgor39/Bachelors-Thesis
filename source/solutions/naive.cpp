#include "naive.h"

int naive::make_string(std::string &word) {
    stream.push_back(word);
    return stream.size() - 1;
}

int naive::concat(int label1, int label2) {
    stream.push_back(stream[label1] + stream[label2]);
    return stream.size() - 1;
}

std::pair<int, int> naive::split(int label, int position) {
    stream.push_back(stream[label].substr(0, position));
    stream.push_back(stream[label].substr(position));
    return {stream.size() - 2, stream.size() - 1};
}

bool naive::equals(int label1, int label2) {
    return stream[label1] == stream[label2];
}

bool naive::smaller(int label1, int label2) {
    return stream[label1] < stream[label2];
}

int naive::longest_common_prefix(int label1, int label2) {
    size_t shorter_word_length = std::min(stream[label1].size(), stream[label2].size());
    for(size_t i = 0; i < shorter_word_length; i++) {
        if(stream[label1][i] != stream[label2][i])
            return i;
    }
    return shorter_word_length;
}