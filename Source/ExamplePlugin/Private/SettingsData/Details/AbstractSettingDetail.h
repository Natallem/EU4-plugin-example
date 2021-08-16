#pragma once

#include "CoreMinimal.h"
#include "Widgets/SBoxPanel.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

class SWidget;

class FAbstractSettingDetail
{
public:
	virtual ~FAbstractSettingDetail() = default;

	virtual FText GetDisplayName() const = 0;
	virtual FName GetName() const = 0;
	virtual void DoAction() const = 0;
	virtual FString GetPath() const = 0;

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
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(GetDisplayName())
			]
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Right)
			  .FillWidth(0.25)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FColor::FromHex("#808080"))
				.Text(FText::FromString(GetPath()))
			];
	}

	mutable TSharedPtr<SWidget> RowWidget;
	FString Delimiter = "|";
};
