#pragma once

#include "CoreMinimal.h"
#include "Multithreading//SearchableItem.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Text/STextBlock.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSearchSettingDetail, Log, All);

class SWidget;
class SDetailsView;

class FAbstractSettingDetail : public ISearchableItem
{
public:

	virtual FName GetName() const;
	virtual FString GetPath() const = 0;

	virtual TSharedRef<SWidget> GetRowWidget() const override;

protected:
	TSharedPtr<SDetailsView> GetSDetailsView() const;
	void SetTextInSearchBox(TSharedPtr<SDetailsView> DetailsViewPtr, const FText& newText);
	FString Delimiter = "|";
};
