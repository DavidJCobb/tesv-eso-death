#pragma once
#include <vector> using namespace std;
#include "ReverseEngineered/Forms/Actor.h"

namespace Patches {
   namespace DeathIntercept {
      extern void(*g_deathInterceptHandler)(RE::Actor* dead);
      extern void(*g_killmoveDoneHandler)(RE::Actor* dead);

      void Apply();
   };
};