#include "DeathIntercept.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "skse/SafeWrite.h"

namespace Patches {
   namespace DeathIntercept {
      extern void(*g_deathInterceptHandler)(RE::Actor* dead) = nullptr;
      extern void(*g_killmoveDoneHandler)(RE::Actor* dead) = nullptr;

      __declspec(naked) bool ShouldBeImmortal(RE::Actor* subject) {
         _asm {
            mov  eax, 0x01310588; // addr found in reverse-engineering
            mov  eax, dword ptr [eax];
            cmp  eax, dword ptr [esp + 0x4];
            sete al;
            retn;
         };
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
               push esi;
               call ShouldBeImmortal;
               add  esp, 4;
               test al, al;
               jz   lNormal;
               push esi;
               call Intercept;
               add  esp, 4;
               pop  esi;
               retn 0x10;
            lNormal:
               mov  ecx, dword ptr [esi + 0x88]; // ecx = this->processManager; // reproduce patched-over instruction
               mov  eax, 0x006AC3A9;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x006AC3A3, (UInt32)&Hook); // Actor::Kill
            SafeWrite8  (0x006AC3A8, 0x90); // NOP
         };
      };
      namespace PreKill { // maybe unnecessary?
         __declspec(naked) void Hook() {
            _asm {
               setne cl; // preserve comparison // cl = (INI:GamePlay:bEssentialTakeNoDamage == true);
               push  esi;
               call  ShouldBeImmortal;
               add   esp, 4;
               test  al, al;
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
               push esi;
               call ShouldBeImmortal;
               add  esp, 4;
               test al, al;
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