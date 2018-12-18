//
// Created by Andrew Berlin on 12/12/18.
//

#include "indexing_worker.h"
#include "QThreadPool"

indexing_worker::indexing_worker(const QString &file_path,
                                 std::unordered_map<QString, std::unordered_set<hash_t>> &index,
                                 std::mutex &index_mutex)
    : file_path(file_path), index(index), index_mutex(index_mutex) {

}

void indexing_worker::run() {
    emit send_status(true);
    auto res = get_file_trigrams(file_path);
    std::lock_guard<std::mutex> lock(index_mutex);
    index[file_path] = res;
    emit send_status(false);
}
