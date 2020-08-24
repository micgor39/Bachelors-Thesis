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

bool parsings::is_rle(int symbol) {
    assert(symbol != -1);
    if(symbol >= 0) {
        return rle[symbol];
    } else {
        return false;
    }
}

int parsings::create_new_rle_symbol(int symbol, int quantity) {
    int index = symbols_counter++;
    rules.push_back({symbol, quantity});
    inverse_rle_rules[{symbol, quantity}] = index;
    level.push_back(get_level(symbol) + 1);
    length.push_back(get_length(symbol) * quantity);
    rle.push_back(true);
    return index;
}

int parsings::create_new_shrink_symbol(int left_symbol, int right_symbol) {
    int index = symbols_counter++;
    rules.push_back({left_symbol, right_symbol});
    inverse_shrink_rules[{left_symbol, right_symbol}] = index;
    level.push_back(get_level(left_symbol) + 1);
    length.push_back(get_length(left_symbol) + get_length(right_symbol));
    rle.push_back(false);
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
    assert(1 <= k && k <= get_length(symbol));
    if(get_level(symbol) == 0) {
        return -symbol - 2;
    } else if(get_level(symbol) % 2 == 0) {
        int left_symbol = rules[symbol].first,
            right_symbol = rules[symbol].second;
        if(k <= get_length(left_symbol)) {
            return get_kth_character(left_symbol, k);
        } else {
            return get_kth_character(right_symbol, k - get_length(left_symbol));
        }
    } else {
        int child_symbol = rules[symbol].first;
        return get_kth_character(child_symbol, (k - 1) % get_length(child_symbol) + 1);
    }
}

void parsings::get_path_to_kth_leaf(int symbol, int k, std::vector<std::tuple<int, int, int>> &dest, int siblings, bool left, int index) {
    assert(k <= get_length(symbol));
    dest.push_back({symbol, index, siblings});
    if(get_level(symbol) == 0) {
        return;
    } else if(get_level(symbol) % 2 == 0) {
        int left_symbol = rules[symbol].first,
            right_symbol = rules[symbol].second;
            assert(get_length(left_symbol) + get_length(right_symbol) == get_length(symbol));
        if(k <= get_length(left_symbol)) {
            if(right_symbol != -1 && left) {
                get_path_to_kth_leaf(left_symbol, k, dest, -1, left, index);
            } else {
                get_path_to_kth_leaf(left_symbol, k, dest, 1, left, index);
            }
        } else {
            if(right_symbol != -1 && !left) {
                get_path_to_kth_leaf(right_symbol, k - get_length(left_symbol), dest, -1, left, index + get_length(left_symbol));    
            } else {
                get_path_to_kth_leaf(right_symbol, k - get_length(left_symbol), dest, 1, left, index + get_length(left_symbol));
            }
        }
    } else {
        int child_symbol = rules[symbol].first, children = rules[symbol].second;
        int new_siblings = left ?
                           children - (k - 1) / get_length(child_symbol) :
                           (k - 1) / get_length(child_symbol) + 1;
        get_path_to_kth_leaf(child_symbol, (k - 1) % get_length(child_symbol) + 1, dest, new_siblings, left, index + (k - 1) / get_length(child_symbol) * get_length(child_symbol));
    }
}

std::vector<std::pair<int, int>> parsings::context_insensitive_decomposition(int symbol, int from, int to) {
    std::vector<std::tuple<int, int, int>> left_path, right_path;
    get_path_to_kth_leaf(symbol, from, left_path, 0, true, 0);
    get_path_to_kth_leaf(symbol, to, right_path, 0, false, 0);
    // std::cerr << "from = " << from << " to = " << to << "\n";
    // int expected_length = get_length(symbol);
    std::vector<std::pair<int, int>> left_decomposition, right_decomposition;
    while(std::get<1>(left_path.back()) < std::get<1>(right_path.back()) && std::get<1>(left_path[left_path.size() - 2]) < std::get<1>(right_path[right_path.size() - 2])) {
        // left decomposition
        if(std::get<2>(left_path.back()) == -1) {
            left_path.pop_back();
        } else {
            left_decomposition.push_back({std::get<0>(left_path.back()), std::get<2>(left_path.back())});
            int expected_level = get_level(std::get<0>(left_path.back())) + 1;
            left_path.pop_back();
            int steps_up = 0;
            while(std::get<2>(left_path.back()) == 1) {
                steps_up++;
                left_path.pop_back();
            }
            auto last = left_path.back();
            int last_symbol = std::get<0>(last);
            left_path.pop_back();
            auto parent = left_path.back();
            int parent_symbol = std::get<0>(parent);
            if(is_rle(parent_symbol)) {
                left_path.push_back({
                    last_symbol,
                    std::get<1>(last) + get_length(last_symbol),
                    std::get<2>(last) - 1
                });
            } else {
                left_path.push_back({
                    rules[parent_symbol].second,
                    std::get<1>(parent) + get_length(rules[parent_symbol].first),
                    1
                });
            }
            for(int i = 0; i < steps_up; i++) {
                last = left_path.back();
                last_symbol = std::get<0>(last);
                if(is_rle(last_symbol)) {
                    left_path.push_back({
                        rules[last_symbol].first,
                        std::get<1>(last),
                        rules[last_symbol].second
                    });
                } else {
                    left_path.push_back({
                        rules[last_symbol].first,
                        std::get<1>(last),
                        rules[last_symbol].second == -1 ? 1 : -1
                    });
                }
            }
            // std::cerr << "level " << get_level(std::get<0>(left_path.back())) << " " << expected_level - 1 << " symbol " << std::get<0>(left_path.back()) << " " << std::get<2>(left_path.back()) << "\n";
            assert(get_level(std::get<0>(left_path.back())) == expected_level);
        }
        // right decomposition
        if(std::get<2>(right_path.back()) == -1) {
            right_path.pop_back();
        } else {
            right_decomposition.push_back({std::get<0>(right_path.back()), std::get<2>(right_path.back())});
            right_path.pop_back();
            int steps_up = 0;
            while(std::get<2>(right_path.back()) == 1) {
                steps_up++;
                right_path.pop_back();
            }
            auto last = right_path.back();
            int last_symbol = std::get<0>(last);
            right_path.pop_back();
            auto parent = right_path.back();
            int parent_symbol = std::get<0>(parent);
            if(is_rle(parent_symbol)) {
                right_path.push_back({
                    last_symbol,
                    std::get<1>(last) - get_length(last_symbol),
                    std::get<2>(last) - 1
                });
            } else {
                right_path.push_back({
                    rules[parent_symbol].first,
                    std::get<1>(parent),
                    1
                });
            }
            for(int i = 0; i < steps_up; i++) {
                last = right_path.back();
                last_symbol = std::get<0>(last);
                if(is_rle(last_symbol)) {
                    right_path.push_back({
                        rules[last_symbol].first,
                        std::get<1>(last) + get_length(rules[last_symbol].first) * (rules[last_symbol].second - 1),
                        rules[last_symbol].second
                    });
                } else {
                    right_path.push_back({
                        (rules[last_symbol].second == -1 ? rules[last_symbol].first : rules[last_symbol].second),
                        (rules[last_symbol].second == -1 ? std::get<1>(last) : std::get<1>(last) + get_length(rules[last_symbol].first)),
                        (rules[last_symbol].second == -1 ? 1 : -1)
                    });
                }
            }
        }
    }

    // bool wrong = false;
    // if(std::get<1>(left_path.back()) > std::get<1>(right_path.back()) && left_decomposition.back().first == right_decomposition.back().first) {
    //     wrong = get_level(left_decomposition.back().first) % 2 == 1;
    // }

    // if(debug || wrong) {
    //     std::cerr << "begin left path\n";
    //     for(auto x: left_path) {
    //         std::cerr << std::get<0>(x) << " " << std::get<1>(x) << " " << std::get<2>(x) << "\n";
    //     }
    //     std::cerr << "end left path\n";

    //     std::cerr << "begin right path\n";
    //     for(auto x: right_path) {
    //         std::cerr << std::get<0>(x) << " " << std::get<1>(x) << " " << std::get<2>(x) << "\n";
    //     }
    //     std::cerr << "end right path\n";
        
    //     std::cerr << "begin left internal decomposition\n";
    //     for(auto x: left_decomposition) {
    //         std::cerr << x.first << " " << x.second << "\n";
    //     }
    //     std::cerr << "end left decomposition\n";
    //     std::cerr << "begin right internal decomposition\n";
    //     for(auto x: right_decomposition) {
    //         std::cerr << x.first << " " << x.second << " " << "\n";
    //     }
    //     std::cerr << "end right decomposition\n";

    //     std::cerr << "jestem tutaj2 " << std::get<1>(right_path.back()) << " " <<  std::get<1>(left_path.back()) << " " << get_length(std::get<0>(left_path.back())) << "\n";
    // }

    if(std::get<1>(left_path.back()) < std::get<1>(right_path.back())) {
        if(std::get<0>(left_path.back()) == std::get<0>(right_path.back())) {
            // std::cerr << "jestem tutaj " << std::get<1>(right_path.back()) << " " <<  std::get<1>(left_path.back()) << " " << get_length(std::get<0>(left_path.back())) << "\n";
            left_decomposition.push_back({std::get<0>(left_path.back()), (std::get<1>(right_path.back()) - std::get<1>(left_path.back())) / get_length(std::get<0>(left_path.back())) + 1});
        } else {
            left_decomposition.push_back({std::get<0>(left_path.back()), 1});
            left_decomposition.push_back({std::get<0>(right_path.back()), 1});
        }
    } else if(std::get<1>(left_path.back()) == std::get<1>(right_path.back())) {
        left_decomposition.push_back({std::get<0>(left_path.back()), 1});
    } else if(left_decomposition.back().first == right_decomposition.back().first) {
        left_decomposition.back().second += right_decomposition.back().second;
        right_decomposition.pop_back();
        assert(get_level(left_decomposition.back().first) % 2 == 0);
        // wrong = get_level(left_decomposition.back().first) % 2 == 1;
    }
    while(!right_decomposition.empty()) {
        left_decomposition.push_back(right_decomposition.back());
        right_decomposition.pop_back();
    }

    // int result_length = 0;
    // for(auto x: left_decomposition) {
    //     result_length += get_length(x.first) * x.second;
    // }
    // if(wrong) {
    //     std::cerr << "rule: " << rules[11].first << " " << rules[11].second << "\n";
    //     std::cerr << "s" << 1 << ": " << get_kth_character(symbol, 1) << "\n";
    //     std::cerr << "s" << 2 << ": " << get_kth_character(symbol, 2) << "\n";
    //     std::cerr << "s" << 3 << ": " << get_kth_character(symbol, 3) << "\n";
    //     std::cerr << "s" << 4 << ": " << get_kth_character(symbol, 4) << "\n";
    //     std::cerr << expected_length << " " << result_length << "\n";
    // }
    // assert(result_length == expected_length);


    // if(debug || wrong) {
    //     std::cerr << "begin decomposition\n";
    //     for(auto x: left_decomposition) {
    //         std::cerr << x.first << " " << x.second << " " << get_level(x.first) << "\n";
    //     }
    //     std::cerr << "end decomposition\n";
    // }

    // assert(!wrong);

    return left_decomposition;
}

int parsings::find_representant(std::vector<std::pair<int, int>> decomposition) {
    // symbol, multiplicity, index
    std::vector<std::vector<std::tuple<int, int, int>>> levels;
    int accumulator = 0;
    for(int i = 0; i < (int)decomposition.size(); i++) {
        int symbol = decomposition[i].first, quantity = decomposition[i].second;
        if((int)levels.size() <= get_level(symbol)) {
            levels.resize(get_level(symbol) + 1);
        }
        levels[get_level(symbol)].push_back({symbol, quantity, accumulator});
        accumulator += get_length(symbol) * quantity;
    }
    size_t i = 0;
    while(!(i + 1 == levels.size() && levels[i].size() == 1 && std::get<1>(levels[i][0]) == 1)) {
        // if(i > 300) break;
        std::vector<std::tuple<int, int, int>> next_level;
        size_t index = 0;
        
        for(size_t j = 0; j < levels[i].size(); j++) {
            // std::cerr << i << " " << j << "\n";
            while(i + 1 < levels.size() && index < levels[i + 1].size() 
            && std::get<2>(levels[i + 1][index]) < std::get<2>(levels[i][j])) {
                next_level.push_back(levels[i + 1][index]);
                index++;
            }
            if(i % 2 == 0) {
                if(j + 1 < levels[i].size() 
                && std::get<2>(levels[i][j]) + get_length(std::get<0>(levels[i][j])) * std::get<1>(levels[i][j]) == std::get<2>(levels[i][j + 1]) 
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
                && std::get<2>(levels[i][j]) + get_length(std::get<0>(levels[i][j])) == std::get<2>(levels[i][j + 1])
                && !get_random_bit(std::get<0>(levels[i][j])) && get_random_bit(std::get<0>(levels[i][j + 1]))) {
                    next_level.push_back({
                        get_shrink_symbol(std::get<0>(levels[i][j]), std::get<0>(levels[i][j + 1])),
                        1,
                        std::get<2>(levels[i][j])
                    });
                    j++;
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
        if(i + 1 == levels.size()) {
            levels.push_back(next_level);
        } else {
            levels[i + 1] = next_level;
        }
        i++;
    }

    // if(i > 300) {
    //     for(auto x: decomposition) {
    //         std::cerr << x.first << " " << x.second << " " << get_length(x.first) << " " << get_level(x.first) << "\n";
    //     }
    //     for(int jj = 0; jj < i + 1; jj++) {
    //         std::cerr << "level " << jj << "\n";
    //         for(size_t j = 0; j < levels[jj].size(); j++) {
    //             std::cerr << jj << " " << j << " " << std::get<0>(levels[jj][j]) << " " << std::get<1>(levels[jj][j]) << " " << std::get<2>(levels[jj][j]) << " length " << get_length(std::get<0>(levels[jj][j])) << "\n";
    //         }
    //     }
    //     assert(false);
    // }

    return std::get<0>(levels[i][0]);
}

parsings::parsings(int seed, bool _debug) {
    debug = _debug;
    random_number_generator.seed(seed);
}

int parsings::make_string(std::vector<int> &word) {
    std::vector<std::pair<int, int>> decomposition(word.size());
    for(size_t i = 0; i < word.size(); i++) {
        decomposition[i] = {-word[i] - 2, 1};
    }
    return find_representant(decomposition);
}

int parsings::concat(int label1, int label2) {
    int expected_length = get_length(label1) + get_length(label2);
    //  std::cerr << "expected length " << expected_length << "\n";
    std::vector<std::pair<int, int>> left_decomposition, right_decomposition;
    left_decomposition = context_insensitive_decomposition(label1, 1, get_length(label1));
    
    // std::cerr << "begin left decomposition, length = " << get_length(label1) << "\n";
    // for(auto x: left_decomposition) {
    //     std::cerr << x.first << " " << x.second << " " << get_length(x.first) << "\n";
    // }
    // std::cerr << "end left decomposition\n";
    
    right_decomposition = context_insensitive_decomposition(label2, 1, get_length(label2));
    
    // std::cerr << "begin right decomposition, length = " << get_length(label2) << "\n";
    // for(auto x: right_decomposition) {
    //     std::cerr << x.first << " " << x.second << " " << get_length(x.first) << "\n";
    // }
    // std::cerr << "end right decomposition\n";
    
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
    
    // std::cerr << "begin concat decomposition\n";
    // for(auto x: concatenated_decomposition) {
    //     std::cerr << x.first << " " << x.second << " " << get_length(x.first) << "\n";
    // }
    // std::cerr << "end concat decomposition\n";
    
    int ret = find_representant(concatenated_decomposition);
    // std::cerr << "expected length " << expected_length << " vs " << get_length(ret) << "\n";
    assert(get_length(ret) == expected_length);
    return ret;
}

std::pair<int, int> parsings::split(int label, int position) {
    // std::cerr << label << " " << get_length(label) << " " << position << "\n";
    assert(1 <= position && position < get_length(label));
    return {
        find_representant(context_insensitive_decomposition(label, 1, position)),
        find_representant(context_insensitive_decomposition(label, position + 1, get_length(label)))
    };
}

bool parsings::equals(int label1, int label2) {
    return label1 == label2;
}

int parsings::longest_common_prefix(int label1, int label2) {
    // int max_possible_lcp = std::min(get_length(label1), get_length(label2));
    // int left_pointer = 0, right_pointer = max_possible_lcp + 1;
    // while(left_pointer + 1 < right_pointer) {
    //     int guess = (left_pointer + right_pointer) / 2;
    //     auto s1 = context_insensitive_decomposition(label1, 1, guess);
    //     auto s2 = context_insensitive_decomposition(label2, 1, guess);
    //     if(s1 == s2) {
    //         left_pointer = guess;
    //     } else {
    //         right_pointer = guess;
    //     }
    // }
    // return left_pointer;
    return 0;
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