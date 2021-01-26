#include "translation_result.h"
#include "common/logging.h"
#include "data/alignment.h"

#include <utility>

namespace marian {
namespace bergamot {

TranslationResult::TranslationResult(std::string &&source, Segments &&segments,
                                     std::vector<TokenRanges> &&sourceRanges,
                                     Histories &&histories,
                                     std::vector<Ptr<Vocab const>> &vocabs)
    : source_(std::move(source)), sourceRanges_(std::move(sourceRanges)),
      segments_(std::move(segments)), histories_(std::move(histories)),
      vocabs_(&vocabs) {

  // Process sourceMappings into sourceMappings_.
  LOG(info, "Creating sourcemappings");
  sourceMappings_.reserve(segments_.size());
  for (int i = 0; i < segments_.size(); i++) {
    string_view first = sourceRanges_[i].front();
    string_view last = sourceRanges_[i].back();
    sourceMappings_.emplace_back(first.data(), last.end() - first.begin());
    std::cout << "Reconstructed " << i << ":" << sourceMappings_.back()
              << std::endl;
  }

  // Compiles translations into a single std::string translation_
  // Current implementation uses += on std::string, multiple resizes.
  // Stores ByterRanges as indices first, followed by conversion into
  // string_views.
  // TODO(jerin): Add token level string_views here as well.
  LOG(info, "Decoding");
  std::vector<std::pair<int, int>> translationRanges;
  size_t offset{0}, end{0}, start{0};
  bool first{true};
  for (auto &history : histories_) {
    // TODO(jerin): Change hardcode of nBest = 1
    NBestList onebest = history->nBest(1);

    Result result = onebest[0]; // Expecting only one result;
    Words words = std::get<0>(result);
    std::string decoded = vocabs_->back()->decode(words);
    if (first) {
      first = false;
    } else {
      translation_ += " ";
      ++offset;
    }

    translation_ += decoded;
    translationRanges.emplace_back(offset, decoded.size());
    offset += decoded.size();
  }

  // Converting ByteRanges as indices into string_views.
  LOG(info, "generating targetMappings");
  targetMappings_.reserve(translationRanges.size());
  for (auto &p : translationRanges) {
    const char *begin = &translation_[p.first];
    targetMappings_.emplace_back(begin, p.second);
    std::cout << "Reconstructed targetMapping " << targetMappings_.back()
              << std::endl;
  }

  // Surely, let's add sentenceMappings_
  LOG(info, "generating SentenceMappings");
  for (auto p = sourceMappings_.begin(), q = targetMappings_.begin();
       p != sourceMappings_.end() && q != targetMappings_.end(); ++p, ++q) {
    sentenceMappings_.emplace_back(*p, *q);
    auto &t = sentenceMappings_.back();
    std::cout << "translationResult [src] " << t.first << std::endl;
    std::cout << "translationResult [tgt] " << t.second << std::endl;
  }
}

} // namespace bergamot
} // namespace marian
