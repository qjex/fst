//
// Created by Andrew Berlin on 12/15/18.
//

#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include "searching_worker.h"
#include "QDebug"

searching_worker::searching_worker(const std::vector<QString> &files,
                                   const QString &pattern,
                                   std::unordered_map<QString, std::vector<hash_t>> &index,
                                   std::mutex &index_mutex)
    : files(files),
      pattern(pattern),
      index(index),
      index_mutex(index_mutex) {
}

void searching_worker::run() {
    std::unordered_set<hash_t> trigrams;
    get_trigrams(pattern, trigrams);
    for (auto &file : files) {
        if (!contains_all(file, trigrams)) {
            return;
        }
        if (stop_requested) {
            return;
        }
        if (contains(file)) {
            emit send_result(file);
        }
    }
}

void searching_worker::stop() {
    stop_requested = true;
}

bool searching_worker::contains_all(const QString &path, const std::unordered_set<hash_t> &trigrams) {
    std::lock_guard<std::mutex> g(index_mutex);
    for (auto &x : trigrams) {
        if (stop_requested) {
            return false;
        }
        auto it = lower_bound(index[path].begin(), index[path].end(), x);
        if (it == index[path].end() || (*it) != x) {
            return false;
        }
    }
    return true;
}

bool searching_worker::contains(const QString &path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString buffer;
        while (buffer.append(stream.read(BUFFER_SZ)).size() >= pattern.size()) {
            for (int i = 0; i < BUFFER_SZ - pattern.size() + 1; i++) {
                int j = 0;
                while (pattern[j] == buffer[i + j] && j < pattern.size()) {
                    j++;
                }
                if (j == pattern.size()) {
                    file.close();
                    return true;
                }
            }
            buffer = buffer.mid(buffer.size() - pattern.size() + 1, pattern.size() - 1);
        }
    }
    file.close();
    return false;
}
