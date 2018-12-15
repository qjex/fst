//
// Created by Andrew Berlin on 10/22/18.
//

#include <QFile>
#include <QCryptographicHash>
#include <QTextStream>
#include "file_utils.h"
#include "QDebug"

const int MAX_RES_SZ = 200000;

std::unordered_set<hash_t> get_file_trigrams(QString const &path) {
    std::unordered_set<hash_t> result;
    QFile file(path);
    int64_t all = 0;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buffer;
        int64_t blocks = 0;
        while (true) {
            buffer.append(stream.read(BUFFER_SZ));
            if (buffer.size() < 3) {
                break;
            }
            all += buffer.size();
            for (const auto &t : get_trigrams(buffer)) {
                result.insert(t);
            }
            if (result.size() >= (blocks * BUFFER_SZ + buffer.size()) * 70 / 100) {
                return std::unordered_set<hash_t>{};
            }
            buffer = buffer.mid(buffer.size() - 2, 2);
            blocks++;
        }
        file.close();
    }
    qDebug() << path << ' ' << result.size() << ' ' << all << ' ' << 1.0 * result.size() / all;
    if (result.size() > MAX_RES_SZ) {
        result.clear();
    }
    return result;

}

std::unordered_set<hash_t> get_trigrams(QString const &s) {
    std::unordered_set<hash_t> result;
    for (int i = 0; i < s.size() - 2; ++i) {
        hash_t hash = 0;
        for (int j = 0; j < 3; ++j) {
            hash = (hash << 16);
            hash += s[i + j].unicode();
        }
        result.insert(hash);
    }
    return result;
}