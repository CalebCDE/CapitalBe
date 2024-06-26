#ifndef TRANSEDITWIN_H
#define TRANSEDITWIN_H

#include <Message.h>
#include <Window.h>
#include "SplitView.h"

class TransactionData;

class TransactionEditWindow : public BWindow {
public:
	TransactionEditWindow(const BRect& frame, const TransactionData& trans);
	void MessageReceived(BMessage* msg);

private:
	SplitView* fSplitView;
};

#endif
