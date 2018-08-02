#pragma once
#include <mutex> using namespace std;
#include <set> using namespace std;

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
      typedef UInt32              RefHandle;
      typedef std::set<RefHandle> RefHandleList;
      //
      RefHandleList forceUnseen;
      RefHandleList forceUnseeing;
      std::recursive_mutex lock;
      //
      void _EditList  (RefHandleList&, RefHandle, bool state);
      bool _SearchList(const RefHandleList&, RE::Actor*) const;
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