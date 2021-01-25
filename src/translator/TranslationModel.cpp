/*
 * TranslationModel.cpp
 *
 */

#include <future>
#include <vector>

#include "3rd_party/marian-dev/src/3rd_party/yaml-cpp/yaml.h"
#include "3rd_party/marian-dev/src/common/config_parser.h"
#include "TranslationModel.h"
#include "common/config_validator.h"
#include "common/options.h"
#include "translator/service.h"

std::shared_ptr<marian::Options> parseOptions(const std::string &config) {
  marian::Options options;
  marian::ConfigParser configParser(marian::cli::mode::translation);

  const YAML::Node &defaultConfig = configParser.getConfig();
  options.merge(defaultConfig);
  options.parse(config);
  YAML::Node configCopy = options.cloneToYamlNode();
  marian::ConfigValidator validator(configCopy);
  validator.validateOptions(marian::cli::mode::translation);
  return std::make_shared<marian::Options>(options);
}

TranslationModel::TranslationModel(const std::string &config)
    : configOptions_(std::move(parseOptions(config))),
      AbstractTranslationModel(), service_(configOptions_) {}

TranslationModel::~TranslationModel() {}

std::future<std::vector<TranslationResult>>
TranslationModel::translate(std::vector<std::string> &&texts,
                            TranslationRequest request) {

  // Implementing a non-async version first. Unpleasant, but should work.
  std::promise<std::vector<TranslationResult>> promise;
  auto future = promise.get_future();

  auto convert = [](marian::bergamot::TranslationResult &mTranslationResult) {
    // Change marian::string_view to std::string_view
    TranslationResult::SentenceMappings sentenceMappings;
    for (auto &p : sentenceMappings) {
      std::string_view src(p.first.data(), p.first.size()),
          tgt(p.second.data(), p.second.size());
      sentenceMappings.emplace_back(src, tgt);
    }

    TranslationResult translationResult(
        std::move(mTranslationResult.source_),
        std::move(mTranslationResult.translation_),
        std::move(sentenceMappings));

    return translationResult;
  };

  // This code, move into async?
  std::vector<TranslationResult> translationResults;
  for (auto &text : texts) {
    // Copying text, can also be replaced with move based function.
    // translate(...)
    auto intermediate = service_.translateWithCopy(text);
    intermediate.wait();
    marian::bergamot::TranslationResult result = intermediate.get();
    translationResults.push_back(convert(result));
  }

  promise.set_value(translationResults);
  return future;
}

bool TranslationModel::isAlignmentSupported() const { return false; }
