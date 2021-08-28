#pragma once

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "InputData.h"
#include "MessageEndpoint.h"
#include "SettingsData/SettingItemTypes.h"

class FRunnableThread;
/**
 * Class used to hold searching thread, that used to find satisfying items in stored collected data in FPropertyHolder.
 * Contains logic for searching and handling life time of thread.
 */
class FSearcher : public FRunnable
{
public:
	/**
	 * @param ChunkSize Number of elements that will be found initially for one user request also number of elements
	 * that will be found additionally after clicking on "More button", also number of element in buffer
	 * @param MessageEndpoint Endpoint to main thread, to send found items for dislaying
	 */
	FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint);

	virtual ~FSearcher() override = default;

	/** Calls before destruct searcher, stops thread and wait for it completion */
	void EnsureCompletion();

	/** FRunnable interface */
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	/** Calls by main thread to set new input for searcher and cancel previous task
	 * @param NewInput new user request string
	 * @param SearchType type of items for searching for request
	 */
	void SetInput(const FString& NewInput, ESettingType SearchType);

	/** Calls by main thread to request more items for current input data
	 */
	void FindMoreDataResult();
private:
	/** Moves previously searched data from buffer to main thread, if we have request for more data, and checks, if we
	 * still need to process this input (i.e. find more data to be sent to UI thread and fill emptied buffer) 
	 * @return SharedPointer to current FInputData, if we still need to process it, otherwise return nullptr
	 */
	TSharedPtr<FInputData, ESPMode::ThreadSafe> CheckIfCurrentInputNeedToBeProcessed();

	bool ProcessCurrentInput(TSharedPtr<FInputData, ESPMode::ThreadSafe>& Task);

	/** Fills buffer task for given input data.
	 * @param InputData contains information about fill buffer task and buffer itself
	 * @return true if completed successfully (i.e. input does not change)
	 */
	static bool FillBuffer(TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputData);


	/** Send FItemFoundMsg with Item to main thread for given InputData if it is not canceled and if Endpoint is accessible
	 * @brief Sends to main UI thread found item
	 * @param Item Found item to send
	 * @param InputDataWeakPtr Input data, for which this item was found. Check before sending if it that was not cancelled 
	 * @return true if sent successfully (i.e. input is not changed and endpoint is accessible)
	 */
	bool SendFoundItem(TSharedRef<ISearchableItem>&& Item,
	                   TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputDataWeakPtr);

	/** Notify main thread, by sending FItemFoundMsg, that search is completed, by sending 
	 * @param InputTaskWeakPtr WeakPtr to task, that was completed.
	 * @return true if completed successfully (i.e. input is not changed and endpoint is accessible)
	 */
	bool AllWordsFound(const TWeakPtr<FInputData, ESPMode::ThreadSafe> InputTaskWeakPtr);

	/** If there is no work for thread, it sleeps, so we need to wake it up and continue processing input*/
	FEventRef WakeUpWorkerEvent;

	/** Endpoint to main thread to send messages, containing found data */
	TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
	uint32 ChunkSize;

	/** Contains the current user input query with data, that need to be processed or need to be processed more in
	 * future (in case user presses "More" button)
	 * Searching thread creates a copy of it and processes it. If input changes, previous InputData sets the flag
	 * InputData::bIsCancelled to true and reassign this TSharedPtr to new FInputData. As soon as searching thread
	 * detects, that task was cancelled due to checking InputData::bIsCancelled flag, takes new copy of InputData
	 * SharedPtr and previous one is deleted due to SharedPtr semantic.
	 */
	TSharedPtr<FInputData, ESPMode::ThreadSafe> InputData;

	/** Critical Section uses then we change or copy FSearcher::InputData */
	FCriticalSection CriticalSection;

	/** Used as signal to stop the searching thead */
	FThreadSafeBool bShouldStopThread = false;

	/* Searching thread */
	TUniquePtr<FRunnableThread> Thread;
};
