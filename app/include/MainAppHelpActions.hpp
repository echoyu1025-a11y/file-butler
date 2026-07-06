// File Butler — customized edition. Maintained by qianyu.

#ifndef MAIN_APP_HELP_ACTIONS_HPP
#define MAIN_APP_HELP_ACTIONS_HPP

#include "Language.hpp"

#include <QString>

class QWidget;

class MainAppHelpActions {
public:
    static void show_about(QWidget* parent);
    static void show_quick_start(QWidget* parent);
    static void show_agpl_info(QWidget* parent);
    static QString faq_page_url();
    static bool open_faq_page();
    static QString support_page_url();
    static bool open_support_page();

#ifdef AI_FILE_SORTER_TEST_BUILD
    static QString quick_start_markdown_for_language(Language language);
#endif
};

#endif // MAIN_APP_HELP_ACTIONS_HPP
