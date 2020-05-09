#pragma once
#include <cstdint>
#include <string>

namespace cobb {
   extern void sprintf(std::string& out, const char* format, ...);
   extern int  strieq(const std::string& a, const std::string& b);

   extern bool string_has_content(const char* str); // any characters besides '\0' and std::isspace-positive glyphs
   extern bool string_says_false(const char* str); // the string spells the case-insensitive word "false", ignoring whitespace
   extern bool string_to_int(const char* str, int32_t& out, bool allowHexOrDecimal = false); // returns true if it's a valid integer and no non-whitespace follows the number; out is not modified otherwise
   extern bool string_to_int(const char* str, uint32_t& out, bool allowHexOrDecimal = false);
   extern bool string_to_float(const char* str, float& out); // returns true if it's a valid float   and no non-whitespace follows the number; out is not modified otherwise
   //
   extern void replace_all_of(std::string& subject, const std::string& token, const std::string& target);
   extern std::string& ltrim(std::string& subject);
   extern std::string& rtrim(std::string& subject);
   extern std::string& trim(std::string& subject);
   extern void snprintf(std::string& out, const char* format, ...);
   extern SInt32 stricmp(const std::string& a, const std::string& b);
   extern bool striendswith(const std::string& haystack, const std::string& needle);

   //
   // Given a format like "example %s %s %d", the valid tokens would be "ssd". For 
   // "example %.*s" or "example %*d" the tokens would be "*s". Note that format 
   // tokens with multiple flags (e.g. "%- d") are not supported, unless the last 
   // flag is zero (in which case it's taken as part of the width field e.g. "%-0d").
   //
   extern __declspec(noinline) bool validate_format(const std::string& format, const std::string& tokens);
};