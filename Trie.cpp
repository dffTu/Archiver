#include "Trie.h"

Trie::Node::Node(int32_t val) {
    ord = val;
}

Trie::Node::~Node() {
    ord = -1;
    delete left;
    delete right;
}

Trie::Trie() : root_() {
    current_node_ = &root_;
}

void Trie::AddToTrie(std::vector<bool>& code, const size_t& val) {
    Node* node = &root_;
    for (const auto x : code) {
        if (x) {
            if (!node->right) {
                node->right = new Node(-1);
            }
            node = node->right;
        } else {
            if (!node->left) {
                node->left = new Node(-1);
            }
            node = node->left;
        }
    }
    node->ord = val;
}

void Trie::ResetCurrentNode() {
    current_node_ = &root_;
}

int32_t Trie::GetCurrentNodeValue() const {
    return current_node_->ord;
}

void Trie::MoveLeft() {
    if (current_node_ == nullptr) {
        throw std::runtime_error("Invalid file format (wrong coding)");
    }
    current_node_ = current_node_->left;
}

void Trie::MoveRight() {
    if (current_node_ == nullptr) {
        throw std::runtime_error("Invalid file format (wrong coding)");
    }
    current_node_ = current_node_->right;
}