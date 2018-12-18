#ifndef WATCHER_H
#define WATCHER_H

#include <QObject>
#include <QThread>
#include <unordered_map>
#include <unordered_set>
#include <file_utils.h>
#include <QFileSystemWatcher>
#include <QThreadPool>

class main_window;

class watcher : public QThread {
Q_OBJECT
public:
    watcher(main_window *);
    ~watcher();
public slots:
    void index_file(QString const &);
    bool update_watch_files(QString const &path, bool add);
    void search_text(QString const &text);
    void shutdown_pools();
signals:
    void stop_searching();
private:
    std::unordered_map<QString, std::unordered_set<hash_t>> index;
    std::mutex index_mutex;
    QFileSystemWatcher fs_watcher;
    main_window *mw;
    QThreadPool indexing_pool;
    QThreadPool searching_pool;
    bool shutdown = false;
};

#endif // WATCHER_H
