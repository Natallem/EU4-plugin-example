#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

class SWidget;

class ISettingDetail
{
public:
	virtual ~ISettingDetail() = default;
	virtual FText GetDisplayName() = 0;
	virtual TSharedRef<SWidget> GetRowWidget() = 0;
};
