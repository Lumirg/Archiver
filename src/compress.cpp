#include "archiver.h"

#include <fstream>
#include <iostream>
#include <algorithm>

std::string GetCode(int c, int len) {
    std::string ans;
    while (c) {
        ans += static_cast<char>((c & 1) + '0');
        len--;
        c >>= 1;
    }
    while (len) {
        ans += '0';
        len--;
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}

void ClearBuffer(std::string& buffer, std::ofstream& ofs) {
    int byte = 0;
    while (buffer.size() >= BYTE) {
        byte = 0;
        for (int i = 0; i < BYTE; i++) {
            byte += ((buffer[i] - '0') << i);
        }
        ofs.put(static_cast<char>(byte));
        buffer.erase(0, BYTE);
    }
}

std::string GetStringBlock(int c) {
    std::string a;
    for (int i = 0; i < BLOCK; i++) {
        a += static_cast<char>((c & 1) + '0');
        c >>= 1;
    }
    return a;
}

void CompressFile(const std::string& inputfile, std::string& outputfile, int cnt_file, std::ofstream& ofs) {
    std::ifstream ifs(inputfile, std::ios::binary);

    std::unordered_map<int, int> freqmap;

    std::string name_file;
    for (auto c : inputfile) {
        freqmap[static_cast<unsigned char>(c)]++;
        name_file += c;
        if (c == '/') {
            freqmap.clear();
            name_file.clear();
        }
    }

    freqmap[FILENAME_END]++;
    freqmap[ONE_MORE_FILE]++;
    freqmap[ARCHIVE_END]++;
    char ch = 0;
    while (ifs.get(ch)) {
        freqmap[static_cast<unsigned char>(ch)]++;
    }

    Node* root = BuildHuffmanTree(freqmap);
    std::vector<std::pair<int, int>> huffman_len_code;
    GenerateHuffmanCodes(root, 0, huffman_len_code);
    std::sort(huffman_len_code.begin(), huffman_len_code.end());
    delete root;

    std::unordered_map<int, std::string> huffman_code;
    int code = -1;
    int lastlen = 0;
    for (auto [len, c] : huffman_len_code) {
        code = ((code + 1) << (len - lastlen));
        huffman_code[c] = GetCode(code, len);
        lastlen = len;
    }

    ifs.clear();
    ifs.seekg(0);
    std::string buffer = GetStringBlock(static_cast<int>(huffman_len_code.size()));
    for (auto [len, c] : huffman_len_code) {
        buffer += GetStringBlock(c);
    }

    int nowlen = 1;
    int cntnowlen = 0;
    for (auto [len, c] : huffman_len_code) {
        while (len != nowlen) {
            buffer += GetStringBlock(cntnowlen);
            cntnowlen = 0;
            nowlen++;
        }
        cntnowlen++;
    }
    if (cntnowlen != 0) {
        buffer += GetStringBlock(cntnowlen);
    }
    ClearBuffer(buffer, ofs);

    for (auto c : name_file) {
        buffer += huffman_code[static_cast<unsigned char>(c)];
        if (buffer.size() > BLOCK * BYTE) {
            ClearBuffer(buffer, ofs);
        }
    }
    buffer += huffman_code[FILENAME_END];

    while (ifs.get(ch)) {
        buffer += huffman_code[static_cast<unsigned char>(ch)];
        if (buffer.size() > BLOCK * BYTE) {
            ClearBuffer(buffer, ofs);
        }
    }

    if (cnt_file) {
        buffer += huffman_code[ONE_MORE_FILE];
    } else {
        buffer += huffman_code[ARCHIVE_END];
    }

    ClearBuffer(buffer, ofs);
    if (!buffer.empty()) {
        int byte = 0;
        for (size_t i = 0; i < buffer.size(); i++) {
            byte += ((buffer[i] - '0') << i);
        }
        ofs.put(static_cast<char>(byte));
        buffer.erase(0, BYTE);
    }
    ifs.close();
}