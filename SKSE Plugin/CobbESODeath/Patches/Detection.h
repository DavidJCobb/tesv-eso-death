#pragma once
#include <mutex>
//#define COBB_DETECTION_INTERCEPTOR_USES_VECTOR 1
#ifdef COBB_DETECTION_INTERCEPTOR_USES_VECTOR
   //
   // I wanted to be able to easily switch the container used for the 
   // list of actor handles we're working with.
   //
   // The problem we're dealing with here is that if we pass a std::set 
   // to a function by reference -- a perfectly reasonable thing to do 
   // -- and then try to iterate over it, the iterator sometimes goes 
   // bad. Like, in _SearchList, we end up with cases where (it == 3). 
   // Obviously 3 is not a valid pointer, so we crash.
   //
   // It's pure 100% luck of the draw, but after some stress-testing, 
   // I haven't had any crashes with std::vector.
   //
   #include <vector>
#else
   #include <set>
#endif

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
      #ifdef COBB_DETECTION_INTERCEPTOR_USES_VECTOR
         typedef std::vector<RefHandle> RefHandleList;
      #else
         typedef std::set<RefHandle> RefHandleList;
      #endif
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