#include "Shared.h"

#include "skse/GameAPI.h"

namespace cobb {
   float degrees_to_radians(float degrees) {
      if (!degrees)
         return 0.0;
      return degrees * cobb::pi / 180.0;
   }
   float radians_to_degrees(float radians) {
      if (!radians)
         return 0.0;
      return radians * 180.0 / cobb::pi;
   }
   std::string stringf(const char* format, ...) {
      va_list args;
      va_start(args, format);
      //
      std::string result;
      //
      char buffer[32];
      size_t count = std::vsnprintf(buffer, sizeof(buffer), format, args);
      if (count < sizeof(buffer)) {
         result = buffer;
      } else {
         char* buffer = (char*) malloc(count + 1);
         std::vsnprintf(buffer, count + 1, format, args);
         result = buffer;
         free(buffer);
      }
      //
      va_end(args);
      return result;
   }

   namespace skyrim {
      __declspec(naked) void dispose(void*, bool) {
         __asm {
            mov ecx, [esp + 0x8];
            push ecx;
            mov ecx, [esp + 0x4];
            mov eax, [ecx];
            mov eax, [eax];
            call eax; // Arg1->Unk_00(Arg2);
            retn 8;
         };
      };
      void* heap_alloc(UInt32 size) {
         void* memory = FormHeap_Allocate(size);
         memset(memory, 0, size);
         return memory;
      };
      void* heap_create_by_vtbl(UInt32 size, UInt32 vtbl) {
         void* memory = FormHeap_Allocate(size);
         memset(memory, 0, size);
         ((UInt32*) memory)[0] = vtbl;
         return memory;
      };
      /*template<class T> T* heap_create_struct() { // usage causes linker errors
         T* memory = (T*) FormHeap_Allocate(sizeof(T));
         memset(memory, 0, sizeof(T));
         return memory;
      };*/
   };
};

float DegreesToRadians(float aValue) {
   return aValue * PI / 180;
}
float RadiansToDegrees(float aValue) {
   return aValue * 180 / PI;
}

/*
namespace BhkCollisionObjectIteration {
   void _GetMotionType(bhkCollisionObject* collision, ParametersGetMT* parameters) {
      UInt8 current = collision->unk10->unk08->motionSystem; // VS is compiling this as collision->unk0C->unk08, i.e. it doesn't know that collision inherits an unk04 from a superclass
      if (parameters->result == -1) {
         parameters->result = current;
      } else if (parameters->result != 0 && parameters->result != current) {
         parameters->result = 0;
      }
   };
   void _SetMotionType(bhkCollisionObject* collision, ParametersSetMT* parameters) {
      collision->SetMotionSystem(parameters->motionType, 0, parameters->unknown);
   };

   SInt8 GetMotionTypeOnNodeTree(NiNode* root) {
      //
      // Returns -1 if there are no collision objects, and 0 if there are multiple collision objects with inconsistent motion types.
      // Returns the motion type on all collision objects, otherwise.
      //
      ParametersGetMT parameters;
      parameters.result = -1;
      IterateOverBhkCollisionObjects(root, (Parameters*)&parameters, (IteratorFunction*)&_GetMotionType);
      return parameters.result;
   };
   void SetMotionTypeOnNodeTree(NiNode* root, UInt8 motionType) {
      ParametersSetMT parameters;
      parameters.motionType = motionType;
      parameters.unknown = false; // until we know what this is
      IterateOverBhkCollisionObjects(root, &parameters, (IteratorFunction*)&_SetMotionType);
   };
}
*/