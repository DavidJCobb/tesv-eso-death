#include "DefineHardcodedForms.h"
#include "ReverseEngineered/Forms/SpellItem.h"
#include "ReverseEngineered/Systems/GameData.h"
#include "skse/GameData.h"
#include "skse/SafeWrite.h"

namespace Patches {
   namespace DefineHardcodedForms {
      extern TESForm* g_cobbPlayerDeadAbility = nullptr;

      void Inner() {
         _MESSAGE("DefineHardcodedForms hook is running...");
         _MESSAGE("Does 00C0BB02 already exist? Pointer: %08X", LookupFormByID(0x00C0BB02));
         auto dataHandler = (RE::TESDataHandler*) DataHandler::GetSingleton();
         _MESSAGE("DataHandler is at %08X.", dataHandler);
         //
         RE::SpellItem* form = RE::CreateSpellItem();
         form->SetFormID(0x00C0BB02, true);
         form->SetName("CobbHardcodedDeadPlayerAbility"); // editor ID?
         CALL_MEMBER_FN(&(form->fullName.name), Set)("Cobb: Dead Player Ability");
         form->data.type = RE::SpellItem::kTypeAbility;
         CALL_MEMBER_FN(dataHandler, Subroutine0043FEC0)((RE::TESForm*) form);
         g_cobbPlayerDeadAbility = form;
         _MESSAGE("DefineHardcodedForms hook is done. Spell should exist at %08X.", form);
      };
      __declspec(naked) void Outer() {
         _asm {
            //mov  dword ptr [0x01310570], ebx; // reproduce patched-over instruction
            mov  eax, 0x01310570;      // reproduce patched-over instruction piecemeal
            mov  dword ptr [eax], ebx; // reproduce patched-over instruction piecemeal
            call Inner;
            mov  eax, 0x00440EB9;
            jmp  eax;
         };
      };
      //
      void Apply() {
         WriteRelJump(0x00440EB3, (UInt32)&Outer);
      };
   };
};