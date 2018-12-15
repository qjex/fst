#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QtWidgets>
#include <QLocale>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), w(this) {
//    qRegisterMetaType<std::unordered_map<QByteArray, std::vector<QString>>>(
//        "std::unordered_map<QByteArray,std::vector<QString>>");
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    ui->label->setText("");

    ui->dirListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QCommonStyle style;
    ui->actionAdd_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionAdd_Directory, &QAction::triggered, this, &main_window::add_directory);
    connect(ui->actionExit, &QAction::triggered, this, &main_window::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    connect(ui->textInput, &QLineEdit::textEdited, this, &main_window::search_text);
    connect(ui->removeDirButton, SIGNAL(released()), SLOT(remove_dirs()));
}

main_window::~main_window() {}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}

void main_window::add_result_item(const QString &filename) {
    if (cleared) {
        ui->listWidget->clear();
        cleared = false;
    }
    auto *item = new QListWidgetItem();
    item->setText(filename);
    ui->listWidget->addItem(item);
}

void main_window::set_indexing_status(bool f) {
    ui->label->setText(f ? "Indexing..." : "");
}

void main_window::closeEvent(QCloseEvent *event) {
    w.shutdown_pools();
}

void main_window::add_directory() {
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    "Select Directory for watching",
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->dirListWidget->addItem(dir);
    w.update_watch_files(dir, true);
}

void main_window::remove_dirs() {
    auto selected = ui->dirListWidget->selectedItems();
    for (auto dir_item : selected) {
        w.update_watch_files(dir_item->text(), false);
    }
    qDeleteAll(selected);
}

void main_window::search_text(QString const &query) {
    if (query.length() == 0) {
        ui->listWidget->clear();
        return;
    }
    cleared = true;
    w.search_text(query);
    ui->listWidget->clear();
}
