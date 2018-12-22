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
    searching_worker(const std::vector<QString> &files,
                     const QString &pattern,
                     std::unordered_map<QString, std::vector<hash_t>> &index,
                     std::mutex &index_mutex);
    void run() override;
private:
    bool contains_all(const QString&, const std::unordered_set<hash_t> &);
    bool contains(const QString&);
signals:
    void send_result(const QString &);
public slots:
    void stop();
private:
    std::vector<QString> files;
    QString pattern;
    bool stop_requested = false;
    std::mutex &index_mutex;
    std::unordered_map<QString, std::vector<hash_t>> &index;
};

#endif //FST_SEARCHING_WORKER_H
