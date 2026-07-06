/**
 * @file CleanupPage.hpp
 * @brief Embedded "File Cleanup" page shown in the main content stack.
 *
 * Inspection-only: it scans, counts, and reveals items in the system file
 * manager. It never deletes, moves, or renames anything.
 */
#pragma once

#include <QWidget>

#include <filesystem>

class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

struct CleanupScanResult;
struct CleanupGroup;

/**
 * @brief Embedded page hosting the read-only cleanup scanner.
 */
class CleanupPage : public QWidget {
public:
    /**
     * @brief Constructs the cleanup page.
     * @param parent Optional parent widget.
     */
    explicit CleanupPage(QWidget* parent = nullptr);

    /**
     * @brief Pre-fills the folder field when it is still empty.
     * @param dir Directory suggested as the scan root.
     */
    void set_initial_directory(const std::filesystem::path& dir);

private:
    /** @brief Opens a folder chooser and stores the selection. */
    void browse_for_folder();
    /** @brief Runs the read-only scan on the current folder and repopulates the tree. */
    void run_scan();
    /** @brief Populates one category group node from a scan group. */
    void populate_group(QTreeWidgetItem* group_item, const CleanupGroup& group);
    /** @brief Reveals the selected item's containing folder in the file manager. */
    void reveal_selected_in_file_manager();
    /** @brief Reveals the folder for a specific tree item, if it carries a path. */
    void reveal_item(QTreeWidgetItem* item);

    QLineEdit* path_edit_{nullptr};
    QPushButton* browse_button_{nullptr};
    QPushButton* scan_button_{nullptr};
    QPushButton* reveal_button_{nullptr};
    QLabel* status_label_{nullptr};
    QFrame* banner_{nullptr};
    QLabel* banner_caption_{nullptr};
    QLabel* banner_size_{nullptr};
    QLabel* banner_count_{nullptr};
    QTreeWidget* tree_{nullptr};
};
