#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BCAnimInstance.generated.h"

/*  Animation Instance class to parent our character's Anim BP from */
UCLASS()
class MP_COURSE_API UBCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	


private:

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess="true"))
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bIsAccellerating;
	
};
