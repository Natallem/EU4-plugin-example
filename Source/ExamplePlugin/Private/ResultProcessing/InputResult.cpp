#include "InputResult.h"

FInputResult::FInputResult(int32 Id, const FString& Input, int32 DesiredResultSize): Id(Id),
	Input(Input),
	DesiredResultSize(DesiredResultSize),
	IsFinishedProcess(Input.IsEmpty())
{
}
