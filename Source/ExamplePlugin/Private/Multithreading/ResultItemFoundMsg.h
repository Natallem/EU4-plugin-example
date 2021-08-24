#pragma once
#include "Misc/TVariant.h"
#include "ResultItemFoundMsg.generated.h"

class ISearchableItem;

USTRUCT()
struct FResultItemFoundMsg
{
	GENERATED_USTRUCT_BODY()

	FResultItemFoundMsg() = default;

	FResultItemFoundMsg(bool bIsFinished, TSharedRef<ISearchableItem> Value)
		: bIsFinished(bIsFinished),
		  Storage(TInPlaceType<TSharedRef<ISearchableItem>>(), MoveTemp(Value))
	{
	}

	FResultItemFoundMsg(bool bIsFinished, TArray<TSharedRef<ISearchableItem>>&& Values)
		: bIsFinished(bIsFinished),
		  Storage(TInPlaceType<TArray<TSharedRef<ISearchableItem>>>(), MoveTemp(Values))
	{
	}

	bool bIsFinished = true;
	TVariant<FEmptyVariantState, TSharedRef<ISearchableItem>, TArray<TSharedRef<ISearchableItem>>> Storage;
};
