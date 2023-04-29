#pragma once

#include <vector>
#include <stdexcept>

class Trie {
private:
    class Node {
    public:
        int32_t ord = -1;
        Node* left = nullptr;
        Node* right = nullptr;

        Node() = default;

        explicit Node(int32_t val);

        Node(Node& other) = delete;

        Node& operator=(Node& other) = delete;

        ~Node();
    };

    Node root_;
    Node* current_node_;

public:
    Trie();

    void AddToTrie(std::vector<bool>& code, const size_t& val);

    void ResetCurrentNode();

    int32_t GetCurrentNodeValue() const;

    void MoveLeft();

    void MoveRight();
};