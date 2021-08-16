#pragma once

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "Configuration.h"
#include "InputHandler.h"
#include "MessageEndpoint.h"
#include "SearchTask.h"
#include "SettingsData/PropertyHolder.h"

template <typename T>
class TSearchTask;
class FRunnableThread;

class FSearcher : public FRunnable
{
private:
	using FSearchTask = TSearchTask<RequiredType>;
	using FInputHandler = TInputHandler<RequiredType>;
public:
	FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint);

	virtual ~FSearcher() override = default;

	/** Calls before destruct searcher, stops thread and wait for it completion */
	void EnsureCompletion();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void SetInput(const FString& NewInput);

	/** Gives found result to main thread
	 * @return Pair, where key indicate weather all items for input request are found, value is found result
	 */
	TPair<bool, TArray<RequiredType>> GetRequestData();
	void FindMoreDataResult();
private:
	bool ExecuteFindResultTask(FSearchTask& Task);

	/** Executes fill buffer task for processing input.
	 * @param Task contains information about fill buffer task and buffer itself
	 * @return true if input change while processing
	 */
	bool FillBuffer(FSearchTask& Task) const;

	/** Saves state of task for input request, i.e. return to InputHandler request string, PArray, buffer and next index to search
	 * @param Task completed task, which data need to be saved in InputHandler
	 * @return true if saved successfully
	 */
	bool SaveTaskStateToResult(FSearchTask& Task);

	/** Append found item to InputHandler output array, using lock and calling NotifyMainThread
	 * @param Item will be given to main thread
	 * @param TaskId Id of current task
	 * @return true if added successfully (i.e. input does not change)
	 */
	bool AddFoundItemToResult(RequiredType&& Item, int32 TaskId);

	/** Notify main thread, that search is completed. 
	 * @param InputId Id of input, that was completed
	 * @return true if completed successfully (i.e. input does not change)
	 */
	bool AllWordsFound(int32 InputId);

	/** Send message, that new request item found. Does not notify main thread twice. Calls under lock*/
	void NotifyMainThread();

	FPropertyHolder& PropertyHolder;
	FEventRef WakeUpWorkerEvent;
	TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
	uint32 ChunkSize;
	FInputHandler InputHandler;
	FThreadSafeCounter RequestCounter;
	FCriticalSection InputOperationSection;
	bool bIsNotifiedMainThread = false;
	FThreadSafeBool bShouldStopThread = false;
	TUniquePtr<FRunnableThread> Thread;

	friend FInputHandler;
};
