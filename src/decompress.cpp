#include "archiver.h"

#include <fstream>

int GetInt(std::string& s) {
    while (s.size() < BLOCK) {
        s += '0';
    }
    int c = 0;
    for (int i = 0; i < BLOCK; i++) {
        c += ((s[i] - '0') << i);
    }
    s.erase(0, BLOCK);
    return c;
}

std::string GetStringByte(int c) {
    std::string a;
    for (int i = 0; i < BYTE; i++) {
        a += static_cast<char>((c & 1) + '0');
        c >>= 1;
    }
    return a;
}
bool DecompressFile(std::ifstream& ifs, std::string& outputfile) {
    std::string buffer;
    char ch = 0;
    while (buffer.size() < BLOCK && ifs.get(ch)) {
        buffer += GetStringByte(ch);
    }

    const int alph = GetInt(buffer);
    std::vector<int> letter(alph);
    std::vector<int> used_len;
    int sum = 0;
    for (int i = 0; i < alph; i++) {
        while (buffer.size() < BLOCK && ifs.get(ch)) {
            buffer += GetStringByte(ch);
        }
        letter[i] = GetInt(buffer);
    }

    while (sum < alph) {
        while (buffer.size() < BLOCK && ifs.get(ch)) {
            buffer += GetStringByte(ch);
        }
        used_len.push_back(GetInt(buffer));
        sum += used_len.back();
    }

    Node* s = new Node(-1, 0);
    Node* now = s;
    int code = -1;
    int lastlen = 0;
    int id = 0;
    int len = 1;
    for (auto cnt_len : used_len) {
        while (cnt_len) {
            code = ((code + 1) << (len - lastlen));
            std::string t = GetCode(code, len);
            for (auto c : t) {
                if (c == '0') {
                    if (now->left == nullptr) {
                        now->left = new Node(-1, 0);
                    }
                    now = (now->left);
                } else {
                    if (now->right == nullptr) {
                        now->right = new Node(-1, 0);
                    }
                    now = (now->right);
                }
            }
            now->ch = letter[id++];
            now = s;
            lastlen = len;
            cnt_len--;
        }
        len++;
    }

    bool what_name = false;
    while (ifs.get(ch)) {
        buffer += GetStringByte(ch);
        int i = 0;
        for (auto c : buffer) {
            i++;
            if (c == '0') {
                now = now->left;
            } else {
                now = now->right;
            }
            if (now->ch != -1) {
                if (now->ch == FILENAME_END) {
                    what_name = true;
                    break;
                } else {
                    outputfile += static_cast<char>(now->ch);
                }
                now = s;
            }
        }
        if (what_name) {
            buffer.erase(0, i);
            break;
        }
        buffer.clear();
    }

    std::ofstream ofs(outputfile, std::ios::binary);
    now = s;
    while (ifs.get(ch)) {
        buffer += GetStringByte(ch);
        int i = 0;
        for (auto c : buffer) {
            i++;
            if (c == '0') {
                now = now->left;
            } else {
                now = now->right;
            }
            if (now->ch != -1) {
                if (ONE_MORE_FILE == now->ch) {
                    buffer.erase(0, i);
                    ofs.close();
                    delete s;
                    return true;
                } else if (ARCHIVE_END == now->ch) {
                    buffer.clear();
                    ofs.close();
                    delete s;
                    return false;
                } else {
                    ofs.put(static_cast<char>(now->ch));
                }
                now = s;
            }
        }
        buffer.clear();
    }
    ofs.close();
    return false;
}