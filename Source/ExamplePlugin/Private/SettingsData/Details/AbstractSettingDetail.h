#pragma once

#include "CoreMinimal.h"
#include "Widgets/SBoxPanel.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

class SWidget;
class SDetailsView;
class FAbstractSettingDetail
{
public:
	virtual ~FAbstractSettingDetail() = default;

	virtual FText GetDisplayName() const = 0;
	virtual FName GetName() const = 0;
	virtual void DoAction() const = 0;
	virtual FString GetPath() const = 0;

	virtual TSharedRef<SWidget> GetRowWidget() const;

protected:
	virtual TSharedPtr<SWidget> CreateRowWidget() const;

	void Foo() const;
	mutable TSharedPtr<SWidget> RowWidget;
	FString Delimiter = "|";
private:
	void SetTextInSearchBox(TSharedPtr<SDetailsView> DetailsViewPtr, const FText& newText) const;
};
