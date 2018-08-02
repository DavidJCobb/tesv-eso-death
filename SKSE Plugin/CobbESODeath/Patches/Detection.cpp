#include "Detection.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "skse/SafeWrite.h"

void DetectionInterceptor::_EditList(RefHandleList& list, RefHandle handle, bool state) {
   if (handle) {
      if (state) {
         list.insert(handle);
      } else {
         list.erase(list.find(handle));
      }
   }
}
bool DetectionInterceptor::_SearchList(const RefHandleList& list, RE::Actor* subject) const {
//size_t i = 0;
   for (auto it = list.begin(); it != list.end(); ++it) {
//_MESSAGE("_SearchList iterating over element %d; iterator is %08X; end is %08X", i, it, list.end());
//i++;
      UInt32     handle = (*it);
      RE::Actor* ref = nullptr;
      LookupREFRByHandle(&handle, (TESObjectREFR**)&ref);
      if (ref) {
         ref->handleRefObject.DecRefHandle(); // LookupREFRByHandle incremented the refcount
         if (ref == subject)
            return true;
         ref = nullptr; // if ref is non-null, LookupREFRByHandle will decrement its refcount (which we can't rely on because it wouldn't run after the loop)
      }
   }
   return false;
};
void DetectionInterceptor::SetActorUnseen(RE::Actor* subject, bool state) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   //
   UInt32 handle = *g_invalidRefHandle;
   CreateRefHandleByREFR(&handle, (TESObjectREFR*)subject);
   this->_EditList(this->forceUnseen, handle, state);
//_MESSAGE("Attempted to hide actor %08X from view.", subject->formID);
}
void DetectionInterceptor::SetActorUnseeing(RE::Actor* subject, bool state) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   //
   UInt32 handle = *g_invalidRefHandle;
   CreateRefHandleByREFR(&handle, (TESObjectREFR*)subject);
   this->_EditList(this->forceUnseeing, handle, state);
//_MESSAGE("Attempted to blind actor %08X.", subject->formID);
}
bool DetectionInterceptor::IsActorUnseen(RE::Actor* subject) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   return this->_SearchList(this->forceUnseen, subject);
};
bool DetectionInterceptor::IsActorUnseeing(RE::Actor* subject) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   return this->_SearchList(this->forceUnseeing, subject);
};
void DetectionInterceptor::GetActorStatus(RE::Actor* subject, bool& outUnseen, bool& outUnseeing) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   outUnseen   = this->_SearchList(this->forceUnseen, subject);
   outUnseeing = this->_SearchList(this->forceUnseeing, subject);
}
__declspec(noinline) bool DetectionInterceptor::ShouldCancelDetection(RE::Actor* seeker, RE::Actor* target) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   if (this->_SearchList(this->forceUnseeing, seeker))
      return true;
   if (this->_SearchList(this->forceUnseen, target))
      return true;
   return false;
}
__declspec(noinline) void DetectionInterceptor::Reset() {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   this->forceUnseeing.clear();
   this->forceUnseen.clear();
}

namespace Patches {
   namespace Detection {
      bool Inner(RE::Actor* seeker, RE::Actor* target) {
         return DetectionInterceptor::GetInstance().ShouldCancelDetection(seeker, target);
      };
      __declspec(naked) void Outer() {
         _asm {
            mov  edx, dword ptr [ebp + 0x8]; // edx == Arg1
            mov  eax, dword ptr [ebp + 0xC]; // eax = Arg2;
            push eax;
            push edx;
            call Inner;
            add  esp, 8;
            test al, al;
            jnz  lAbort;
            mov  eax, 0x01B11820;      // reproduce patched-over instruction piecemeal // eax = GMST:fSneakBaseValue;
            mov  eax, dword ptr [eax]; // reproduce patched-over instruction piecemeal
            mov  ecx, 0x0059897E;
            jmp  ecx;
         lAbort:
            mov  eax, -0x3E8; // -1000
            mov  ecx, 0x00598DA8;
            jmp  ecx;
         };
      };
      void Apply() {
         WriteRelJump(0x00598979, (UInt32)&Outer);
      };
   };
};