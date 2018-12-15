//
// Created by Andrew Berlin on 12/12/18.
//

#ifndef FST_INDEXING_WORKER_H
#define FST_INDEXING_WORKER_H

#include <QRunnable>
#include <QString>
#include <QDebug>
#include <mutex>
#include <file_utils.h>
#include <unordered_map>

struct indexing_worker : public QObject, public QRunnable {
Q_OBJECT
public:
    indexing_worker(const QString &file_path,
                        std::unordered_map<QString, std::unordered_set<hash_t>> &index,
                        std::mutex &index_mutex);
    void run() override;
signals:
    void send_status(bool);
private:
    QString file_path;
    std::mutex &index_mutex;
    std::unordered_map<QString, std::unordered_set<hash_t>> &index;
};

#endif //FST_INDEXING_WORKER_H
