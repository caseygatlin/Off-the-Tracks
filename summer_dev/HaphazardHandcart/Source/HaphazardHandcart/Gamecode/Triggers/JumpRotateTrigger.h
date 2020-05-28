#pragma once

#include "CoreMinimal.h"
#include "Gamecode/Triggers/JumpTrigger.h"
#include "JumpRotateTrigger.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API AJumpRotateTrigger : public AJumpTrigger
{
	GENERATED_BODY()

public:
	virtual void CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate Settings")
		// Rotate speed.
		float m_RotateSpeed = 100;
};
