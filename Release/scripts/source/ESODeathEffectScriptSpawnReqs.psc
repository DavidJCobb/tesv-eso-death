Scriptname ESODeathEffectScriptSpawnReqs extends ActiveMagicEffect Hidden 

Message Property CobbESODeathMenuChooseSpawnMethod Auto
Message Property CobbESODeathMessageForcingTravel Auto

Spell Property pkSpell Auto

Event OnEffectStart(Actor akTarget, Actor akCaster)
   ObjectReference kTravelTo = ESODeath_Miscellaneous.GetNearestMapMarkerTo(akTarget, 2)
   If !kTravelTo
      kTravelTo = ESODeath_Miscellaneous.GetNearestMapMarkerTo(akTarget, 1) ; allow undiscovered if the player somehow hasn't discovered anything in this worldspace
   EndIf
   Bool bTravel = True
   Bool bSuppressTravelMessage = False
   If ESODeath_Miscellaneous.SoulGemFunctionalityEnabled() &&  ESODeath_Miscellaneous.HasUsableSoulGem(akTarget)
      If ESODeath_Miscellaneous.GemPreemptsTravel()
         If kTravelTo && CobbESODeathMenuChooseSpawnMethod && ESODeath_Miscellaneous.CanChooseToUseSoulGem()
            bTravel = CobbESODeathMenuChooseSpawnMethod.Show() as Bool
            bSuppressTravelMessage = True
         Else
            bTravel = False
         EndIf
         If !bTravel
            ESODeath_Miscellaneous.TryConsumeSoulGem(akTarget)
         EndIf
      Else
         ESODeath_Miscellaneous.TryConsumeSoulGem(akTarget)
         bSuppressTravelMessage = True
      EndIf
   EndIf
   If bTravel && ESODeath_Miscellaneous.ResurrectOffsiteEnabled()
      Debug.Trace("ATTEMPTING TO RESPAWN PLAYER AT: " + kTravelTo)
      If kTravelTo
         If !bSuppressTravelMessage
            CobbESODeathMessageForcingTravel.Show()
         EndIf
         akTarget.ForceRemoveRagdollFromWorld() ; ragdolling breaks FastTravel and MoveTo
         Game.FastTravel(kTravelTo)
         akTarget.MoveTo(kTravelTo)
         akTarget.DispelSpell(pkSpell)
      EndIf
   EndIf
EndEvent