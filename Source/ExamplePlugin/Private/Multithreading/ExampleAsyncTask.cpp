#include "ExampleAsyncTask.h"

ExampleAsyncTask::ExampleAsyncTask(int32 InExampleData, FEvent* HelloEvent): ExampleData(InExampleData), HelloEvent(HelloEvent)
{
}

void ExampleAsyncTask::DoWork()
{
	for (int i = 0; i < ExampleData; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("EP : ExampleAsyncTask with input %d completed %d"), ExampleData, i);
	}
}

TStatId ExampleAsyncTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
}

ExampleAsyncTask::~ExampleAsyncTask()
{
	UE_LOG(LogTemp, Log, TEXT("EP : ExampleAsyncTask destructor"));
}
