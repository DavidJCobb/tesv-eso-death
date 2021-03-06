#include "DeathIntercept.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "skse/SafeWrite.h"

namespace Patches {
   namespace DeathIntercept {
      extern bool(*g_deathQueryHandler)(RE::Actor* dead) = nullptr;
      extern void(*g_deathInterceptHandler)(RE::Actor* dead) = nullptr;
      extern void(*g_killmoveDoneHandler)(RE::Actor* dead) = nullptr;

      bool ShouldBeImmortal(RE::Actor* subject) {
         if (subject != *(RE::Actor**)0x01310588)
            return false;
         if (g_deathQueryHandler)
            return g_deathQueryHandler(subject);
         return true;
      };

      namespace Kill {
         //
         // The code that we patch here handles the bulk of the work involved in killing 
         // an actor.
         //
         void Intercept(RE::Actor* dead) {
            if (!dead)
               return;
            _MESSAGE("Intercepted the death of Actor %08X.", dead->formID);
            if (g_deathInterceptHandler)
               g_deathInterceptHandler(dead);
         };
         __declspec(naked) void Hook() {
            _asm {
               pushad; // protect ecx
               push esi;
               call ShouldBeImmortal;
               add  esp, 4;
               test  al, al;
               popad; // restore ecx
               jnz  lIntercept;
               test ecx, ecx;
               jz   lVanilla_HasNoAI;
            lVanilla_HasAI:
               mov  eax, 0x0071F770; // reproduce patched-over call
               call eax;
               mov  ecx, 0x006AC3B2;
               jmp  ecx;
            lVanilla_HasNoAI:
               mov  eax, 0x006AC3B6;
               jmp  eax;
               //
            lIntercept:
               push esi;
               call Intercept;
               add  esp, 4; // clean up after last non-member call
               //
               // vanilla return:
               //
               pop  esi;
               retn 0x10;
            };
         };
         void Apply() {
            WriteRelJump(0x006AC3A9, (UInt32)&Hook); // Actor::Kill
            SafeWrite32 (0x006AC3AE, 0x90909090); // NOP
         };
      };
      namespace PreKill { // maybe unnecessary?
         __declspec(naked) void Hook() {
            _asm {
               setne cl; // preserve comparison // cl = (INI:GamePlay:bEssentialTakeNoDamage == true);
               pushad;
               push  esi;
               call  ShouldBeImmortal;
               add   esp, 4;
               test  al, al;
               popad;
               jnz   lExit;
               test  cl, cl;
               jz    lEssentialDisabled;
               mov   eax, dword ptr [esi + 0x138]; // reproduce patched-over instrutcion
               mov   ecx, 0x006D2C18;
               jmp   ecx;
            lExit:
               mov  eax, 0x006D2FEF; // return
               jmp  eax;
            lEssentialDisabled:
               mov  eax, 0x006D2C34;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x006D2C10, (UInt32)&Hook);
            SafeWrite16 (0x006D2C15, 0x9090); // NOP
            SafeWrite8  (0x006D2C17, 0x90); // NOP
         };
      };
      namespace ActorVirtual127 { // maybe unnecessary?
         __declspec(naked) void Hook() {
            _asm {
               push esi;
               call ShouldBeImmortal;
               add  esp, 4;
               test al, al;
               jz   lNormalHandling;
               mov  ecx, esi;
               call RE::Actor::IsEssential;
               test al, al;
               jnz  lNormalHandling;
               mov  eax, 0x006DFC7F;
               jmp  eax;
            lNormalHandling:
               mov  eax, dword ptr [esi + 0x60]; // reproduce patched-over instruction
               mov  edx, dword ptr [eax + 0x04]; // reproduce patched-over instruction
               mov  eax, 0x006DFC07;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x006DFC01, (UInt32)&Hook);
            SafeWrite8  (0x006DFC06, 0x90); // NOP
         };
      };
      namespace Decapitate {
         //
         // Refuse to allow decapitation of an actor we're protecting, since decapitation 
         // seems irreversible. In this case, we shim the call that checks whether the 
         // actor has already been decapitated.
         //
         __declspec(naked) void __stdcall Shim() {
            _asm {
               pushad;
               push esi;
               call ShouldBeImmortal;
               add  esp, 4;
               test al, al;
               popad;
               jnz  lExit;
               mov  eax, 0x004D5B90; // return this->CheckDismembermentBit(Arg1);
               jmp  eax;
            lExit:
               retn 4;
            };
         };
         void Apply() {
            WriteRelCall(0x006AA565, (UInt32)&Shim);
         };
      };
      //
      namespace KillmoveStart { // not currently needed; commented out in Apply as of this writing
         //
         // Hook Actor::OnKillmoveStart. Note that the function is called on 
         // both the killed actor (with the killer as an argument) and on the 
         // killer (with a nullptr argument).
         //
         void Inner() {
            _MESSAGE("A killmove somewhere just started...");
         };
         __declspec(naked) void Outer() {
            _asm {
               pushad;
               call Inner;
               popad;
               mov  eax, 0x006AE350; // shimmed call to Actor::GetProcessManagerUnk9B
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelCall(0x006E3C23, (UInt32)&Outer);
         };
      };
      namespace KillmoveDone {
         //
         // Hook Actor::OnKillmoveDone.
         //
         void _stdcall Inner(RE::Actor* subject) {
            if (g_killmoveDoneHandler && ShouldBeImmortal(subject)) {
               _MESSAGE("A killmove on the player has completed.");
               g_killmoveDoneHandler(subject);
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               and  dword ptr [esi + 0x138], 0xFFFFBFFF; // reproduce patched-over instruction
               push esi;
               call Inner; // stdcall
               mov  eax, 0x006E3D23;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x006E3D19, (UInt32)&Outer);
            SafeWrite32 (0x006E3D1E, 0x90909090);
            SafeWrite8  (0x006E3D22, 0x90);
         };
      };

      void Apply() {
         Kill::Apply();
         PreKill::Apply();
         ActorVirtual127::Apply();
         Decapitate::Apply();
         //
         //KillmoveStart::Apply();
         KillmoveDone::Apply();
      };
   };
};