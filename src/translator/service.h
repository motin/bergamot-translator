#ifndef SRC_BERGAMOT_SERVICE_H_
#define SRC_BERGAMOT_SERVICE_H_

#include "batch_translator.h"
#include "batcher.h"
#include "pcqueue.h"
#include "textops.h"
#include "translation_result.h"

#include <queue>
#include <vector>

#include "data/types.h"

namespace marian {
namespace bergamot {

class Service {

  // Service exposes methods to translate an incoming blob of text to the
  // Consumer of bergamot API.
  //
  // An example use of this API looks as follows:
  //
  //  options = ...;
  //  service = Service(options);
  //  std::string input_blob = "Hello World";
  //  std::future<TranslationResult>
  //      response = service.translate(std::move(input_blob));
  //  response.wait();
  //  TranslationResult result = response.get();

public:
  explicit Service(Ptr<Options> options);

  // Constructs new string copying, calls translate internally.
  std::future<TranslationResult> translateWithCopy(std::string input);
  std::future<TranslationResult> translate(std::string &&input);

  void stop();

  Ptr<Vocab const> sourceVocab() const { return vocabs_.front(); }
  Ptr<Vocab const> targetVocab() const { return vocabs_.back(); }

  ~Service();

private:
  unsigned int requestId_;
  unsigned int batchNumber_;
  int numWorkers_;

  // vocabs are used to construct a Request, which later uses it to construct
  // TranslationResult (decode from words to string).
  std::vector<Ptr<Vocab const>> vocabs_; // ORDER DEPENDENCY

  // Consists of:
  //
  // 1. text-processing class (TextProcessor), which handles breaking a blob of
  //    text into sentences and providing them representated by finite
  //    vocabulary for further processing by hte neural machine translation.
  // 2. a Batcher class which handles efficient batching by minimizing
  //    padding wasting compute.
  // 3. Multiple workers - which are instances of BatchTranslators are
  //    spawned in separate threads.
  //
  // Batcher acts as a producer for a producer-consumer queue (pcqueue_), with
  // idle BatchTranslators being consumers requesting batches as they're ready.

  TextProcessor text_processor_; // ORDER DEPENDENCY
  Batcher batcher_;
  PCQueue<PCItem> pcqueue_;
  std::vector<BatchTranslator> workers_;
};

std::vector<Ptr<const Vocab>> loadVocabularies(Ptr<Options> options);

} // namespace bergamot
} // namespace marian

#endif // SRC_BERGAMOT_SERVICE_H_