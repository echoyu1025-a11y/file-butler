#ifndef CATEGORYLANGUAGE_HPP
#define CATEGORYLANGUAGE_HPP

#include <QString>
#include <array>
#include <string>

#define AI_FILE_SORTER_CATEGORY_LANGUAGE_LIST(X) \
    X(Afrikaans, "Afrikaans") \
    X(Akan, "Akan") \
    X(Albanian, "Albanian") \
    X(Amharic, "Amharic") \
    X(Arabic, "Arabic") \
    X(Armenian, "Armenian") \
    X(Assamese, "Assamese") \
    X(Azerbaijani, "Azerbaijani") \
    X(Basque, "Basque") \
    X(Belarusian, "Belarusian") \
    X(Bengali, "Bengali") \
    X(Bhojpuri, "Bhojpuri") \
    X(Bosnian, "Bosnian") \
    X(Bulgarian, "Bulgarian") \
    X(Burmese, "Burmese") \
    X(Catalan, "Catalan") \
    X(Corsican, "Corsican") \
    X(Croatian, "Croatian") \
    X(Czech, "Czech") \
    X(Danish, "Danish") \
    X(Dutch, "Dutch") \
    X(English, "English") \
    X(Esperanto, "Esperanto") \
    X(Estonian, "Estonian") \
    X(Finnish, "Finnish") \
    X(French, "French") \
    X(Frisian, "Frisian") \
    X(Galician, "Galician") \
    X(Georgian, "Georgian") \
    X(German, "German") \
    X(Greek, "Greek") \
    X(Gujarati, "Gujarati") \
    X(HaitianCreole, "Haitian Creole") \
    X(Hausa, "Hausa") \
    X(Hebrew, "Hebrew") \
    X(Hindi, "Hindi") \
    X(Hungarian, "Hungarian") \
    X(Icelandic, "Icelandic") \
    X(Igbo, "Igbo") \
    X(Indonesian, "Indonesian") \
    X(Irish, "Irish") \
    X(Italian, "Italian") \
    X(Japanese, "Japanese") \
    X(Javanese, "Javanese") \
    X(Kannada, "Kannada") \
    X(Kazakh, "Kazakh") \
    X(Khmer, "Khmer") \
    X(Kinyarwanda, "Kinyarwanda") \
    X(Korean, "Korean") \
    X(Kurdish, "Kurdish") \
    X(Kyrgyz, "Kyrgyz") \
    X(Lao, "Lao") \
    X(Latvian, "Latvian") \
    X(Lithuanian, "Lithuanian") \
    X(Luxembourgish, "Luxembourgish") \
    X(Macedonian, "Macedonian") \
    X(Malay, "Malay") \
    X(Malayalam, "Malayalam") \
    X(Maltese, "Maltese") \
    X(Marathi, "Marathi") \
    X(Mongolian, "Mongolian") \
    X(Nepali, "Nepali") \
    X(Norwegian, "Norwegian") \
    X(Odia, "Odia") \
    X(Pashto, "Pashto") \
    X(Persian, "Persian") \
    X(Polish, "Polish") \
    X(Portuguese, "Portuguese") \
    X(Punjabi, "Punjabi") \
    X(Romanian, "Romanian") \
    X(Russian, "Russian") \
    X(Serbian, "Serbian") \
    X(SimplifiedChinese, "Simplified Chinese") \
    X(Sindhi, "Sindhi") \
    X(Sinhala, "Sinhala") \
    X(Slovak, "Slovak") \
    X(Slovenian, "Slovenian") \
    X(Somali, "Somali") \
    X(Spanish, "Spanish") \
    X(Swahili, "Swahili") \
    X(Swedish, "Swedish") \
    X(Tagalog, "Tagalog") \
    X(Tamil, "Tamil") \
    X(Telugu, "Telugu") \
    X(Thai, "Thai") \
    X(TraditionalChinese, "Traditional Chinese") \
    X(Turkish, "Turkish") \
    X(Ukrainian, "Ukrainian") \
    X(Urdu, "Urdu") \
    X(Uyghur, "Uyghur") \
    X(Uzbek, "Uzbek") \
    X(Vietnamese, "Vietnamese") \
    X(Welsh, "Welsh") \
    X(Xhosa, "Xhosa") \
    X(Yoruba, "Yoruba") \
    X(Zulu, "Zulu")

enum class CategoryLanguage {
#define AI_FILE_SORTER_DECLARE_CATEGORY_LANGUAGE(name, display_name) name,
    AI_FILE_SORTER_CATEGORY_LANGUAGE_LIST(AI_FILE_SORTER_DECLARE_CATEGORY_LANGUAGE)
#undef AI_FILE_SORTER_DECLARE_CATEGORY_LANGUAGE
    Count
};

inline constexpr std::size_t kCategoryLanguageCount =
    static_cast<std::size_t>(CategoryLanguage::Count);

inline constexpr std::size_t categoryLanguageIndex(CategoryLanguage language)
{
    return static_cast<std::size_t>(language);
}

inline QString categoryLanguageToString(CategoryLanguage language)
{
    static const std::array<const char*, kCategoryLanguageCount> names = {
#define AI_FILE_SORTER_CATEGORY_LANGUAGE_NAME(name, display_name) display_name,
        AI_FILE_SORTER_CATEGORY_LANGUAGE_LIST(AI_FILE_SORTER_CATEGORY_LANGUAGE_NAME)
#undef AI_FILE_SORTER_CATEGORY_LANGUAGE_NAME
    };

    const auto idx = categoryLanguageIndex(language);
    if (idx < names.size()) {
        return QString::fromUtf8(names[idx]);
    }
    return QStringLiteral("English");
}

inline QString normalizeCategoryLanguageKey(QString value)
{
    value = value.trimmed().toLower();
    value.replace(QChar('-'), QChar(' '));
    value.replace(QChar('_'), QChar(' '));
    return value.simplified();
}

inline CategoryLanguage categoryLanguageFromString(const QString& value)
{
    const QString normalized = normalizeCategoryLanguageKey(value);
    for (std::size_t idx = 0; idx < kCategoryLanguageCount; ++idx) {
        const auto language = static_cast<CategoryLanguage>(idx);
        if (normalizeCategoryLanguageKey(categoryLanguageToString(language)) == normalized) {
            return language;
        }
    }

    struct Alias {
        const char* key;
        CategoryLanguage language;
    };

    static const std::array<Alias, 13> aliases = {{
        {"bokmal", CategoryLanguage::Norwegian},
        {"filipino", CategoryLanguage::Tagalog},
        {"farsi", CategoryLanguage::Persian},
        {"myanmar", CategoryLanguage::Burmese},
        {"nb", CategoryLanguage::Norwegian},
        {"nb no", CategoryLanguage::Norwegian},
        {"no", CategoryLanguage::Norwegian},
        {"norwegian bokmal", CategoryLanguage::Norwegian},
        {"traditional chinese", CategoryLanguage::TraditionalChinese},
        {"zh cn", CategoryLanguage::SimplifiedChinese},
        {"zh hans", CategoryLanguage::SimplifiedChinese},
        {"zh hant", CategoryLanguage::TraditionalChinese},
        {"zh tw", CategoryLanguage::TraditionalChinese},
    }};
    for (const Alias& alias : aliases) {
        if (normalized == QString::fromLatin1(alias.key)) {
            return alias.language;
        }
    }
    return CategoryLanguage::English;
}

inline std::string categoryLanguageDisplay(CategoryLanguage lang) {
    return categoryLanguageToString(lang).toStdString();
}

#undef AI_FILE_SORTER_CATEGORY_LANGUAGE_LIST

#endif // CATEGORYLANGUAGE_HPP
