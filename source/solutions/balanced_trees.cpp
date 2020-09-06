#include "balanced_trees.h"

balanced_trees::treap *
balanced_trees::get_fresh_treap(balanced_trees::treap *t) {
    assert(treap_factory_index < MAX_PERMISSIBLE_OBJECTS);
    treap_factory[treap_factory_index].size = t->size;
    treap_factory[treap_factory_index].base_power = t->base_power;
    treap_factory[treap_factory_index].character = t->character;
    treap_factory[treap_factory_index].hash = t->hash;
    treap_factory[treap_factory_index].left_subtree = t->left_subtree;
    treap_factory[treap_factory_index].leftmost_character =
        t->leftmost_character;
    treap_factory[treap_factory_index].priority = t->priority;
    treap_factory[treap_factory_index].right_subtree = t->right_subtree;
    return &treap_factory[treap_factory_index++];
}

balanced_trees::treap *balanced_trees::get_fresh_treap(
    long long priority, int character, int leftmost_character, int size,
    int hash, int base_power, treap *left_subtree, treap *right_subtree) {
    assert(treap_factory_index < MAX_PERMISSIBLE_OBJECTS);
    treap_factory[treap_factory_index].size = size;
    treap_factory[treap_factory_index].base_power = base_power;
    treap_factory[treap_factory_index].character = character;
    treap_factory[treap_factory_index].hash = hash;
    treap_factory[treap_factory_index].left_subtree = left_subtree;
    treap_factory[treap_factory_index].leftmost_character = leftmost_character;
    treap_factory[treap_factory_index].priority = priority;
    treap_factory[treap_factory_index].right_subtree = right_subtree;
    return &treap_factory[treap_factory_index++];
}

long long balanced_trees::random_long_long() {
    return std::uniform_int_distribution<long long>()(random_number_generator);
}

int balanced_trees::get_size(balanced_trees::treap *t) {
    return t ? t->size : 0;
}

int balanced_trees::get_hash(balanced_trees::treap *t) {
    return t ? t->hash : 0;
}

int balanced_trees::get_base_power(balanced_trees::treap *t) {
    return t ? t->base_power : 1;
}

void balanced_trees::update_values(balanced_trees::treap *t) {
    t->size = get_size(t->left_subtree) + get_size(t->right_subtree) + 1;
    t->hash = (get_hash(t->left_subtree) +
               (long long)get_base_power(t->left_subtree) * t->character +
               (long long)get_base_power(t->left_subtree) * base % modulo *
                   (long long)get_hash(t->right_subtree)) %
              modulo;
    t->base_power = (long long)get_base_power(t->left_subtree) *
                    get_base_power(t->right_subtree) % modulo *
                    (long long)base % modulo;
    t->leftmost_character =
        t->left_subtree ? t->left_subtree->leftmost_character : t->character;
}

balanced_trees::treap *balanced_trees::create(int character) {
    return get_fresh_treap(random_long_long(), character, character, 1,
                           character, base, nullptr, nullptr);
}

balanced_trees::treap *balanced_trees::merge(balanced_trees::treap *t1,
                                             balanced_trees::treap *t2) {
    if (t1 == nullptr) {
        return t2;
    }
    if (t2 == nullptr) {
        return t1;
    }
    if (t1->priority > t2->priority) {
        // auto return_treap = new treap(*t1);
        auto return_treap = get_fresh_treap(t1);
        return_treap->right_subtree = merge(t1->right_subtree, t2);
        update_values(return_treap);
        return return_treap;
    } else {
        // auto return_treap = new treap(*t2);
        auto return_treap = get_fresh_treap(t2);
        return_treap->left_subtree = merge(t1, t2->left_subtree);
        update_values(return_treap);
        return return_treap;
    }
}

std::pair<balanced_trees::treap *, balanced_trees::treap *>
balanced_trees::split(balanced_trees::treap *t, int left_tree_size) {
    if (t == nullptr) {
        return std::pair<treap *, treap *>(nullptr, nullptr);
    }
    if (left_tree_size <= get_size(t->left_subtree)) {
        auto left_subtree_split = split(t->left_subtree, left_tree_size);
        // auto return_treap = new treap(*t);
        auto return_treap = get_fresh_treap(t);
        return_treap->left_subtree = left_subtree_split.second;
        update_values(return_treap);
        return {left_subtree_split.first, return_treap};
    } else {
        auto right_subtree_split = split(
            t->right_subtree, left_tree_size - 1 - get_size(t->left_subtree));
        // auto return_treap = new treap(*t);
        auto return_treap = get_fresh_treap(t);
        return_treap->right_subtree = right_subtree_split.first;
        update_values(return_treap);
        return {return_treap, right_subtree_split.second};
    }
}

void balanced_trees::print_treap(balanced_trees::treap *t) {
    if (t == nullptr) {
        return;
    }
    print_treap(t->left_subtree);
    std::cout << t->character << " ";
    print_treap(t->right_subtree);
}

balanced_trees::treap *balanced_trees::create(std::vector<int> &word) {
    std::vector<treap *> current_level;
    for (auto character : word) {
        current_level.push_back(create(character));
    }
    while (current_level.size() > 1) {
        std::vector<treap *> next_level;
        for (size_t i = 0; i + 1 < current_level.size(); i += 2) {
            next_level.push_back(merge(current_level[i], current_level[i + 1]));
        }
        if (current_level.size() % 2 == 1) {
            next_level.push_back(current_level.back());
        }
        current_level = next_level;
    }
    treap *ret = current_level[0];
    return ret;
}

balanced_trees::balanced_trees(int seed, int _base, int _modulo) {
    random_number_generator.seed(seed);
    base = _base;
    modulo = _modulo;
    treap_factory_index = 0;
    treap_factory = new treap[MAX_PERMISSIBLE_OBJECTS];
}

balanced_trees::~balanced_trees() {
    stream.clear();
    stream.shrink_to_fit();
    // treap_factory.clear();
    // treap_factory.shrink_to_fit();
    delete[] treap_factory;
}

int balanced_trees::make_string(std::vector<int> &word) {
    stream.push_back(create(word));
    return stream.size() - 1;
}

int balanced_trees::concat(int label1, int label2) {
    stream.push_back(merge(stream[label1], stream[label2]));
    return stream.size() - 1;
}

std::pair<int, int> balanced_trees::split(int label, int position) {
    auto split_result = split(stream[label], position);
    stream.push_back(split_result.first);
    stream.push_back(split_result.second);
    return {stream.size() - 2, stream.size() - 1};
}

bool balanced_trees::equals(int label1, int label2) {
    return get_size(stream[label1]) == get_size(stream[label2]) &&
           get_hash(stream[label1]) == get_hash(stream[label2]);
}

int balanced_trees::longest_common_prefix(int label1, int label2) {
    int max_possible_lcp =
        std::min(get_size(stream[label1]), get_size(stream[label2]));
    // binary search the answer
    int left_pointer = 0, right_pointer = max_possible_lcp + 1;
    while (left_pointer + 1 < right_pointer) {
        int guess = (left_pointer + right_pointer) / 2;
        auto treap1 = split(stream[label1], guess).first;
        auto treap2 = split(stream[label2], guess).first;
        if (get_hash(treap1) == get_hash(treap2)) {
            left_pointer = guess;
        } else {
            right_pointer = guess;
        }
    }
    return left_pointer;
}

bool balanced_trees::smaller(int label1, int label2) {
    int lcp = longest_common_prefix(label1, label2);
    auto treap1 = split(stream[label1], lcp).second;
    auto treap2 = split(stream[label2], lcp).second;
    if (treap2 == nullptr) {
        return false;
    }
    if (treap1 == nullptr) {
        return true;
    }
    return treap1->leftmost_character < treap2->leftmost_character;
}