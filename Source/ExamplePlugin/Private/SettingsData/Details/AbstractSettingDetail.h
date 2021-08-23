#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Text/STextBlock.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSearchSettingDetail, Log, All);

class SWidget;
class SDetailsView;
class FAbstractSettingDetail
{
public:
	virtual ~FAbstractSettingDetail() = default;

	virtual FText GetDisplayName() const = 0;
	virtual FName GetName() const;
	virtual void DoAction() = 0;
	virtual FString GetPath() const = 0;

	virtual TSharedRef<SWidget> GetRowWidget() const;

protected:
	TSharedPtr<SDetailsView> GetSDetailsView() const;
	void SetTextInSearchBox(TSharedPtr<SDetailsView> DetailsViewPtr, const FText& newText);
	FString Delimiter = "|";
};
