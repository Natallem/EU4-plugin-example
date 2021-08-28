#pragma once

#include "Templates/SharedPointer.h"

class SWidget;
class FText;


/** Interface for items, which can be displayed in plugin and can be searchable by FSearcher */
class ISearchableItem
{
public:
	virtual ~ISearchableItem() = default;

	/**
	 * @return Display name, which used to check if item satisfy request.
	 */
	virtual FText GetDisplayName() const = 0;


	/** This function is called then user click on item in plugin window */
	virtual void DoAction() = 0;


	/** This function is called for display row for this item it SSearchEverywhereWidget::ItemsListView
	 * @param Request User request, for which this item was searched.
	 * @return Widget that will be displayed by SSearchEverywhereWidget::ItemsListView
	 */
	virtual TSharedRef<SWidget> GetRowWidget(FText Request) const = 0;
};
