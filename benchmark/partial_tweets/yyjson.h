#pragma once

#ifdef SIMDJSON_COMPETITION_YYJSON

#include "partial_tweets.h"

namespace partial_tweets {

class yyjson {
  simdjson_really_inline std::string_view get_string_view(yyjson_val *obj, std::string_view key) {
    auto val = yyjson_obj_getn(obj, key.data(), key.length());
    if (!yyjson_is_str(val)) { throw "field is not uint64 or null!"; }
    return { yyjson_get_str(val), yyjson_get_len(val) };
  }
  simdjson_really_inline uint64_t get_uint64(yyjson_val *obj, std::string_view key) {
    auto val = yyjson_obj_getn(obj, key.data(), key.length());
    if (!yyjson_is_uint(val)) { throw "field is not uint64 or null!"; }
    return yyjson_get_uint(val);
  }
  simdjson_really_inline uint64_t get_nullable_uint64(yyjson_val *obj, std::string_view key) {
    auto val = yyjson_obj_getn(obj, key.data(), key.length());
    if (!yyjson_is_uint(val)) { }
    auto type = yyjson_get_type(val);
    if (type != YYJSON_TYPE_NUM && type != YYJSON_TYPE_NULL ) { throw "field is not uint64 or null!"; }
    return yyjson_get_uint(val);
  }
  simdjson_really_inline partial_tweets::twitter_user get_user(yyjson_val *obj, std::string_view key) {
    auto user = yyjson_obj_getn(obj, key.data(), key.length());
    if (!yyjson_is_obj(user)) { throw "missing twitter user field!"; }
    return { get_uint64(user, "id"), get_string_view(user, "screen_name") };
  }

public:
  bool run(const padded_string &json, std::vector<tweet> &tweets) {
    // Walk the document, parsing the tweets as we go
    yyjson_doc *doc = yyjson_read(json.data(), json.size(), 0);
    if (!doc) { return false; }
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) { return false; }
    yyjson_val *statuses = yyjson_obj_get(root, "statuses");
    if (!yyjson_is_arr(statuses)) { return "Statuses is not an array!"; }

    size_t tweet_idx, tweets_max;
    yyjson_val *tweet;
    yyjson_arr_foreach(statuses, tweet_idx, tweets_max, tweet) {
      if (!yyjson_is_obj(tweet)) { return false; }
      // TODO these can't actually handle errors
      tweets.emplace_back(partial_tweets::tweet{
        get_string_view(tweet, "created_at"),
        get_uint64     (tweet, "id"),
        get_string_view(tweet, "text"),
        get_nullable_uint64     (tweet, "in_reply_to_status_id"),
        get_user       (tweet, "user"),
        get_uint64     (tweet, "retweet_count"),
        get_uint64     (tweet, "favorite_count")
      });
    }

    return true;
  }
};

BENCHMARK_TEMPLATE(partial_tweets, yyjson);

} // namespace partial_tweets

#endif // SIMDJSON_COMPETITION_YYJSON

