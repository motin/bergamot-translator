/*
 * TranslationResult.h
 *
 * The class that represents the result of AbstractTranslationModel::translate() API for each of its text entry and
 * TranslationRequest.
 */

#ifndef SRC_TRANSLATOR_TRANSLATIONRESULT_H_
#define SRC_TRANSLATOR_TRANSLATIONRESULT_H_

#include <vector>
#include <string>

#include "QualityScore.h"

/* This class represents the result of AbstractTranslationModel::translate() API for each of its text entry and
 * TranslationRequest.
 */
class TranslationResult {
public:
	typedef std::vector<std::pair<std::string_view, std::string_view>> SentenceMappings;

	TranslationResult(const std::string &original, const std::string &translation) :
		originalText(original), translatedText(translation) {}

	TranslationResult(std::string &&original, std::string &&translation) :
		originalText(std::move(original)), translatedText(std::move(translation)) {}

	/* Return the original text. */
	const std::string& getOriginalText() const {
		return originalText;
	}

	/* Return the translated text. */
	const std::string& getTranslatedText() const {
		return translatedText;
	}

	/* Return the Quality scores of the translated text. */
	const QualityScore& getQualityScore() const {
		return qualityScore;
	}

	/* Return the Sentence mappings (information regarding how individual sentences of originalText map to
	 * corresponding translated sentences in translatedText).
	 */
	const SentenceMappings& getSentenceMappings() const {
		return sentenceMappings;
	}

private:
	// Original text (in UTF-8 encoded format) that was supposed to be translated
	std::string originalText;

	// Translation (in UTF-8 encoded format) of the originalText
	std::string translatedText;

	// Quality score of the translated text at the granularity specified in TranslationRequest.
	// It is an optional result (it will have no information if not requested in TranslationRequest)
	QualityScore qualityScore;

	// Information regarding how individual sentences of originalText map to corresponding translated sentences
	// in joined translated text (translatedText)
	// An example of sentence mapping:
	//     originalText (contains 2 sentences)              = "What is your name? My name is Abc."
	//     translatedText (contains 2 translated sentences) = "Was ist dein Name? Mein Name ist Abc."
	//     sentenceMappings = [
	//         {"What is your name?", "Was ist dein Name?"},  // Pair(originalText[0],translatedText[0])
	//         {"My name is Abc", "Mein Name ist Abc."}       // Pair(originalText[1],translatedText[1])
	//     ]
	//
	// It is an optional result (it will be empty if not requested in TranslationRequest).
	SentenceMappings sentenceMappings;
};

#endif /* SRC_TRANSLATOR_TRANSLATIONRESULT_H_ */
