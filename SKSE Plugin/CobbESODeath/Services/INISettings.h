#pragma once
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

namespace CobbESODeath {
   namespace INI {
      //
      // CobbESODeath.ini should be in the "Plugins" folder with the DLL.
      //
      // There's a callback system for changes; see bottom of header file
      //
      struct INISetting;
      //
      // INI SETTING DEFINITIONS.
      //
      // The macro used here intentionally differs from the one used in the CPP file. Keep the namespaces 
      // synchronized between the two files.
      //
      #define COBB_MAKE_INI_SETTING(name, category, value) extern INISetting name;
      namespace Options {
         COBB_MAKE_INI_SETTING(bBountyPay,                    "Options", true);
         COBB_MAKE_INI_SETTING(bBountyConfiscate,             "Options", true); // confiscate stolen goods if killed while pursued by a guard? no effect if bBountyPay=false
         COBB_MAKE_INI_SETTING(bReduceSkillsOnDeath,          "Options", true); // reduce skills when killed? (uses same functionality as jail)
         COBB_MAKE_INI_SETTING(iReduceSkillSimulatedBounty,   "Options", 1000); // bounty amount to simulate when reducing skills
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
      //
      // INTERNALS BELOW.
      //
      class INISettingManager {
         private:
            typedef std::vector<INISetting*> VecSettings;
            typedef std::unordered_map<std::string, VecSettings> MapSettings;
            VecSettings settings;
            MapSettings byCategory;
            //
            struct _CategoryToBeWritten { // state object used when saving INI settings
               _CategoryToBeWritten() {};
               _CategoryToBeWritten(std::string name, std::string header) : name(name), header(header) {};
               //
               std::string name;   // name of the category, used to look up INISetting pointers for found setting names
               std::string header; // the header line, including whitespace and comments
               std::string body;   // the body
               VecSettings found;  // found settings
               //
               void Write(INISettingManager* const, std::fstream&);
            };
            //
            VecSettings& GetCategoryContents(std::string category);
            //
         public:
            static INISettingManager& GetInstance();
            //
            void Add(INISetting* setting);
            void Load();
            void Save(); // UNFINISHED -- SEE BOTTOM. IT WRITES TO A TEMPORARY FILE; WE NEED TO SWAP THE FILES AT THE END OF THE PROCESS. THEN WE NEED TO REMOVE THE CALL TO THIS FUNCTION FROM THE END OF Load.
            //
            INISetting* Get(std::string& category, std::string& name) const;
            INISetting* Get(const char*  category, const char*  name) const;
            void ListCategories(std::vector<std::string>& out) const;
      };
      struct INISetting {
         INISetting(const char* name, const char* category, bool   v) : name(name), category(category), bDefault(v), bCurrent(v), type(kType_Bool) {
            INISettingManager::GetInstance().Add(this);
         };
         INISetting(const char* name, const char* category, float  v) : name(name), category(category), fDefault(v), fCurrent(v), type(kType_Float) {
            INISettingManager::GetInstance().Add(this);
         };
         INISetting(const char* name, const char* category, int    v) : name(name), category(category), iDefault(v), iCurrent(v), type(kType_SInt) {
            INISettingManager::GetInstance().Add(this);
         };
         INISetting(const char* name, const char* category, SInt32 v) : name(name), category(category), iDefault(v), iCurrent(v), type(kType_SInt) {
            INISettingManager::GetInstance().Add(this);
         };

         enum ValueType {
            kType_Bool  = 0,
            kType_Float = 1,
            kType_SInt  = 2,
         };
         union {
            bool   bCurrent;
            float  fCurrent;
            SInt32 iCurrent;
         };
         const char* const name;
         const char* const category;
         union {
            bool   bDefault;
            float  fDefault;
            SInt32 iDefault;
         };
         const ValueType type;
         //
         std::string ToString() const;
      };
      //
      // Change callback system.
      // If (setting) is nullptr, then it's a generic change event. (For now, specific change events are not 
      // used or supported, but I want room to add them later.)
      //
      union ChangeCallbackArg {
         bool   b;
         float  f;
         SInt32 i;
         //
         ChangeCallbackArg(bool b) : b(b) {};
         ChangeCallbackArg(float f) : f(f) {};
         ChangeCallbackArg(int i) : i(i) {};
      };
      typedef void(*ChangeCallback)(INISetting* setting, ChangeCallbackArg oldValue, ChangeCallbackArg newValue);
      extern  void RegisterForChanges  (ChangeCallback c);
      extern  void UnregisterForChanges(ChangeCallback c);
      extern  void SendChangeEvent();
   };
};