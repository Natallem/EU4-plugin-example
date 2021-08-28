#pragma once
#include "CoreMinimal.h"
#include "MessageEndpoint.h"
#include "SearchableItem.h"
#include "SettingsData/SettingItemTypes.h"
#include "InputCoreTypes.h"
#include "SettingsData/PropertyHolder.h"
// #include "Searcher.h"
class FSearcher;
/** Class contains all data for one user request : request string, number of elements to found and to buffer, condition
 * of request (is all elements found), and others (see fields description).
 *
 * For each request items that should be shown if user presses "More" button is found in advance to reduce waiting time.
 * 
 */
class FInputData
{
public:
	FInputData(const FString& InInputRequest, int32 DesiredOutputSize, int32 DesiredBufferSize,
	           ESettingType SearchType);
private:
	/** Checks if buffer contains previously found items for request and sends it to main thread, if endpoint is valid.
	 * Cleans buffer from items, which were sent.
	 * @param WeakMessageEndpoint Endpoint that will receive message, containing output items, if buffer has ones
	 */
	void MoveBufferedDataToOutput(const TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe>& WeakMessageEndpoint);

	/** User request input string */
	FString InputRequest;

	/**
	 * True if user request string changed, checks to detect if InputData is needless and searcher can get next
	 * InputData copy to find result for it.
	 */
	FThreadSafeBool bIsCancelled = false;
	/** How many items for this request should be found */
	int32 DesiredOutputSize;
	/** How many items for this request buffer should contain */
	int32 DesiredBufferSize;

	/** Type of items, that should be found */
	ESettingType SearchType;

	/** True then all data found and main thead notified about this */
	bool bIsProcessingFinished;

	/** True if all data found and rest of data is in buffer. */
	bool bIsAllDataFoundInBuffer = false;

	/** Next index in data holder to check */
	int32 NextIndexToCheck = 0;

	/** Found items number for this input for all time */
	int32 FoundOutputCounter = 0;

	/** Buffered items that will be displayed if user presses "More..." button */
	TArray<TSharedRef<ISearchableItem>> Buffer;

	friend FSearcher;
	friend TOptional<TSharedRef<ISearchableItem>> FPropertyHolder::FindNextItem(
		const TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputData);
};
