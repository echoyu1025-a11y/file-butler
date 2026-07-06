#ifndef CATEGORYLANGUAGESUPPORT_HPP
#define CATEGORYLANGUAGESUPPORT_HPP

#include "CategoryLanguage.hpp"
#include "Types.hpp"

#include <vector>

/**
 * @brief Returns the complete category-language set supported by the application UI/runtime.
 * @return Ordered list of category languages that can be shown in the menu.
 */
const std::vector<CategoryLanguage>& all_category_languages();

/**
 * @brief Returns the category languages that should be selectable for the given LLM choice.
 * @param choice Active LLM choice.
 * @return Ordered list of category languages supported for that LLM choice.
 */
const std::vector<CategoryLanguage>& supported_category_languages_for_llm_choice(LLMChoice choice);

/**
 * @brief Reports whether a category language is supported for the given LLM choice.
 * @param choice Active LLM choice.
 * @param language Category language to evaluate.
 * @return True when the language should remain selectable for that LLM choice.
 */
bool is_category_language_supported_for_llm_choice(LLMChoice choice, CategoryLanguage language);

#endif // CATEGORYLANGUAGESUPPORT_HPP
