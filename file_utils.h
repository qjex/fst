//
// Created by Andrew Berlin on 10/22/18.
//

#ifndef DFF_DFF_UTILS_H
#define DFF_DFF_UTILS_H

#include <QString>
#include <unordered_set>

namespace std {
template<>
struct hash<QString> {
    std::size_t operator()(const QString &s) const {
        return qHash(s);
    }
};
}

typedef uint64_t hash_t;

const int BUFFER_SZ = 4 * 1024;

std::vector<hash_t> get_file_trigrams(QString const &path);
void get_trigrams(QString const &s, std::unordered_set<hash_t> &result);
hash_t get_hash(const std::array<ushort, 3> &tmp);
#endif //DFF_DFF_UTILS_H
