#include "CleanupPage.hpp"

#include "CleanupScanService.hpp"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFont>
#include <QFrame>
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

/*
 * Read-only page: no call to remove / rename / unlink anywhere. The only
 * external effect is opening a folder in the system file manager.
 */

namespace {

/// Role storing the folder to reveal for a given tree item.
constexpr int kRevealDirRole = Qt::UserRole + 1;

QString page_tr(const char* source)
{
    return QCoreApplication::translate("CleanupPage", source);
}

QString human_size(std::uintmax_t bytes)
{
    return QString::fromStdString(CleanupScanService::format_size(bytes));
}

QString group_title(const CleanupGroup& group)
{
    switch (group.category) {
    case CleanupCategory::JunkCache:
        return page_tr("Junk / cache files");
    case CleanupCategory::Duplicate:
        return page_tr("Duplicate files");
    case CleanupCategory::LargeFile:
        return page_tr("Large files (over 100 MB)");
    case CleanupCategory::EmptyItem:
        return page_tr("Empty folders / zero-byte files");
    }
    return QString();
}

} // namespace

CleanupPage::CleanupPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root_layout = new QVBoxLayout(this);
    root_layout->setContentsMargins(24, 20, 24, 16);
    root_layout->setSpacing(10);

    // Page header (matches the prototype's view-clean header).
    auto* title = new QLabel(page_tr("File Cleanup"), this);
    QFont title_font = title->font();
    title_font.setPixelSize(18);
    title_font.setBold(true);
    title->setFont(title_font);
    root_layout->addWidget(title);

    auto* subtitle = new QLabel(
        page_tr("Analyze files you may not need. Click an item to jump to its "
                "location and delete it manually."),
        this);
    subtitle->setWordWrap(true);
    root_layout->addWidget(subtitle);
    root_layout->addSpacing(6);

    // Folder selection row.
    auto* path_row = new QHBoxLayout();
    path_edit_ = new QLineEdit(this);
    path_edit_->setPlaceholderText(page_tr("Choose a folder to scan…"));
    path_row->addWidget(path_edit_, 1);

    browse_button_ = new QPushButton(page_tr("Browse…"), this);
    path_row->addWidget(browse_button_);

    scan_button_ = new QPushButton(page_tr("Scan"), this);
    path_row->addWidget(scan_button_);
    root_layout->addLayout(path_row);

    // Status line (before/while scanning). Replaced by the banner on success.
    status_label_ = new QLabel(page_tr("No scan yet."), this);
    status_label_->setWordWrap(true);
    root_layout->addWidget(status_label_);

    // Summary banner (prototype: accent-tinted rounded card). Hidden until a scan completes.
    banner_ = new QFrame(this);
    banner_->setObjectName(QStringLiteral("cleanupBanner"));
    auto* banner_layout = new QHBoxLayout(banner_);
    banner_layout->setContentsMargins(16, 12, 16, 12);
    banner_layout->setSpacing(14);

    auto* banner_icon = new QLabel(QStringLiteral("🗄️"), banner_);
    QFont icon_font = banner_icon->font();
    icon_font.setPointSizeF(icon_font.pointSizeF() * 1.8);
    banner_icon->setFont(icon_font);
    banner_layout->addWidget(banner_icon, 0, Qt::AlignVCenter);

    auto* banner_text_col = new QVBoxLayout();
    banner_text_col->setSpacing(2);
    banner_caption_ = new QLabel(
        page_tr("Scan complete — estimated reclaimable space"), banner_);
    banner_text_col->addWidget(banner_caption_);

    banner_size_ = new QLabel(banner_);
    banner_size_->setObjectName(QStringLiteral("cleanupBannerSize"));
    QFont size_font = banner_size_->font();
    size_font.setPixelSize(24);
    size_font.setBold(true);
    banner_size_->setFont(size_font);
    banner_text_col->addWidget(banner_size_);
    banner_layout->addLayout(banner_text_col, 1);

    banner_count_ = new QLabel(banner_);
    banner_layout->addWidget(banner_count_, 0, Qt::AlignTop);

    banner_->setVisible(false);
    root_layout->addWidget(banner_);

    // Results tree (four category groups).
    tree_ = new QTreeWidget(this);
    tree_->setColumnCount(3);
    tree_->setHeaderLabels({page_tr("Name"), page_tr("Size"), page_tr("Location")});
    tree_->setAlternatingRowColors(true);
    tree_->setUniformRowHeights(true);
    tree_->header()->setStretchLastSection(true);
    tree_->header()->resizeSection(0, 280);
    tree_->header()->resizeSection(1, 100);
    root_layout->addWidget(tree_, 1);

    // Bottom row: reveal button + safety note.
    auto* bottom_row = new QHBoxLayout();
    reveal_button_ = new QPushButton(page_tr("Show in File Manager"), this);
    reveal_button_->setEnabled(false);
    bottom_row->addWidget(reveal_button_);

    auto* safety_note = new QLabel(
        page_tr("🛡 For safety, nothing is deleted automatically. Please review "
                "and remove files yourself."),
        this);
    safety_note->setWordWrap(true);
    bottom_row->addWidget(safety_note, 1);
    root_layout->addLayout(bottom_row);

    // Wiring (lambdas keep us free of Q_OBJECT, matching project convention).
    connect(browse_button_, &QPushButton::clicked, this, [this]() { browse_for_folder(); });
    connect(scan_button_, &QPushButton::clicked, this, [this]() { run_scan(); });
    connect(reveal_button_, &QPushButton::clicked, this,
            [this]() { reveal_selected_in_file_manager(); });

    connect(tree_, &QTreeWidget::itemDoubleClicked, this,
            [this](QTreeWidgetItem* item, int /*column*/) { reveal_item(item); });
    connect(tree_, &QTreeWidget::itemSelectionChanged, this, [this]() {
        const auto selected = tree_->selectedItems();
        const bool revealable = !selected.isEmpty() &&
                                selected.front()->data(0, kRevealDirRole).isValid();
        reveal_button_->setEnabled(revealable);
    });
}

void CleanupPage::set_initial_directory(const std::filesystem::path& dir)
{
    if (dir.empty() || !path_edit_ || !path_edit_->text().trimmed().isEmpty()) {
        return;
    }
    path_edit_->setText(QString::fromStdString(dir.string()));
}

void CleanupPage::browse_for_folder()
{
    const QString start = path_edit_->text();
    const QString directory =
        QFileDialog::getExistingDirectory(this, page_tr("Select folder to scan"), start);
    if (!directory.isEmpty()) {
        path_edit_->setText(directory);
    }
}

void CleanupPage::run_scan()
{
    const QString path_text = path_edit_->text().trimmed();
    if (path_text.isEmpty()) {
        banner_->setVisible(false);
        status_label_->setVisible(true);
        status_label_->setText(page_tr("Please choose a folder to scan first."));
        return;
    }

    const std::filesystem::path root(path_text.toStdString());
    banner_->setVisible(false);
    status_label_->setVisible(true);
    status_label_->setText(page_tr("Scanning…"));
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

    // Success: show the banner, hide the plain status line.
    banner_size_->setText(human_size(result.total_reclaimable()));
    banner_count_->setText(
        page_tr("%1 item(s) found").arg(result.total_items()));
    status_label_->setVisible(false);
    banner_->setVisible(true);
}

void CleanupPage::populate_group(QTreeWidgetItem* group_item, const CleanupGroup& group)
{
    const QString header =
        page_tr("%1  —  %2 item(s), %3")
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
                page_tr("duplicate of %1").arg(QString::fromStdString(item.note)));
        }
        child->setText(0, name);
        child->setText(1, item.is_directory ? page_tr("(folder)") : human_size(item.size_bytes));
        child->setText(2, QString::fromStdString(item.parent_dir.string()));

        // Store the folder to reveal; empty dirs reveal their own parent.
        child->setData(0, kRevealDirRole,
                       QString::fromStdString(item.parent_dir.string()));
    }

    group_item->setExpanded(!group.items.empty());
}

void CleanupPage::reveal_selected_in_file_manager()
{
    const auto selected = tree_->selectedItems();
    if (!selected.isEmpty()) {
        reveal_item(selected.front());
    }
}

void CleanupPage::reveal_item(QTreeWidgetItem* item)
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
