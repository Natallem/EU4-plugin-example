#pragma once

#include "Templates/SharedPointer.h"

class SWidget;
class FText;

class ISearchableItem
{
public:
	virtual ~ISearchableItem() = default;

	virtual FText GetDisplayName() const = 0;
	virtual void DoAction() = 0;
	virtual TSharedRef<SWidget> GetRowWidget(FText Request) const = 0;
};
