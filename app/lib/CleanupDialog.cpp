#include "CleanupDialog.hpp"

#include "CleanupScanService.hpp"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUrl>
#include <QVBoxLayout>
#include <QVariant>

namespace {

/// Role storing the folder to reveal for a given tree item.
constexpr int kRevealDirRole = Qt::UserRole + 1;

QString dialog_tr(const char* source)
{
    return QCoreApplication::translate("CleanupDialog", source);
}

QString human_size(std::uintmax_t bytes)
{
    return QString::fromStdString(CleanupScanService::format_size(bytes));
}

QString group_title(const CleanupGroup& group)
{
    switch (group.category) {
    case CleanupCategory::JunkCache:
        return dialog_tr("Junk / cache files");
    case CleanupCategory::Duplicate:
        return dialog_tr("Duplicate files");
    case CleanupCategory::LargeFile:
        return dialog_tr("Large files (over 100 MB)");
    case CleanupCategory::EmptyItem:
        return dialog_tr("Empty folders / zero-byte files");
    }
    return QString();
}

} // namespace

CleanupDialog::CleanupDialog(const std::filesystem::path& initial_dir, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(dialog_tr("Cleanup Suggestions"));
    setMinimumWidth(720);
    resize(820, 600);

    auto* root_layout = new QVBoxLayout(this);
    root_layout->setContentsMargins(12, 12, 12, 12);
    root_layout->setSpacing(10);

    // Disclaimer (translated). Emphasizes the read-only nature of the tool.
    auto* disclaimer = new QLabel(
        dialog_tr("This tool only counts and locates files. It never deletes or moves "
                  "anything. Please review the results yourself and delete items manually "
                  "in your file manager."),
        this);
    disclaimer->setWordWrap(true);
    root_layout->addWidget(disclaimer);

    // Folder selection row.
    auto* path_row = new QHBoxLayout();
    path_edit_ = new QLineEdit(this);
    path_edit_->setPlaceholderText(dialog_tr("Choose a folder to scan…"));
    if (!initial_dir.empty()) {
        path_edit_->setText(QString::fromStdString(initial_dir.string()));
    }
    path_row->addWidget(path_edit_, 1);

    browse_button_ = new QPushButton(dialog_tr("Browse…"), this);
    path_row->addWidget(browse_button_);

    scan_button_ = new QPushButton(dialog_tr("Scan"), this);
    path_row->addWidget(scan_button_);
    root_layout->addLayout(path_row);

    // Summary line.
    summary_label_ = new QLabel(dialog_tr("No scan yet."), this);
    summary_label_->setWordWrap(true);
    root_layout->addWidget(summary_label_);

    // Results tree.
    tree_ = new QTreeWidget(this);
    tree_->setColumnCount(3);
    tree_->setHeaderLabels({dialog_tr("Name"), dialog_tr("Size"), dialog_tr("Location")});
    tree_->setAlternatingRowColors(true);
    tree_->setUniformRowHeights(true);
    tree_->header()->setStretchLastSection(true);
    tree_->header()->resizeSection(0, 280);
    tree_->header()->resizeSection(1, 100);
    root_layout->addWidget(tree_, 1);

    // Bottom buttons.
    auto* bottom_row = new QHBoxLayout();
    reveal_button_ = new QPushButton(dialog_tr("Show in File Manager"), this);
    reveal_button_->setEnabled(false);
    bottom_row->addWidget(reveal_button_);
    bottom_row->addStretch(1);

    auto* button_box = new QDialogButtonBox(QDialogButtonBox::Close, this);
    // Qt 标准按钮的自带翻译未随应用打包，手动设置文字以走应用自己的 i18n
    if (auto* close_button = button_box->button(QDialogButtonBox::Close)) {
        close_button->setText(dialog_tr("Close"));
    }
    bottom_row->addWidget(button_box);
    root_layout->addLayout(bottom_row);

    // Wiring (lambdas / base-class member pointers keep us free of Q_OBJECT).
    connect(browse_button_, &QPushButton::clicked, this, [this]() { browse_for_folder(); });
    connect(scan_button_, &QPushButton::clicked, this, [this]() { run_scan(); });
    connect(reveal_button_, &QPushButton::clicked, this,
            [this]() { reveal_selected_in_file_manager(); });
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::accept);

    connect(tree_, &QTreeWidget::itemDoubleClicked, this,
            [this](QTreeWidgetItem* item, int /*column*/) { reveal_item(item); });
    connect(tree_, &QTreeWidget::itemSelectionChanged, this, [this]() {
        const auto selected = tree_->selectedItems();
        const bool revealable = !selected.isEmpty() &&
                                selected.front()->data(0, kRevealDirRole).isValid();
        reveal_button_->setEnabled(revealable);
    });
}

void CleanupDialog::browse_for_folder()
{
    const QString start = path_edit_->text();
    const QString directory =
        QFileDialog::getExistingDirectory(this, dialog_tr("Select folder to scan"), start);
    if (!directory.isEmpty()) {
        path_edit_->setText(directory);
    }
}

void CleanupDialog::run_scan()
{
    const QString path_text = path_edit_->text().trimmed();
    if (path_text.isEmpty()) {
        summary_label_->setText(dialog_tr("Please choose a folder to scan first."));
        return;
    }

    const std::filesystem::path root(path_text.toStdString());
    summary_label_->setText(dialog_tr("Scanning…"));
    tree_->clear();
    reveal_button_->setEnabled(false);
    QCoreApplication::processEvents();

    const CleanupScanResult result = CleanupScanService::scan(root);

    auto* junk_item = new QTreeWidgetItem(tree_);
    populate_group(junk_item, result.junk_cache);
    auto* dup_item = new QTreeWidgetItem(tree_);
    populate_group(dup_item, result.duplicates);
    auto* large_item = new QTreeWidgetItem(tree_);
    populate_group(large_item, result.large_files);
    auto* empty_item = new QTreeWidgetItem(tree_);
    populate_group(empty_item, result.empty_items);

    const QString summary =
        dialog_tr("Found %1 cleanup item(s), about %2 reclaimable in total.")
            .arg(result.total_items())
            .arg(human_size(result.total_reclaimable()));
    summary_label_->setText(summary);
}

void CleanupDialog::populate_group(QTreeWidgetItem* group_item, const CleanupGroup& group)
{
    const QString header =
        dialog_tr("%1  —  %2 item(s), %3")
            .arg(group_title(group))
            .arg(group.items.size())
            .arg(human_size(group.total_reclaimable));
    group_item->setText(0, header);
    group_item->setFirstColumnSpanned(true);

    for (const CleanupItem& item : group.items) {
        auto* child = new QTreeWidgetItem(group_item);

        QString name = QString::fromStdString(item.path.filename().string());
        if (item.is_directory) {
            name += QStringLiteral("/");
        }
        if (!item.note.empty()) {
            name += QStringLiteral(" (%1)").arg(
                dialog_tr("duplicate of %1").arg(QString::fromStdString(item.note)));
        }
        child->setText(0, name);
        child->setText(1, item.is_directory ? dialog_tr("(folder)") : human_size(item.size_bytes));
        child->setText(2, QString::fromStdString(item.parent_dir.string()));

        // Store the folder to reveal; empty dirs reveal their own parent.
        child->setData(0, kRevealDirRole,
                       QString::fromStdString(item.parent_dir.string()));
    }

    group_item->setExpanded(!group.items.empty());
}

void CleanupDialog::reveal_selected_in_file_manager()
{
    const auto selected = tree_->selectedItems();
    if (!selected.isEmpty()) {
        reveal_item(selected.front());
    }
}

void CleanupDialog::reveal_item(QTreeWidgetItem* item)
{
    if (!item) {
        return;
    }
    const QVariant dir_value = item->data(0, kRevealDirRole);
    if (!dir_value.isValid()) {
        return;
    }
    const QString dir = dir_value.toString();
    if (dir.isEmpty()) {
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}
