//
// Created by Andrew Berlin on 12/15/18.
//

#ifndef FST_SEARCHING_WORKER_H
#define FST_SEARCHING_WORKER_H

#include <QObject>
#include <QRunnable>
#include <unordered_map>
#include <mutex>
#include "file_utils.h"

struct searching_worker : public QObject, public QRunnable {
Q_OBJECT
public:
    searching_worker(const QString &file_path,
                     const QString &pattern,
                     const std::unordered_set<hash_t> &pattern_trigrams,
                     std::unordered_map<QString, std::unordered_set<hash_t>> &index,
                     std::mutex &index_mutex);
    void run() override;
private:
    bool contains_all();
    bool contains();
signals:
    void send_result(const QString &);
public slots:
    void stop();
private:
    QString file_path;
    QString pattern;
    std::unordered_set<hash_t> pattern_trigrams;
    bool stop_requested = false;
    std::mutex &index_mutex;
    std::unordered_map<QString, std::unordered_set<hash_t>> &index;
};

#endif //FST_SEARCHING_WORKER_H
