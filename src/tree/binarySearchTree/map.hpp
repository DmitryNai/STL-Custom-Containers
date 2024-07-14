#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

#include "exceptions.hpp"

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
public:
    Map() : root_(nullptr) {
    }

    Value& operator[](const Key& key) {
        return FindOrInsert(root_, key)->value;
    }

    bool IsEmpty() const noexcept {
        return !root_;
    }

    size_t Size() const noexcept {
        return Size(root_);
    }

    void Swap(Map& a) {
        std::swap(root_, a.root_);
        std::swap(comp_, a.comp_);
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> values;
        InOrderTraversal(root_, values, is_increase);
        return values;
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (const auto& val : values) {
            Insert(val);
        }
    }

    void Insert(const std::pair<const Key, Value>& val) {
        root_ = Insert(root_, val);
    }

    void Erase(const Key& key) {
        root_ = Erase(root_, key);
    }

    void Clear() noexcept {
        Clear(root_);
        root_ = nullptr;
    }

    bool Find(const Key& key) const {
        return Find(root_, key);
    }

    ~Map() {
        Clear();
    }

private:
    struct Node {
        Node(const Key& k, const Value& v) : key(k), value(v), left(nullptr), right(nullptr) {
        }

        Key key;
        Value value;
        Node* left;
        Node* right;
    };

    Node* root_;
    Compare comp_;

    Node* FindOrInsert(Node*& node, const Key& key) {
        if (!node) {
            node = new Node(key, Value{});
            return node;
        }

        if (comp_(key, node->key)) {
            return FindOrInsert(node->left, key);
        } else if (comp_(node->key, key)) {
            return FindOrInsert(node->right, key);
        } else {
            return node;
        }
    }

    size_t Size(Node* node) const noexcept {
        if (!node) {
            return 0;
        }
        return 1 + Size(node->left) + Size(node->right);
    }

    void InOrderTraversal(Node* node, std::vector<std::pair<const Key, Value>>& values,
                          bool is_increase) const noexcept {
        if (!node) {
            return;
        }
        if (is_increase) {
            InOrderTraversal(node->left, values, is_increase);
            values.push_back({node->key, node->value});
            InOrderTraversal(node->right, values, is_increase);
        } else {
            InOrderTraversal(node->right, values, is_increase);
            values.push_back({node->key, node->value});
            InOrderTraversal(node->left, values, is_increase);
        }
    }

    Node* Insert(Node* node, const std::pair<const Key, Value>& val) {
        if (!node) {
            return new Node(val.first, val.second);
        }

        if (comp_(val.first, node->key)) {
            node->left = Insert(node->left, val);
        } else if (comp_(node->key, val.first)) {
            node->right = Insert(node->right, val);
        } else {
            node->value = val.second;
        }

        return node;
    }

    Node* Erase(Node* node, const Key& key) {
        if (!node) {
            throw MapIsEmptyException("Error");
        }

        if (comp_(key, node->key)) {
            node->left = Erase(node->left, key);
        } else if (comp_(node->key, key)) {
            node->right = Erase(node->right, key);
        } else {
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            } else if (!node->left) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                Node* temp = node->left;
                delete node;
                return temp;
            } else {
                Node* successor = Minimum(node->right);
                node->key = successor->key;
                node->value = successor->value;
                node->right = Erase(node->right, successor->key);
            }
        }

        return node;
    }

    Node* Minimum(Node* node) const noexcept {
        while (node->left) {
            node = node->left;
        }
        return node;
    }

    void Clear(Node* node) noexcept {
        if (!node) {
            return;
        }
        Clear(node->left);
        Clear(node->right);
        delete node;
    }

    bool Find(Node* node, const Key& key) const {
        while (node) {
            if (comp_(key, node->key)) {
                node = node->left;
            } else if (comp_(node->key, key)) {
                node = node->right;
            } else {
                return true;
            }
        }
        return false;
    }
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std