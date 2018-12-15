#include <QFileInfo>
#include <QDir>
#include <QThreadPool>
#include "watcher.h"
#include "QDebug"
#include "indexing_worker.h"
#include "mainwindow.h"
#include "searching_worker.h"

watcher::watcher(main_window *mw) : mw(mw) {
    connect(&fs_watcher, &QFileSystemWatcher::fileChanged, this, &watcher::index_file);
}

void watcher::index_file(QString const &path) {
    if (shutdown) {
        return;
    }
    auto worker = new indexing_worker(path, index, index_mutex);
    connect(worker, &indexing_worker::send_status, mw, &main_window::set_indexing_status);
    indexing_pool.start(worker);
}

bool watcher::update_watch_files(QString const &path, bool add) {
    QDir monitoredFolder(path);
    QFileInfoList watchedList = monitoredFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

    QListIterator<QFileInfo> iterator(watchedList);
    bool res = true;
    while (iterator.hasNext()) {
        auto file = iterator.next().filePath();
        bool prev_res = (add ? fs_watcher.addPath(file) : fs_watcher.removePath(file));
        if (add && prev_res) {
            index_file(file);
        }
        res &= prev_res;
    }
    return res;
}

void watcher::search_text(QString const &text) {
    searching_pool.clear();
    emit stop_searching();
    auto pt = get_trigrams(text);
    std::lock_guard<std::mutex> guard(index_mutex);
    for (auto &i : index) {
        auto worker = new searching_worker(i.first, text, pt, i.second);
        connect(worker, &searching_worker::send_result, mw, &main_window::add_result_item);
        connect(this, &watcher::stop_searching, worker, &searching_worker::stop);
        indexing_pool.start(worker);
    }
}

watcher::~watcher() {
    shutdown_pools();
}

void watcher::shutdown_pools() {
    shutdown = true;
    indexing_pool.clear();
    searching_pool.clear();
    indexing_pool.waitForDone();
    searching_pool.waitForDone();
}

