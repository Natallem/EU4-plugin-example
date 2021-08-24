#pragma once

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "Configuration.h"
#include "InputHandler.h"
#include "MessageEndpoint.h"

class FRunnableThread;

class FSearcher : public FRunnable
{
private:
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

	void FindMoreDataResult();
private:
	/**
	 * @brief 
	 * @param Task 
	 * @return true if completed successfully (i.e. input does not change)
	 */
	bool ExecuteFindResultTask(TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& Task);

	/** Executes fill buffer task for processing input.
	 * @param Task contains information about fill buffer task and buffer itself
	 * @return true if completed successfully (i.e. input does not change)
	 */
	bool FillBuffer(TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& Task) const;
	
	/** Append found item to InputHandler output array, using lock and calling NotifyMainThread
	 * @param Item will be given to main thread
	 * @param TaskId Id of current task
	 * @return true if added successfully (i.e. input does not change)
	 */
	bool AddFoundItemToResult(RequiredType&& Item, TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& Task);

	/** Notify main thread, that search is completed. 
	 * @param InputTaskWeakPtr WeakPtr to task, that was completed.
	 * @return true if completed successfully (i.e. input does not change)
	 */
	bool AllWordsFound(const TWeakPtr<FInputHandler, ESPMode::ThreadSafe> InputTaskWeakPtr);


	FEventRef WakeUpWorkerEvent;
	TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
	uint32 ChunkSize;
	// FInputHandler InputHandler;
	// FThreadSafeCounter RequestCounter;
	TSharedPtr<FInputHandler, ESPMode::ThreadSafe> InputHandlerPtr;
	FCriticalSection CriticalSection;
	bool bIsNotifiedMainThread = false;
	FThreadSafeBool bShouldStopThread = false;
	TUniquePtr<FRunnableThread> Thread;

	friend FInputHandler;
};
