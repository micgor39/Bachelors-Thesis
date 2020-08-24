#include "parsings.h"

size_t
parsings::own_pair_hash::operator()(const std::pair<int, int> &pair) const {
    return std::hash<int>{}(pair.first) ^ std::hash<int>{}(pair.second);
}

bool parsings::get_random_bit(int symbol) {
    auto iterator = random_bit.find(symbol);
    if (iterator == random_bit.end()) {
        bool drew_bit =
            std::uniform_int_distribution<int>(0, 1)(random_number_generator);
        random_bit[symbol] = drew_bit;
        return drew_bit;
    }
    return iterator->second;
}

int parsings::get_length(int symbol) {
    if (symbol >= 0) {
        return grammar[symbol].length;
    } else if (symbol == -1) {
        return 0;
    } else {
        return 1;
    }
}

int parsings::get_level(int symbol) {
    assert(symbol != -1);
    if (symbol >= 0) {
        return grammar[symbol].level;
    } else {
        return 0;
    }
}

bool parsings::is_rle(int symbol) {
    assert(symbol != -1);
    if (symbol >= 0) {
        return grammar[symbol].rle;
    } else {
        return false;
    }
}

int parsings::create_new_rle_symbol(int symbol, int quantity) {
    int index = symbols_counter++;
    if (symbol < 0) {
        level0_inverse_rules[{symbol, quantity}] = index;
    } else {
        grammar[symbol].inverse_rules[quantity] = index;
    }
    grammar.push_back({std::make_pair(symbol, quantity),
                       get_level(symbol) + 1,
                       get_length(symbol) * quantity,
                       true,
                       {}});
    return index;
}

int parsings::create_new_shrink_symbol(int left_symbol, int right_symbol) {
    int index = symbols_counter++;
    grammar[left_symbol].inverse_rules[right_symbol] = index;
    grammar.push_back({std::make_pair(left_symbol, right_symbol),
                       get_level(left_symbol) + 1,
                       get_length(left_symbol) + get_length(right_symbol),
                       false,
                       {}});
    return index;
}

int parsings::get_rle_symbol(int symbol, int quantity) {
    if (symbol < 0) {
        auto iterator = level0_inverse_rules.find({symbol, quantity});
        if (iterator == level0_inverse_rules.end()) {
            int new_symbol = create_new_rle_symbol(symbol, quantity);
            return new_symbol;
        } else {
            return iterator->second;
        }
    } else {
        auto iterator = grammar[symbol].inverse_rules.find(quantity);
        if (iterator == grammar[symbol].inverse_rules.end()) {
            int new_symbol = create_new_rle_symbol(symbol, quantity);
            return new_symbol;
        } else {
            return iterator->second;
        }
    }
}

int parsings::get_shrink_symbol(int left_symbol, int right_symbol) {
    assert(left_symbol >= 0);
    auto iterator = grammar[left_symbol].inverse_rules.find(right_symbol);
    if (iterator == grammar[left_symbol].inverse_rules.end()) {
        int new_symbol = create_new_shrink_symbol(left_symbol, right_symbol);
        return new_symbol;
    } else {
        return iterator->second;
    }
}

int parsings::get_kth_character(int symbol, int k) {
    assert(1 <= k && k <= get_length(symbol));
    if (get_level(symbol) == 0) {
        return -symbol - 2;
    } else if (get_level(symbol) % 2 == 0) {
        int left_symbol = grammar[symbol].rule.first,
            right_symbol = grammar[symbol].rule.second;
        if (k <= get_length(left_symbol)) {
            return get_kth_character(left_symbol, k);
        } else {
            return get_kth_character(right_symbol, k - get_length(left_symbol));
        }
    } else {
        int child_symbol = grammar[symbol].rule.first;
        return get_kth_character(child_symbol,
                                 (k - 1) % get_length(child_symbol) + 1);
    }
}

void parsings::get_path_to_kth_leaf(
    int symbol, int k, std::vector<std::tuple<int, int, int>> &dest,
    int siblings, bool left, int index) {
    assert(k <= get_length(symbol));
    dest.push_back({symbol, index, siblings});
    if (get_level(symbol) == 0) {
        return;
    } else if (get_level(symbol) % 2 == 0) {
        int left_symbol = grammar[symbol].rule.first,
            right_symbol = grammar[symbol].rule.second;
        assert(get_length(left_symbol) + get_length(right_symbol) ==
               get_length(symbol));
        if (k <= get_length(left_symbol)) {
            if (right_symbol != -1 && left) {
                get_path_to_kth_leaf(left_symbol, k, dest, -1, left, index);
            } else {
                get_path_to_kth_leaf(left_symbol, k, dest, 1, left, index);
            }
        } else {
            if (right_symbol != -1 && !left) {
                get_path_to_kth_leaf(right_symbol, k - get_length(left_symbol),
                                     dest, -1, left,
                                     index + get_length(left_symbol));
            } else {
                get_path_to_kth_leaf(right_symbol, k - get_length(left_symbol),
                                     dest, 1, left,
                                     index + get_length(left_symbol));
            }
        }
    } else {
        int child_symbol = grammar[symbol].rule.first,
            children = grammar[symbol].rule.second;
        int new_siblings = left ? children - (k - 1) / get_length(child_symbol)
                                : (k - 1) / get_length(child_symbol) + 1;
        get_path_to_kth_leaf(child_symbol,
                             (k - 1) % get_length(child_symbol) + 1, dest,
                             new_siblings, left,
                             index + (k - 1) / get_length(child_symbol) *
                                         get_length(child_symbol));
    }
}

std::vector<std::pair<int, int>>
parsings::context_insensitive_decomposition(int symbol, int from, int to) {
    // symbol, index, right/left siblings
    std::vector<std::tuple<int, int, int>> left_path, right_path;
    get_path_to_kth_leaf(symbol, from, left_path, 0, true, 0);
    get_path_to_kth_leaf(symbol, to, right_path, 0, false, 0);
    std::vector<std::pair<int, int>> left_decomposition, right_decomposition;
    while (std::get<1>(left_path.back()) < std::get<1>(right_path.back()) &&
           std::get<1>(left_path[left_path.size() - 2]) <
               std::get<1>(right_path[right_path.size() - 2])) {
        // left decomposition
        if (std::get<2>(left_path.back()) == -1) {
            left_path.pop_back();
        } else {
            left_decomposition.push_back(
                {std::get<0>(left_path.back()), std::get<2>(left_path.back())});
            int expected_level = get_level(std::get<0>(left_path.back())) + 1;
            left_path.pop_back();
            int steps_up = 0;
            while (std::get<2>(left_path.back()) == 1) {
                steps_up++;
                left_path.pop_back();
            }
            auto last = left_path.back();
            int last_symbol = std::get<0>(last);
            left_path.pop_back();
            auto parent = left_path.back();
            int parent_symbol = std::get<0>(parent);
            if (is_rle(parent_symbol)) {
                left_path.push_back(
                    {last_symbol, std::get<1>(last) + get_length(last_symbol),
                     std::get<2>(last) - 1});
            } else {
                left_path.push_back(
                    {grammar[parent_symbol].rule.second,
                     std::get<1>(parent) +
                         get_length(grammar[parent_symbol].rule.first),
                     1});
            }
            for (int i = 0; i < steps_up; i++) {
                last = left_path.back();
                last_symbol = std::get<0>(last);
                if (is_rle(last_symbol)) {
                    left_path.push_back({grammar[last_symbol].rule.first,
                                         std::get<1>(last),
                                         grammar[last_symbol].rule.second});
                } else {
                    left_path.push_back(
                        {grammar[last_symbol].rule.first, std::get<1>(last),
                         grammar[last_symbol].rule.second == -1 ? 1 : -1});
                }
            }
            assert(get_level(std::get<0>(left_path.back())) == expected_level);
        }
        // right decomposition
        if (std::get<2>(right_path.back()) == -1) {
            right_path.pop_back();
        } else {
            right_decomposition.push_back({std::get<0>(right_path.back()),
                                           std::get<2>(right_path.back())});
            right_path.pop_back();
            int steps_up = 0;
            while (std::get<2>(right_path.back()) == 1) {
                steps_up++;
                right_path.pop_back();
            }
            auto last = right_path.back();
            int last_symbol = std::get<0>(last);
            right_path.pop_back();
            auto parent = right_path.back();
            int parent_symbol = std::get<0>(parent);
            if (is_rle(parent_symbol)) {
                right_path.push_back(
                    {last_symbol, std::get<1>(last) - get_length(last_symbol),
                     std::get<2>(last) - 1});
            } else {
                right_path.push_back({grammar[parent_symbol].rule.first,
                                      std::get<1>(parent), 1});
            }
            for (int i = 0; i < steps_up; i++) {
                last = right_path.back();
                last_symbol = std::get<0>(last);
                if (is_rle(last_symbol)) {
                    right_path.push_back(
                        {grammar[last_symbol].rule.first,
                         std::get<1>(last) +
                             get_length(grammar[last_symbol].rule.first) *
                                 (grammar[last_symbol].rule.second - 1),
                         grammar[last_symbol].rule.second});
                } else if (grammar[last_symbol].rule.second == -1) {
                    right_path.push_back({grammar[last_symbol].rule.first,
                                          std::get<1>(last), 1});
                } else {
                    right_path.push_back(
                        {grammar[last_symbol].rule.second,
                         std::get<1>(last) +
                             get_length(grammar[last_symbol].rule.first),
                         -1});
                }
            }
        }
    }

    if (std::get<1>(left_path.back()) < std::get<1>(right_path.back())) {
        if (std::get<0>(left_path.back()) == std::get<0>(right_path.back())) {
            left_decomposition.push_back(
                {std::get<0>(left_path.back()),
                 (std::get<1>(right_path.back()) -
                  std::get<1>(left_path.back())) /
                         get_length(std::get<0>(left_path.back())) +
                     1});
        } else {
            left_decomposition.push_back({std::get<0>(left_path.back()), 1});
            left_decomposition.push_back({std::get<0>(right_path.back()), 1});
        }
    } else if (std::get<1>(left_path.back()) ==
               std::get<1>(right_path.back())) {
        left_decomposition.push_back({std::get<0>(left_path.back()), 1});
    } else if (left_decomposition.back().first ==
               right_decomposition.back().first) {
        left_decomposition.back().second += right_decomposition.back().second;
        right_decomposition.pop_back();
        assert(get_level(left_decomposition.back().first) % 2 == 0);
    }
    while (!right_decomposition.empty()) {
        left_decomposition.push_back(right_decomposition.back());
        right_decomposition.pop_back();
    }

    return left_decomposition;
}

int parsings::find_representant(
    std::vector<std::pair<int, int>> decomposition) {
    // symbol, multiplicity, index
    std::vector<std::vector<std::tuple<int, int, int>>> levels;
    int accumulator = 0;
    for (int i = 0; i < (int)decomposition.size(); i++) {
        int symbol = decomposition[i].first, quantity = decomposition[i].second;
        if ((int)levels.size() <= get_level(symbol)) {
            levels.resize(get_level(symbol) + 1);
        }
        levels[get_level(symbol)].push_back({symbol, quantity, accumulator});
        accumulator += get_length(symbol) * quantity;
    }
    size_t i = 0;
    while (!(i + 1 == levels.size() && levels[i].size() == 1 &&
             std::get<1>(levels[i][0]) == 1)) {
        std::vector<std::tuple<int, int, int>> next_level;
        size_t index = 0;

        for (size_t j = 0; j < levels[i].size(); j++) {
            while (i + 1 < levels.size() && index < levels[i + 1].size() &&
                   std::get<2>(levels[i + 1][index]) <
                       std::get<2>(levels[i][j])) {
                next_level.push_back(levels[i + 1][index]);
                index++;
            }
            if (i % 2 == 0) {
                if (j + 1 < levels[i].size() &&
                    std::get<2>(levels[i][j]) +
                            get_length(std::get<0>(levels[i][j])) *
                                std::get<1>(levels[i][j]) ==
                        std::get<2>(levels[i][j + 1]) &&
                    std::get<0>(levels[i][j]) ==
                        std::get<0>(levels[i][j + 1])) {
                    levels[i][j + 1] = {std::get<0>(levels[i][j + 1]),
                                        std::get<1>(levels[i][j]) +
                                            std::get<1>(levels[i][j + 1]),
                                        std::get<2>(levels[i][j])};
                } else {
                    next_level.push_back(
                        {get_rle_symbol(std::get<0>(levels[i][j]),
                                        std::get<1>(levels[i][j])),
                         1, std::get<2>(levels[i][j])});
                }
            } else {
                assert(std::get<1>(levels[i][j]) == 1);
                if (j + 1 < levels[i].size() &&
                    std::get<2>(levels[i][j]) +
                            get_length(std::get<0>(levels[i][j])) ==
                        std::get<2>(levels[i][j + 1]) &&
                    !get_random_bit(std::get<0>(levels[i][j])) &&
                    get_random_bit(std::get<0>(levels[i][j + 1]))) {
                    next_level.push_back(
                        {get_shrink_symbol(std::get<0>(levels[i][j]),
                                           std::get<0>(levels[i][j + 1])),
                         1, std::get<2>(levels[i][j])});
                    j++;
                } else {
                    next_level.push_back(
                        {get_shrink_symbol(std::get<0>(levels[i][j]), -1), 1,
                         std::get<2>(levels[i][j])});
                }
            }
        }
        while (i + 1 < levels.size() && index < levels[i + 1].size()) {
            next_level.push_back(levels[i + 1][index]);
            index++;
        }
        if (i + 1 == levels.size()) {
            levels.push_back(next_level);
        } else {
            levels[i + 1] = next_level;
        }
        i++;
    }

    return std::get<0>(levels[i][0]);
}

parsings::parsings(int seed, bool _debug) {
    debug = _debug;
    random_number_generator.seed(seed);
}

parsings::~parsings() {
    grammar.clear();
    grammar.shrink_to_fit();
    random_bit.clear();
    level0_inverse_rules.clear();
}

int parsings::make_string(std::vector<int> &word) {
    std::vector<std::pair<int, int>> decomposition(word.size());
    for (size_t i = 0; i < word.size(); i++) {
        decomposition[i] = {-word[i] - 2, 1};
    }
    return find_representant(decomposition);
}

int parsings::concat(int label1, int label2) {
    int expected_length = get_length(label1) + get_length(label2);
    std::vector<std::pair<int, int>> left_decomposition, right_decomposition;
    left_decomposition =
        context_insensitive_decomposition(label1, 1, get_length(label1));

    right_decomposition =
        context_insensitive_decomposition(label2, 1, get_length(label2));

    std::vector<std::pair<int, int>> concatenated_decomposition =
        left_decomposition;
    size_t start_copying_from = 0;
    if (left_decomposition.back().first == right_decomposition[0].first) {
        concatenated_decomposition.back().second +=
            right_decomposition[0].second;
        start_copying_from = 1;
    }
    for (size_t i = start_copying_from; i < right_decomposition.size(); i++) {
        concatenated_decomposition.push_back(right_decomposition[i]);
    }

    int ret = find_representant(concatenated_decomposition);
    assert(get_length(ret) == expected_length);
    return ret;
}

std::pair<int, int> parsings::split(int label, int position) {
    assert(1 <= position && position < get_length(label));
    return {find_representant(
                context_insensitive_decomposition(label, 1, position)),
            find_representant(context_insensitive_decomposition(
                label, position + 1, get_length(label)))};
}

bool parsings::equals(int label1, int label2) { return label1 == label2; }

int parsings::longest_common_prefix(int label1, int label2) {
    // symbol, index, right siblings
    std::vector<std::tuple<int, int, int>> path1 = {{label1, 0, 1}},
                                           path2 = {{label2, 0, 1}};
    while (get_level(label1) > get_level(label2)) {
        if (is_rle(label1)) {
            path1.push_back(
                {grammar[label1].rule.first, 0, grammar[label1].rule.second});
        } else {
            path1.push_back({grammar[label1].rule.first, 0,
                             grammar[label1].rule.second == -1 ? 1 : -1});
        }
        label1 = grammar[label1].rule.first;
    }
    while (get_level(label1) < get_level(label2)) {
        if (is_rle(label2)) {
            path2.push_back(
                {grammar[label2].rule.first, 0, grammar[label2].rule.second});
        } else {
            path2.push_back({grammar[label2].rule.first, 0,
                             grammar[label2].rule.second == -1 ? 1 : -1});
        }
        label2 = grammar[label2].rule.first;
    }

    while (get_level(label1) >= 0) {
        assert(label1 == std::get<0>(path1.back()));
        assert(label2 == std::get<0>(path2.back()));
        if (label1 == label2) {
            if (std::get<2>(path1.back()) > std::get<2>(path2.back())) {
                std::swap(path1, path2);
                std::swap(label1, label2);
            }
            if (!is_rle(label1) && !is_rle(label2) &&
                std::get<2>(path1.back()) < std::get<2>(path2.back())) {
                path2.back() = {
                    std::get<0>(path2.back()),
                    std::get<1>(path2.back()) +
                        get_length(label2) * std::get<2>(path1.back()),
                    std::get<2>(path2.back()) - std::get<2>(path1.back())};
                int steps_up = 0;
                if (std::get<2>(path1.back()) > 1) {
                    path1.pop_back();
                    steps_up++;
                }
                while (!path1.empty() && std::get<2>(path1.back()) == 1) {
                    path1.pop_back();
                    steps_up++;
                }
                if (path1.empty()) {
                    return std::get<1>(path2.back());
                }
                auto last = path1.back();
                int last_symbol = std::get<0>(last);
                path1.pop_back();
                auto parent = path1.back();
                int parent_symbol = std::get<0>(parent);
                if (is_rle(parent_symbol)) {
                    path1.push_back(
                        {last_symbol,
                         std::get<1>(last) + get_length(last_symbol),
                         std::get<2>(last) - 1});
                } else {
                    path1.push_back(
                        {grammar[parent_symbol].rule.second,
                         std::get<1>(parent) +
                             get_length(grammar[parent_symbol].rule.first),
                         1});
                }
                for (int i = 0; i < steps_up; i++) {
                    last = path1.back();
                    last_symbol = std::get<0>(last);
                    if (is_rle(last_symbol)) {
                        path1.push_back({grammar[last_symbol].rule.first,
                                         std::get<1>(last),
                                         grammar[last_symbol].rule.second});
                    } else {
                        path1.push_back(
                            {grammar[last_symbol].rule.first, std::get<1>(last),
                             grammar[last_symbol].rule.second == -1 ? 1 : -1});
                    }
                }
            } else /* == */ {
                int steps_up = 0;
                if (std::get<2>(path1.back()) > 1) {
                    path1.pop_back();
                    steps_up++;
                }
                while (!path1.empty() && std::get<2>(path1.back()) == 1) {
                    path1.pop_back();
                    steps_up++;
                }
                if (path1.empty()) {
                    return std::get<1>(path2.back()) +
                           get_length(label2) * abs(std::get<2>(path2.back()));
                }
                auto last = path1.back();
                int last_symbol = std::get<0>(last);
                path1.pop_back();
                auto parent = path1.back();
                int parent_symbol = std::get<0>(parent);
                if (is_rle(parent_symbol)) {
                    path1.push_back(
                        {last_symbol,
                         std::get<1>(last) + get_length(last_symbol),
                         std::get<2>(last) - 1});
                } else {
                    path1.push_back(
                        {grammar[parent_symbol].rule.second,
                         std::get<1>(parent) +
                             get_length(grammar[parent_symbol].rule.first),
                         1});
                }
                for (int i = 0; i < steps_up; i++) {
                    last = path1.back();
                    last_symbol = std::get<0>(last);
                    if (is_rle(last_symbol)) {
                        path1.push_back({grammar[last_symbol].rule.first,
                                         std::get<1>(last),
                                         grammar[last_symbol].rule.second});
                    } else {
                        path1.push_back(
                            {grammar[last_symbol].rule.first, std::get<1>(last),
                             grammar[last_symbol].rule.second == -1 ? 1 : -1});
                    }
                }

                steps_up = 0;
                if (std::get<2>(path2.back()) > 1) {
                    path2.pop_back();
                    steps_up++;
                }
                while (!path2.empty() && std::get<2>(path2.back()) == 1) {
                    path2.pop_back();
                    steps_up++;
                }
                if (path2.empty()) {
                    return std::get<1>(path1.back());
                }
                last = path2.back();
                last_symbol = std::get<0>(last);
                path2.pop_back();
                parent = path2.back();
                parent_symbol = std::get<0>(parent);
                if (is_rle(parent_symbol)) {
                    path2.push_back(
                        {last_symbol,
                         std::get<1>(last) + get_length(last_symbol),
                         std::get<2>(last) - 1});
                } else {
                    path2.push_back(
                        {grammar[parent_symbol].rule.second,
                         std::get<1>(parent) +
                             get_length(grammar[parent_symbol].rule.first),
                         1});
                }
                for (int i = 0; i < steps_up; i++) {
                    last = path2.back();
                    last_symbol = std::get<0>(last);
                    if (is_rle(last_symbol)) {
                        path2.push_back({grammar[last_symbol].rule.first,
                                         std::get<1>(last),
                                         grammar[last_symbol].rule.second});
                    } else {
                        path2.push_back(
                            {grammar[last_symbol].rule.first, std::get<1>(last),
                             grammar[last_symbol].rule.second == -1 ? 1 : -1});
                    }
                }
            }
        }

        label1 = std::get<0>(path1.back());
        label2 = std::get<0>(path2.back());

        assert(std::get<1>(path1.back()) == std::get<1>(path2.back()));

        if (get_level(label1) == 0) {
            break;
        }

        if (is_rle(label1)) {
            path1.push_back({grammar[label1].rule.first,
                             std::get<1>(path1.back()),
                             grammar[label1].rule.second});
        } else {
            path1.push_back({grammar[label1].rule.first,
                             std::get<1>(path1.back()),
                             grammar[label1].rule.second == -1 ? 1 : -1});
        }
        label1 = grammar[label1].rule.first;

        if (is_rle(label2)) {
            path2.push_back({grammar[label2].rule.first,
                             std::get<1>(path2.back()),
                             grammar[label2].rule.second});
        } else {
            path2.push_back({grammar[label2].rule.first,
                             std::get<1>(path2.back()),
                             grammar[label2].rule.second == -1 ? 1 : -1});
        }
        label2 = grammar[label2].rule.first;
    }
    return std::get<1>(path1.back());
}

bool parsings::smaller(int label1, int label2) {
    int lcp = longest_common_prefix(label1, label2);
    assert(0 <= lcp && lcp <= std::min(get_length(label1), get_length(label2)));
    if (lcp == get_length(label2)) {
        return false;
    }
    if (lcp == get_length(label1)) {
        return true;
    }
    return get_kth_character(label1, lcp + 1) <
           get_kth_character(label2, lcp + 1);
}