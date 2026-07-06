/**
 * @file CleanupDialog.hpp
 * @brief Settings dialog that scans a folder and lists cleanup candidates.
 *
 * This dialog is inspection-only. It reveals items in the system file
 * manager but never deletes, moves, or renames anything.
 */
#pragma once

#include <QDialog>

#include <filesystem>

class QLabel;
class QLineEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

struct CleanupScanResult;
struct CleanupGroup;

/**
 * @brief Modal dialog for the "Cleanup Suggestions" scanner.
 */
class CleanupDialog : public QDialog {
public:
    /**
     * @brief Constructs the cleanup suggestions dialog.
     * @param initial_dir Directory pre-filled into the path field (optional).
     * @param parent Optional parent widget.
     */
    explicit CleanupDialog(const std::filesystem::path& initial_dir,
                           QWidget* parent = nullptr);

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
    QLabel* summary_label_{nullptr};
    QTreeWidget* tree_{nullptr};
};
