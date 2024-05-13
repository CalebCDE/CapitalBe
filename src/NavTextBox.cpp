#include "NavTextBox.h"
#include "CBLocale.h"
#include "Database.h"
#include "MsgDefs.h"
#include "TimeSupport.h"

NavTextBoxFilter::NavTextBoxFilter(NavTextBox* box)
	: AutoTextControlFilter(box)
{
}

filter_result
NavTextBoxFilter::KeyFilter(const int32& key, const int32& mod)
{
	// Here is where all the *real* work for a date box is done.
	if (key == B_TAB && ((NavTextBox*)TextControl())->IsTabFiltering()) {
		if (mod & B_SHIFT_KEY)
			SendMessage(new BMessage(M_PREVIOUS_FIELD));
		else
			SendMessage(new BMessage(M_NEXT_FIELD));
		return B_SKIP_MESSAGE;
	}

#ifdef ENTER_NAVIGATION
	if (key == B_ENTER) {
		SendMessage(new BMessage(M_ENTER_NAVIGATION));
		return B_SKIP_MESSAGE;
	}
#endif

	return B_DISPATCH_MESSAGE;
}

NavTextBox::NavTextBox(
	const char* name, const char* label, const char* text, BMessage* msg, uint32 flags)
	: AutoTextControl(name, label, text, msg, flags),
	  fFilterTab(true)
{
	SetFilter(new NavTextBoxFilter(this));
}

bool
NavTextBox::Validate(const bool& alert)
{
	return true;
}
