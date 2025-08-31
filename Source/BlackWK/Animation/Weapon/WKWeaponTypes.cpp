#include "WKWeaponTypes.h"

TAutoConsoleVariable<float> FWKWeaponConsoleVariables::CVarDebugMeleeCollision(
	TEXT("wk.DebugMeleeCollision"),
	0,
	TEXT("Display duration of melee collision shapes.\n")
	TEXT("0: do not display \n"),
	ECVF_Default);