//
// Created by Andrew Berlin on 10/22/18.
//

#include <QFile>
#include <QCryptographicHash>
#include <QTextStream>
#include <array>
#include "file_utils.h"
#include "QDebug"
#include "unordered_set"

const int MAX_RES_SZ = 20000;
const int READ_BUFFER_SZ = 4 * 1024 * 1024;


std::vector<hash_t> get_file_trigrams(QString const &path) {
    std::unordered_set<hash_t> result;
    QFile file(path);
    int64_t all = 0;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buffer;
        while (true) {
            buffer.append(stream.read(READ_BUFFER_SZ));
            if (buffer.size() < 3) {
                break;
            }
            all += buffer.size();
            get_trigrams(buffer, result);
            if (result.empty()) {
                break;
            }
            buffer = buffer.mid(buffer.size() - 2, 2);
        }
        file.close();
    }
    if (result.size() >= MAX_RES_SZ) {
        result.clear();
    }
    if (!result.empty()) {
        qDebug() << path << ' ' << result.size() << ' ' << all;
    }
    std::vector<hash_t> v(result.begin(), result.end());
    std::sort(v.begin(), v.end());
    return v;

}

void get_trigrams(QString const &s, std::unordered_set<hash_t> &result) {
    std::array<ushort, 3> tmp = {s[0].unicode(), s[1].unicode(), s[2].unicode()};
    result.insert(get_hash(tmp));
    for (int i = 3; i < s.size(); ++i) {
        tmp[0] = tmp[1];
        tmp[1] = tmp[2];
        tmp[2] = s[i].unicode();
        result.insert(get_hash(tmp));
    }
}

hash_t get_hash(const std::array<ushort, 3> &tmp) {
    hash_t hash = 0;
    for (size_t i = 0; i < 3; i++) {
        hash = (hash << 16);
        hash += tmp[i];
    }
    return hash;
}