#include "INISettings.h"
#include "Miscellaneous/strings.h"
#include <algorithm>
#include <fstream>
#include "skse/Utilities.h"

//#include "Shlwapi.h" // for PathFileExists // doesn't work anyway; link error

namespace CobbESODeath {
   namespace INI {
      const std::wstring& _GetPath() {
         static std::wstring out;
         if (!out.empty())
            return out;
         //
         // We can't use SKSE's GetRuntimeDirectory function because that gets the "narrow" path 
         // (i.e. for the "A"-suffixed Windows APIs), and we're using "wide" paths here. I can't 
         // find any material on the internet indicating whether these are interchangeable (i.e. 
         // whether all I need to do to convert from narrow to wide is literally widen the chars) 
         // so I'm just gonna get a W path from scratch, roughly the same way SKSE does.
         //
         wchar_t  buffer[MAX_PATH];
         uint32_t length = GetModuleFileNameW(GetModuleHandle(NULL), buffer, MAX_PATH); // writes TESV.exe path to (buffer); stores number of chars in (length)
         if (length && length < MAX_PATH) {
            std::wstring working(buffer, length);
            size_t last = working.rfind('\\');
            if (last != std::string::npos)
               working = working.substr(0, last + 1);
            std::swap(out, working);
         }
         out += L"Data\\SKSE\\Plugins\\CobbESODeath.ini";
         return out;
         /*// SKSE-based code:
         static std::string path;
         if (path.empty()) {
            std::string runtimePath = GetRuntimeDirectory();
            if (!runtimePath.empty()) {
               path = runtimePath;
               path += "Data\\SKSE\\Plugins\\CobbESODeath.ini";
            }
         }
         return path;
         */
      };
      extern cobb::ini::file& get() {
         static cobb::ini::file instance = cobb::ini::file(_GetPath().c_str());
         return instance;
      }
      //
      // INI SETTING DEFINITIONS.
      //
      // The macro used here intentionally differs from the one used in the H file. Keep the namespaces 
      // synchronized between the two files.
      //
      #define COBB_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name = cobb::ini::setting(get, #name, category, value);
      namespace Options {
         COBB_MAKE_INI_SETTING(bBountyPay,                    "Options", true);
         COBB_MAKE_INI_SETTING(bBountyConfiscate,             "Options", true);
         COBB_MAKE_INI_SETTING(bReduceSkillsOnDeath,          "Options", true);
         COBB_MAKE_INI_SETTING(iReduceSkillSimulatedBounty,   "Options", 1000);
         COBB_MAKE_INI_SETTING(bKillIfHardRequirementsNotMet, "Options", true);
      };
      namespace ResurrectOffsite {
         COBB_MAKE_INI_SETTING(bEnabled,           "ResurrectOffsite", true);
         COBB_MAKE_INI_SETTING(bPreemptsDeath,     "ResurrectOffsite", true);
         COBB_MAKE_INI_SETTING(bRequireSettlement, "ResurrectOffsite", true);
      };
      namespace SoulGem {
         COBB_MAKE_INI_SETTING(bEnabled,               "SoulGem", true);
         COBB_MAKE_INI_SETTING(bAllowReusableGems,     "SoulGem", true);
         COBB_MAKE_INI_SETTING(bAlwaysLetPlayerChoose, "SoulGem", true);
         COBB_MAKE_INI_SETTING(bGemPreemptsTravel,     "SoulGem", true);
         COBB_MAKE_INI_SETTING(iMinimumSoulSize,       "SoulGem", 1);
      };
      #undef COBB_MAKE_INI_SETTING
   }
};