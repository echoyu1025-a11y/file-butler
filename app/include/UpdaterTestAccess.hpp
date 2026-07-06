#pragma once

#ifdef AI_FILE_SORTER_TEST_BUILD

#include "UpdateFeed.hpp"

#include <QString>
#include <functional>
#include <optional>
#include <string>

class QWidget;
class Updater;

class UpdaterTestAccess {
public:
    static bool is_update_available(Updater& updater);
    static std::optional<UpdateInfo> current_update_info(const Updater& updater);
    static std::optional<std::string> selected_update_spec_file_url(const Updater& updater);
    static bool has_update_task(const Updater& updater);
    static void wait_for_update_task(Updater& updater);
    static void set_open_download_url_handler(Updater& updater,
                                              std::function<void(const std::string&)> handler);
    static void set_quit_handler(Updater& updater,
                                 std::function<void()> handler);
    static bool trigger_update_action(Updater& updater,
                                      const UpdateInfo& info,
                                      QWidget* parent,
                                      bool quit_after_open);
    static void show_required_update_dialog(Updater& updater,
                                            const UpdateInfo& info,
                                            QWidget* parent);
    static void show_optional_update_dialog(Updater& updater,
                                            const UpdateInfo& info,
                                            QWidget* parent);
    static bool handle_update_error(Updater& updater,
                                    const UpdateInfo& info,
                                    const QString& message,
                                    QWidget* parent,
                                    bool quit_after_open);
};

#endif // AI_FILE_SORTER_TEST_BUILD
