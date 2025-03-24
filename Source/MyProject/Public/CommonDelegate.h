#pragma once
#include "CoreMinimal.h"

class UC_PickUp;
class AMyPlayerController;

DECLARE_MULTICAST_DELEGATE_ThreeParams( FOnKillOccurred , AMyPlayerController* , AMyPlayerController* , UC_PickUp* );
