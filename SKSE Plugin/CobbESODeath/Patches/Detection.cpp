#include "Detection.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "skse/SafeWrite.h"
#ifdef COBB_DETECTION_INTERCEPTOR_USES_VECTOR
   #include <algorithm>
#endif

bool DetectionInterceptor::RefHandleList::has_actor(RE::Actor* subject) const {
   /*//
   for (auto it = this->begin(); it != this->end(); ++it) {
      RE::refr_ptr ref(*it);
      if (ref && ref == subject)
         return true;
   }
   return false;
   //*/
   //
   UInt32 handle;
   CreateRefHandleByREFR(&handle, (TESObjectREFR*)subject);
   if (handle != *g_invalidRefHandle)
      return this->find(handle) != this->end();
   //*/
   return false;
};
void DetectionInterceptor::RefHandleList::edit_actor(RefHandle handle, bool state) {
   if (handle) {
      if (state)
         this->insert(handle);
      else {
         auto found = this->find(handle);
         if (found != this->end())
            this->erase(found);
      }
   }
};
void DetectionInterceptor::SetActorUnseen(RE::Actor* subject, bool state) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   //
   UInt32 handle = *g_invalidRefHandle;
   CreateRefHandleByREFR(&handle, (TESObjectREFR*)subject);
   /*DetectionInterceptor::_EditList(this->forceUnseen, handle, state);*/
   this->forceUnseen.edit_actor(handle, state);
//_MESSAGE("Attempted to hide actor %08X from view.", subject->formID);
}
void DetectionInterceptor::SetActorUnseeing(RE::Actor* subject, bool state) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   //
   UInt32 handle = *g_invalidRefHandle;
   CreateRefHandleByREFR(&handle, (TESObjectREFR*)subject);
   /*DetectionInterceptor::_EditList(this->forceUnseeing, handle, state);*/
   this->forceUnseeing.edit_actor(handle, state);
//_MESSAGE("Attempted to blind actor %08X.", subject->formID);
}
bool DetectionInterceptor::IsActorUnseen(RE::Actor* subject) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   /*return DetectionInterceptor::_SearchList(this->forceUnseen, subject);*/
   return this->forceUnseen.has_actor(subject);
};
bool DetectionInterceptor::IsActorUnseeing(RE::Actor* subject) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   /*return DetectionInterceptor::_SearchList(this->forceUnseeing, subject);*/
   return this->forceUnseeing.has_actor(subject);
};
void DetectionInterceptor::GetActorStatus(RE::Actor* subject, bool& outUnseen, bool& outUnseeing) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   /*outUnseen   = DetectionInterceptor::_SearchList(this->forceUnseen, subject);
   outUnseeing = DetectionInterceptor::_SearchList(this->forceUnseeing, subject);*/
   outUnseen   = this->forceUnseen.has_actor(subject);
   outUnseeing = this->forceUnseeing.has_actor(subject);
}
__declspec(noinline) bool DetectionInterceptor::ShouldCancelDetection(RE::Actor* seeker, RE::Actor* target) {
   std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
   /*if (DetectionInterceptor::_SearchList(this->forceUnseeing, seeker))
      return true;
   if (DetectionInterceptor::_SearchList(this->forceUnseen, target))
      return true;*/
   if (this->forceUnseeing.has_actor(seeker))
      return true;
   if (this->forceUnseen.has_actor(target))
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
            mov  edx, dword ptr [ebp + 0x8]; // edx = Arg1;
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