#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
class SWidget;

class FAbstractSettingDetail
{
public:
	virtual ~FAbstractSettingDetail() = default;

	virtual FText GetDisplayName() const = 0;
	virtual void DoAction() const = 0;

	virtual TSharedRef<SWidget> GetRowWidget() const
	{
		if (!RowWidget.IsValid())
		{
			RowWidget = CreateRowWidget();
		}
		return RowWidget.ToSharedRef();
	}

protected:
	virtual TSharedPtr<SWidget> CreateRowWidget() const
	{
		return SNew(STextBlock)
			.Text(GetDisplayName());
	}

	mutable TSharedPtr<SWidget> RowWidget;
};
