#pragma once

class ExampleAsyncTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<ExampleAsyncTask>;
	int32 ExampleData;
	FEvent * HelloEvent;

	ExampleAsyncTask(int32 InExampleData, FEvent * HelloEvent);

	void DoWork();

	FORCEINLINE TStatId GetStatId() const;
	
	~ExampleAsyncTask();
	
	
};