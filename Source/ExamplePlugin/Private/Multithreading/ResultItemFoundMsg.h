#pragma once
#include "Misc/TVariant.h"
#include "Configuration.h"
#include "ResultItemFoundMsg.generated.h"
USTRUCT()
struct FResultItemFoundMsg
{
	GENERATED_USTRUCT_BODY()

	FResultItemFoundMsg() = default;

	FResultItemFoundMsg(bool bIsFinished, RequiredType Value)
		: bIsFinished(bIsFinished),
		  Storage(TInPlaceType<RequiredType>(), MoveTemp(Value))
	{
	}

	FResultItemFoundMsg(bool bIsFinished, TArray<RequiredType>&& Values)
		: bIsFinished(bIsFinished),
		  Storage(TInPlaceType<TArray<RequiredType>>(), MoveTemp(Values))
	{
	}

	bool bIsFinished = true;
	TVariant<FEmptyVariantState, RequiredType, TArray<RequiredType>> Storage;
};
