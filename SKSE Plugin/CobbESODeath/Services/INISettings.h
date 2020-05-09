#pragma once
#include "../Miscellaneous/ini.h"
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

namespace CobbESODeath {
   namespace INI {
      extern cobb::ini::file& get();
      //
      // CobbESODeath.ini should be in the "Plugins" folder with the DLL.
      //
      // INI SETTING DEFINITIONS.
      //
      // The macro used here intentionally differs from the one used in the CPP file. Keep the namespaces 
      // synchronized between the two files.
      //
      #define COBB_MAKE_INI_SETTING(name, category, value) extern cobb::ini::setting name;
      namespace Options {
         COBB_MAKE_INI_SETTING(bBountyPay,                    "Options", true);
         COBB_MAKE_INI_SETTING(bBountyConfiscate,             "Options", true); // confiscate stolen goods if killed while pursued by a guard? no effect if bBountyPay=false
         COBB_MAKE_INI_SETTING(bReduceSkillsOnDeath,          "Options", true); // reduce skills when killed? (uses same functionality as jail)
         COBB_MAKE_INI_SETTING(iReduceSkillSimulatedBounty,   "Options", (uint32_t)1000); // bounty amount to simulate when reducing skills
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
         COBB_MAKE_INI_SETTING(iMinimumSoulSize,       "SoulGem", (uint32_t)1);
      };
      #undef COBB_MAKE_INI_SETTING
   };
};