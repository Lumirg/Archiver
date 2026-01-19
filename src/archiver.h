#pragma once

#include <unordered_map>
#include <queue>
#include <string>

const int BLOCK = 9;
const int BYTE = 8;
const int BAD_END = 111;
const int FILENAME_END = 256;
const int ONE_MORE_FILE = 257;
const int ARCHIVE_END = 258;

struct Node {
    int ch;
    int freq;
    Node* left;
    Node* right;
    Node(int ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {
    }
    ~Node() {
        delete left;
        delete right;
    }
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        if (l->freq == r->freq) {
            return l->ch > r->ch;
        }
        return l->freq > r->freq;
    }
};

std::string GetCode(int c, int len);

Node* BuildHuffmanTree(const std::unordered_map<int, int>& freq_map);

void GenerateHuffmanCodes(Node* root, int len, std::vector<std::pair<int, int>>& huffman_len_code);

void CompressFile(const std::string& inputfile, std::string& outputfile, int cnt_file, std::ofstream& ofs);

bool DecompressFile(std::ifstream& ifs, std::string& outputfile);
