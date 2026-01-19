#include "archiver.h"

Node* BuildHuffmanTree(const std::unordered_map<int, int>& freqmap) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (const auto& pair : freqmap) {
        pq.push(new Node(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();
        Node* parent = new Node(-1, left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

void GenerateHuffmanCodes(Node* root, int len, std::vector<std::pair<int, int>>& huffman_len_code) {
    if (!root) {
        return;
    }
    if (!root->left && !root->right) {
        huffman_len_code.push_back({len, root->ch});
    }
    GenerateHuffmanCodes(root->left, len + 1, huffman_len_code);
    GenerateHuffmanCodes(root->right, len + 1, huffman_len_code);
}
