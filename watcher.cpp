#include <QFileInfo>
#include <QDir>
#include <QThreadPool>
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>
#include "watcher.h"
#include "mainwindow.h"
#include "searching_worker.h"

watcher::watcher(main_window *mw) : mw(mw) {
    connect(&fs_watcher, &QFileSystemWatcher::fileChanged, this, &watcher::index_file);
}

QFuture<void> watcher::index_file(QString const &path) {
    if (shutdown) {
        return QFuture<void>();
    }

    return QtConcurrent::run(&indexing_pool, [this, path]() {
        auto trigrams = get_file_trigrams(path);
        std::lock_guard<std::mutex> lock(index_mutex);
        index[path] = trigrams;
    });
}

bool watcher::update_watch_files(QString const &path, bool add) {
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    emit send_indexing_status(true);
    bool res = true;
    std::vector<QFuture<void>> futures;
    while (it.hasNext()) {
        auto file = it.next();
        bool prev_res = (add ? fs_watcher.addPath(file) : fs_watcher.removePath(file));
        if (add && prev_res) {
            futures.emplace_back(index_file(file));
        }
        if (!add && prev_res) {
            std::lock_guard<std::mutex> g(index_mutex);
            index[file].clear();
        }
        res &= prev_res;
    }
    for (auto &f : futures) {
        f.waitForFinished();
    }
    emit send_indexing_status(false);
    return res;
}

void watcher::search_text(QString const &text) {
    searching_pool.clear();
    emit stop_searching();
    for (auto& files : get_tasks()) {
        auto worker = new searching_worker(files, text, index, index_mutex);
        connect(worker, &searching_worker::send_result, mw, &main_window::add_result_item);
        connect(this, &watcher::stop_searching, worker, &searching_worker::stop);
        indexing_pool.start(worker);
    }
}

std::vector<std::vector<QString>> watcher::get_tasks() {
    std::lock_guard<std::mutex> g(index_mutex);
    std::vector<std::vector<QString>> tasks(static_cast<unsigned long>(searching_pool.maxThreadCount()));
    int cnt = 0;
    for (auto &i : index) {
        if (i.second.empty()) {
            continue;
        }
        tasks[(cnt++) % tasks.size()].emplace_back(i.first);
    }
    return tasks;
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

