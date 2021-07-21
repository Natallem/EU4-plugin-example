#pragma once

class FSearcher : public FRunnable
{
	
public:
	DECLARE_DELEGATE(FDataPortionFoundDelegate);

	FSearcher();
	//Destructor
	virtual ~FSearcher() override;

	//Use this method to kill the thread!!
	void EnsureCompletion();

	//Pause the thread 
	void PauseThread();

	//Continue/UnPause the thread
	void ContinueThread();

	//FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	bool IsThreadPaused();
	
	FEvent* GetSayHelloEvent() const
	{
		return SayHelloEvent;
	}
	FDataPortionFoundDelegate& OnNewDataFound();

private:
	//Thread to run the worker FRunnable on
	FRunnableThread* Thread;

	FCriticalSection Mutex;
	FEvent * SayHelloEvent;

	//As the name states those members are Thread safe
	FThreadSafeBool m_Kill;
	FThreadSafeBool m_Pause;
	FDataPortionFoundDelegate DataPortionFoundDelegate;

};
