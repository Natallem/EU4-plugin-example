#pragma once
#include "Misc/TVariant.h"
#include "ItemFoundMsg.generated.h"

class ISearchableItem;


/** Message class, that used to transfer found items from searching thread in FSearcher to main thread to be shown to
 * user, using message bus.
 *
 * Message can contains
 * 
 * 1) one word in case FSearcher found one item
 * 
 * 2) array of found words in case of sending data from buffer 
 *
 * 3) no items and notify main thread that all items if found
 *
 * Do not need to free memory after new operator, "message bus will take ownership of all sent and published message objects".
 * @see FMessageEndpoint::Send
 */
USTRUCT()
struct FItemFoundMsg
{
	GENERATED_USTRUCT_BODY()

	FItemFoundMsg() = default;

	FItemFoundMsg(bool bIsFinished, TSharedRef<ISearchableItem> Value)
		: bIsFinished(bIsFinished),
		  Storage(TInPlaceType<TSharedRef<ISearchableItem>>(), MoveTemp(Value))
	{
	}

	FItemFoundMsg(bool bIsFinished, TArray<TSharedRef<ISearchableItem>>&& Values)
		: bIsFinished(bIsFinished),
		  Storage(TInPlaceType<TArray<TSharedRef<ISearchableItem>>>(), MoveTemp(Values))
	{
	}

	bool bIsFinished = true;
	TVariant<FEmptyVariantState, TSharedRef<ISearchableItem>, TArray<TSharedRef<ISearchableItem>>> Storage;
};
