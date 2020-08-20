#include "parsings.h"

bool parsings::get_random_bit(int symbol) {
    auto iterator = random_bit.find(symbol);
    if(iterator == random_bit.end()) {
        bool drew_bit = std::uniform_int_distribution<int>(0, 1)(random_number_generator);
        random_bit[symbol] = drew_bit;
        return drew_bit;
    }
    return iterator->second;
}

int parsings::get_length(int symbol) {
    if(symbol >= 0) {
        return length[symbol];
    } else if (symbol == -1) {
        return 0;
    } else {
        return 1;
    }
}

int parsings::get_level(int symbol) {
    assert(symbol != -1);
    if(symbol >= 0) {
        return level[symbol];
    } else {
        return 0;
    }
}

int parsings::create_new_rle_symbol(int symbol, int quantity) {
    int index = symbols_counter++;
    rle_rules.push_back({symbol, quantity});
    inverse_rle_rules[{symbol, quantity}] = index;
    level.push_back(get_level(symbol) + 1);
    length.push_back(get_length(symbol) * quantity);
    return index;
}

int parsings::create_new_shrink_symbol(int left_symbol, int right_symbol) {
    int index = symbols_counter++;
    shrink_rules.push_back({left_symbol, right_symbol});
    inverse_shrink_rules[{left_symbol, right_symbol}] = index;
    level.push_back(get_level(left_symbol) + 1);
    length.push_back(get_length(left_symbol) + get_length(right_symbol));
    return index;
}

int parsings::get_rle_symbol(int symbol, int quantity) {
    auto iterator = inverse_rle_rules.find({symbol, quantity});
    if(iterator == inverse_rle_rules.end()) {
        int new_symbol = create_new_rle_symbol(symbol, quantity);
        return new_symbol;
    } else {
        return iterator->second;
    }
}

int parsings::get_shrink_symbol(int left_symbol, int right_symbol) {
    auto iterator = inverse_shrink_rules.find({left_symbol, right_symbol});
    if(iterator == inverse_shrink_rules.end()) {
        int new_symbol = create_new_shrink_symbol(left_symbol, right_symbol);
        return new_symbol;
    } else {
        return iterator->second;
    }
}

int parsings::get_kth_character(int symbol, int k) {
    assert(get_length(symbol) <= k);
    if(get_level(symbol) == 0) {
        return -symbol - 2;
    } else if(get_level(symbol) % 2 == 0) {
        int left_symbol = shrink_rules[symbol].first,
            right_symbol = shrink_rules[symbol].second;
        if(get_length(left_symbol) <= k) {
            return get_kth_character(left_symbol, k);
        } else {
            return get_kth_character(right_symbol, k - get_length(left_symbol));
        }
    } else {
        int child_symbol = shrink_rules[symbol].first;
        return get_kth_character(child_symbol, (k - 1) % get_length(child_symbol) + 1);
    }
}

std::vector<std::pair<int, int>> parsings::context_insensitive_decomposition(int symbol, int from, int to) {
    
}

int parsings::find_representant(std::vector<std::pair<int, int>> decomposition) {
    std::vector<std::vector<std::tuple<int, int, int>>> levels;
    int accumulator = 0;
    for(int i = 0; i < (int)decomposition.size(); i++) {
        int symbol = decomposition[i].first, quantity = decomposition[i].second;
        if((int)levels.size() <= get_level(symbol)) {
            levels.resize(level[symbol]);
        }
        levels[symbol].push_back({symbol, quantity, accumulator});
        accumulator += get_length(symbol);
    }
    size_t i = 0;
    while(!(i + 1 == levels.size() && levels[i].size() == 1)) {
        std::vector<std::tuple<int, int, int>> next_level;
        size_t index = 0;
        for(size_t j = 0; j < levels[i].size(); j++) {
            while(i + 1 < levels.size() && index < levels[i + 1].size() 
            && std::get<2>(levels[i + 1][index]) < std::get<2>(levels[i][j])) {
                next_level.push_back(levels[i + 1][index]);
                index++;
            }
            if(i % 2 == 0) {
                if(j + 1 < levels[i].size() 
                && std::get<2>(levels[i][j]) + get_length(std::get<0>(levels[i][j])) == std::get<2>(levels[i][j + 1]) 
                && std::get<0>(levels[i][j]) == std::get<0>(levels[i][j + 1])) {
                    levels[i][j + 1] = {
                        std::get<0>(levels[i][j + 1]),
                        std::get<1>(levels[i][j]) + std::get<1>(levels[i][j + 1]),
                        std::get<2>(levels[i][j])
                    };
                } else {
                    next_level.push_back({
                        get_rle_symbol(std::get<0>(levels[i][j]), std::get<1>(levels[i][j])),
                        1,
                        std::get<2>(levels[i][j])
                    });
                }
            } else {
                assert(std::get<1>(levels[i][j]) == 1);
                if(j + 1 < levels[i].size()
                && !get_random_bit(std::get<0>(levels[i][j])) && get_random_bit(std::get<0>(levels[i][j + 1]))) {
                    next_level.push_back({
                        get_shrink_symbol(std::get<0>(levels[i][j]), std::get<1>(levels[i][j])),
                        1,
                        std::get<2>(levels[i][j])
                    });
                } else {
                    next_level.push_back({
                        get_shrink_symbol(std::get<0>(levels[i][j]), -1),
                        1,
                        std::get<2>(levels[i][j])
                    });
                }
            }
        }
        while(i + 1 < levels.size() && index < levels[i + 1].size()) {
            next_level.push_back(levels[i + 1][index]);
            index++;
        }
        i++;
        if(i + 1 == levels.size()) {
            levels.push_back(next_level);
        } else {
            levels[i + 1] = next_level;
        }
    }
    return std::get<0>(levels[i][0]);
}

parsings::parsings(int seed) {
    random_number_generator.seed(seed);
}

int parsings::make_string(std::vector<int> &word) {
    std::vector<std::pair<int, int>> decomposition(word.size());
    for(size_t i = 0; i < word.size(); i++) {
        decomposition[i] = {-i - 2, 1};
    }
    return find_representant(decomposition);
}

int parsings::concat(int label1, int label2) {
    std::vector<std::pair<int, int>> left_decomposition, right_decomposition;
    left_decomposition = context_insensitive_decomposition(label1, 1, get_length(label1));
    right_decomposition = context_insensitive_decomposition(label2, 1, get_length(label2));
    std::vector<std::pair<int, int>> concatenated_decomposition = left_decomposition;
    size_t start_copying_from = 0;
    if(left_decomposition.back().first == right_decomposition[0].first) {
        // check
        concatenated_decomposition.back().second += right_decomposition[0].second;
        start_copying_from = 1;
    }
    for(size_t i = start_copying_from; i < right_decomposition.size(); i++) {
        concatenated_decomposition.push_back(right_decomposition[i]);
    }
    return find_representant(concatenated_decomposition);
}

std::pair<int, int> parsings::split(int label, int position) {
    assert(1 <= position && position <= get_length(label));
}

bool parsings::equals(int label1, int label2) {
    return label1 == label2;
}

int parsings::longest_common_prefix(int label1, int label2) {
    // todo: change
    int max_possible_lcp = std::min(get_length(label1), get_length(label2));
    // binary search the answer
    int left_pointer = 0, right_pointer = max_possible_lcp + 1;
    while(left_pointer + 1 < right_pointer) {
        int guess = (left_pointer + right_pointer) / 2;
        auto s1 = find_representant(context_insensitive_decomposition(label1, 1, guess));
        auto s2 = find_representant(context_insensitive_decomposition(label2, 1, guess));
        if(s1 == s2) {
            left_pointer = guess;
        } else {
            right_pointer = guess;
        }
    }
    return left_pointer;
}

bool parsings::smaller(int label1, int label2) {
    int lcp = longest_common_prefix(label1, label2);
    if(lcp == get_length(label2)) {
        return false;
    }
    if(lcp == get_length(label1)) {
        return true;
    }
    return get_kth_character(label1, lcp + 1) < get_kth_character(label2, lcp + 1);
}