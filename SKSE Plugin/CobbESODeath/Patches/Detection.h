#pragma once
#include <mutex>
#include <set>

namespace RE {
   class Actor;
};
//
class DetectionInterceptor { // Make actors impossible to detect, or make them incapable of detecting anyone else
   public:
      static DetectionInterceptor& GetInstance() {
         static DetectionInterceptor instance;
         return instance;
      };
   protected:
      typedef UInt32 RefHandle;
      class RefHandleList : public std::set<RefHandle> {
         public:
            bool has_actor(RE::Actor*) const;
            void edit_actor(RefHandle, bool state);
      };
      //
      RefHandleList forceUnseen;
      RefHandleList forceUnseeing;
      std::recursive_mutex lock;
      //
      static void _EditList  (RefHandleList&, RefHandle, bool state);
      static bool _SearchList(const RefHandleList&, RE::Actor*);
   public:
      void SetActorUnseen(RE::Actor*, bool shouldBlind);
      void SetActorUnseeing(RE::Actor*, bool shouldBeUndetectable);
      bool IsActorUnseen(RE::Actor*);
      bool IsActorUnseeing(RE::Actor*);
      void GetActorStatus(RE::Actor*, bool& outUnseen, bool& outUnseeing);
      bool ShouldCancelDetection(RE::Actor* seeker, RE::Actor* target);
      void Reset();
};

namespace Patches {
   namespace Detection {
      void Apply();
   };
};