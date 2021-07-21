#include "Searcher.h"

FSearcher::FSearcher()
{
	m_Kill = false;
	m_Pause = false;

	//Initialize FEvent (as a cross platform (Confirmed Mac/Windows))
	SayHelloEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);;

	Thread = FRunnableThread::Create(this, TEXT("SearchEverywhereThread"), 0, TPri_Normal);
}

FSearcher::~FSearcher()
{
	if (SayHelloEvent)
	{
		//Cleanup the FEvent
		FGenericPlatformProcess::ReturnSynchEventToPool(SayHelloEvent);
		SayHelloEvent = nullptr;
	}

	if (Thread)
	{
		//Cleanup the worker thread
		delete Thread;
		Thread = nullptr;
	}
}

bool FSearcher::Init()
{
	return true;
}

uint32 FSearcher::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);
	UE_LOG(LogTemp, Log, TEXT("EP : FSearcher Running!"));

	while (!m_Kill)
	{
		SayHelloEvent->Wait();
		if (m_Kill)
		{
			return 0;
		}
		FPlatformProcess::Sleep(1);
		UE_LOG(LogTemp, Log, TEXT("EP : FSearcher say hello"));
		OnNewDataFound().ExecuteIfBound();
	}
	return 0;
}

void FSearcher::PauseThread()
{
	m_Pause = true;
}

void FSearcher::ContinueThread()
{
	m_Pause = false;
}

void FSearcher::Stop()
{
	m_Kill = true; //Thread kill condition "while (!m_Kill){...}"
	m_Pause = false;
	if (SayHelloEvent)
	{
		SayHelloEvent->Trigger();
	}
}

void FSearcher::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

bool FSearcher::IsThreadPaused()
{
	return m_Pause;
}

FSearcher::FDataPortionFoundDelegate& FSearcher::OnNewDataFound()
{
	return DataPortionFoundDelegate;
}
