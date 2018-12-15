//
// Created by Andrew Berlin on 12/15/18.
//

#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include "searching_worker.h"

searching_worker::searching_worker(const QString &file_path,
                                   const QString &pattern,
                                   const std::unordered_set<hash_t> &pattern_trigrams,
                                   std::unordered_map<QString, std::unordered_set<hash_t>> &index,
                                   std::mutex &index_mutex)
    : file_path(file_path),
      pattern(pattern),
      pattern_trigrams(pattern_trigrams),
      index(index),
      index_mutex(index_mutex) {
}

void searching_worker::run() {
    if (!contains_all()) {
        return;
    }
    if (stop_requested) {
        return;;
    }
    if (contains()) {
        emit send_result(file_path);
    }
}

void searching_worker::stop() {
    stop_requested = true;
}

bool searching_worker::contains_all() {
    std::lock_guard<std::mutex> g(index_mutex);
    for (auto &x : index[file_path]) {
        if (stop_requested) {
            return false;
        }
        if (!index[file_path].count(x)) {
            return false;
        }
    }
    return true;
}

bool searching_worker::contains() {
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString buffer;
        while (buffer.append(stream.read(BUFFER_SZ)).size() >= pattern.size()) {
            for (int i = 0; i <= BUFFER_SZ - pattern.size(); i++) {
                int j = 0;
                for (; pattern[j] == buffer[i + j] && j < pattern.size(); j++);
                if (j == pattern.size()) {
                    return true;
                }
            }
            buffer = buffer.mid(buffer.size() - pattern.size() + 1, pattern.size() - 1);
        }
    }
    file.close();

    return false;
}
