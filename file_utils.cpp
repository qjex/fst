//
// Created by Andrew Berlin on 10/22/18.
//

#include <QFile>
#include <QCryptographicHash>
#include <QTextStream>
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
            auto buf_trigrams = get_trigrams(buffer);
            if (buf_trigrams.empty()) {
                result.clear();
                break;
            }
            for (const auto &t : buf_trigrams) {
                result.insert(t);
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

std::unordered_set<hash_t> get_trigrams(QString const &s) {
    std::unordered_set<hash_t> result;
    for (int i = 0; i < s.size() - 2; ++i) {
        hash_t hash = 0;
        for (int j = 0; j < 3; ++j) {
            ushort c = s[i + j].unicode();
            if (c == 0) {
                result.clear();
                return result;
            }
            hash = (hash << 16);
            hash += c;
        }
        result.insert(hash);
    }
    return result;
}