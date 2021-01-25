/*
 * main.cpp
 *
 * An example application to demonstrate the use of Bergamot translator.
 *
 */

#include <iostream>

#include "AbstractTranslationModel.h"
#include "TranslationModelConfiguration.h"
#include "TranslationRequest.h"
#include "TranslationResult.h"
#include "translator/parser.h"

int main(int argc, char **argv) {

  // Create an instance of AbstractTranslationModel with a dummy model
  // configuration TranslationModelConfiguration config("dummy_modelFilePath",
  // 			"dummy_sourceVocabPath",
  // 			"dummy_targetVocabPath");
  auto cp = marian::bergamot::createConfigParser();
  auto options = cp.parseOptions(argc, argv, true);
  std::string config = options->asYamlString();
  std::cout << config << std::endl;

  std::shared_ptr<AbstractTranslationModel> model =
      AbstractTranslationModel::createInstance(config);

  // Call to translate a dummy (empty) texts with a dummy (empty) translation
  // request
  TranslationRequest req;
  std::vector<std::string> texts;
  for (int i = 0; i < 100; i++) {
    texts.emplace_back(
        "The Bergamot project will add and improve client-side machine"
        "translation in a web browser.  Unlike current cloud-based"
        "options, running directly on users’ machines empowers citizens to"
        "preserve their privacy and increases the uptake of language"
        "technologies in Europe in various sectors that require"
        "confidentiality. Free software integrated with an open-source web"
        "browser, such as Mozilla Firefox, will enable bottom-up adoption"
        "by non-experts, resulting in cost savings for private and public"
        "sector users who would otherwise procure translation or operate"
        "monolingually.  Bergamot is a consortium coordinated by the"
        "University of Edinburgh with partners Charles University in"
        "Prague, the University of Sheffield, University of Tartu, and"
        "Mozilla.");
  }

  auto result = model->translate(std::move(texts), req);

  // Resolve the future and get the actual result
  std::vector<TranslationResult> res = result.get();

  std::cout << "Count is: " << res.size() << std::endl;
  return 0;
}
