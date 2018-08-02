#pragma once
#include "skse/GameForms.h"
#include "TESForm.h"

namespace RE {
   class SpellItem : public MagicItem {
      public:
         enum { kTypeID = kFormType_Spell };
         enum {
            kTypeSpell = 0,
            kTypeDisease,
            kTypePower,
            kTypeLesserPower,
            kTypeAbility,
            kTypePoison,
            kTypeAddition,
            kTypeVoice
         };
         //
         // Parents:
         BGSEquipType         equipType;   // 50
         BGSMenuDisplayObject dispObj;     // 58
         TESDescription       description; // 60

         // Members:
         struct Data { // sizeof == 0x24
            struct Data0 {
               UInt32 cost;  // 00
               UInt32 flags; // 04
            };
            //
            Data0  unk00;    // 00
            UInt32 type;     // 08
            float  castTime; // 0C
            UInt32 castType; // 10
            UInt32 unk14; // 14
            UInt32 unk18; // 18
            UInt32 unk1C; // 1C
            BGSPerk* spellPerk; // 20
         };
         Data data; // 6C
         //
         UInt32 GetMagickaCost() { return data.unk00.cost; }
   };
   DEFINE_SUBROUTINE(SpellItem*, CreateSpellItem, 0x00409F50); // allocates and constructs
};
