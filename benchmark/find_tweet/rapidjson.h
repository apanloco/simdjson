#pragma once

#ifdef SIMDJSON_COMPETITION_RAPIDJSON

#include "find_tweet.h"

namespace find_tweet {

using namespace rapidjson;

template<int F>
class rapidjson_base {
  Document doc{};

public:
  bool run(const padded_string &json, uint64_t find_id, std::string_view &text) {
    auto &root = doc.Parse<F>(json.data());
    if (root.HasParseError() || !root.IsObject()) { return false; }
    auto statuses = root.FindMember("statuses");
    if (statuses == root.MemberEnd() || !statuses->value.IsArray()) { return false; }
    for (auto &tweet : statuses->value.GetArray()) {
      if (!tweet.IsObject()) { return false; }
      auto id = tweet.FindMember("id");
      if (id == tweet.MemberEnd() || !id->value.IsUint64()) { return false; }
      if (id->value.GetUint64() == find_id) {
        auto _text = tweet.FindMember("text");
        if (_text == tweet.MemberEnd() || !_text->value.IsString()) { return false; }
        text = { _text->value.GetString(), _text->value.GetStringLength() };
        return true;
      }
    }

    return false;
  }
};

class rapidjson : public rapidjson_base<kParseValidateEncodingFlag> {};
BENCHMARK_TEMPLATE(find_tweet, rapidjson);

} // namespace partial_tweets

#endif // SIMDJSON_COMPETITION_RAPIDJSON
