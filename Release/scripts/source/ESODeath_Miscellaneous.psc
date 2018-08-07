Scriptname ESODeath_Miscellaneous Hidden

Int Function CanChooseToUseSoulGem       () Global Native ; treat the result as a bool
Int Function GemPreemptsTravel           () Global Native ; treat the result as a bool
Int Function HasUsableSoulGem            (Actor akActor) Global Native ; treat the result as a bool
Int Function SoulGemFunctionalityEnabled () Global Native ; treat the result as a bool
Int Function TryConsumeSoulGem           (Actor akActor) Global Native ; treat the result as a success bool

ObjectReference Function GetNearestMapMarkerTo (ObjectReference akSubject, Int aiMinimumStatus) Global Native ; statuses: 0, 1, 2 == any, discovered, travelable
Int Function ResurrectOffsiteEnabled () Global Native ; treat the result as a bool

;
; Tests to see if we can tell when a script object's underlying form/etc. is 
; deleted out from under it:
;
Int Function FormExists   (Form akForm)                Global Native ; treat the result as a bool
Int Function EffectExists (ActiveMagicEffect akEffect) Global Native ; treat the result as a bool