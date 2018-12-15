#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemWatcher>
#include "watcher.h"
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <QMainWindow>
#include "watcher.h"

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow {
Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();
private:
    void closeEvent(QCloseEvent *event);
public slots:
    void add_directory();
    void remove_dirs();
    void show_about_dialog();
    void add_result_item(QString const &duplicates);
    void set_indexing_status(bool f);
    void search_text(QString const &query);
private:
    std::unique_ptr<Ui::MainWindow> ui;
    watcher w;
    bool cleared = false;
};

#endif // MAINWINDOW_H
