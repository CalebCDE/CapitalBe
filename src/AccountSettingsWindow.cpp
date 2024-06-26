#include "AccountSettingsWindow.h"
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <MessageFilter.h>

#include "AutoTextControl.h"
#include "CBLocale.h"
#include "Database.h"
#include "PrefWindow.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Account"


#define M_EDIT_ACCOUNT_SETTINGS 'east'
#define M_NAME_CHANGED 'nmch'
#define M_TOGGLE_USE_DEFAULT 'tgud'


AccountSettingsWindow::AccountSettingsWindow(Account* account)
	: BWindow(BRect(0, 0, 1, 1), B_TRANSLATE("Account settings"), B_FLOATING_WINDOW_LOOK,
		  B_MODAL_APP_WINDOW_FEEL,
		  B_NOT_RESIZABLE | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS |
			  B_CLOSE_ON_ESCAPE),
	  fAccount(account)
{
	AddShortcut('W', B_COMMAND_KEY, new BMessage(B_QUIT_REQUESTED));
	AddShortcut('Q', B_COMMAND_KEY, new BMessage(B_QUIT_REQUESTED));

	BView* back = new BView("back", B_WILL_DRAW);
	back->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	fAccountName = new AutoTextControl("accname", B_TRANSLATE("Name:"),
		(fAccount ? fAccount->Name() : NULL), new BMessage(M_NAME_CHANGED));
	fAccountName->SetCharacterLimit(32);

	fUseDefault = new BCheckBox("usedefault", B_TRANSLATE("Use default currency settings"),
		new BMessage(M_TOGGLE_USE_DEFAULT));
	if (!fAccount || fAccount->IsUsingDefaultLocale())
		fUseDefault->SetValue(B_CONTROL_ON);

	Locale templocale;
	if (fAccount)
		templocale = fAccount->GetLocale();
	fPrefView = new CurrencyPrefView("prefview", &templocale);

	fOK = new BButton("okbutton", B_TRANSLATE("OK"), new BMessage(M_EDIT_ACCOUNT_SETTINGS));

	if (strlen(fAccountName->Text()) < 1)
		fOK->SetEnabled(false);

	fCancel = new BButton("cancelbutton", B_TRANSLATE("Cancel"), new BMessage(B_QUIT_REQUESTED));

	SetDefaultButton(fOK);

	if (!fAccount || fAccount->IsUsingDefaultLocale()) {
		fPrefView->Hide();
	}

	BLayoutBuilder::Group<>(back, B_VERTICAL, 0.0f)
		.SetInsets(10)
		.AddGroup(B_VERTICAL, 0.0f)
		.Add(fAccountName)
		.Add(fUseDefault)
		.Add(fPrefView)
		.End()
		.AddGrid(0.0f, 0.0f)
		.AddGlue(0, 0)
		.Add(fCancel, 1, 0)
		.Add(fOK, 2, 0)
		.End()
		.End();
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0.0f).SetInsets(0).Add(back).End();
	CenterIn(Frame());
	fAccountName->MakeFocus(true);
}


void
AccountSettingsWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case M_EDIT_ACCOUNT_SETTINGS:
		{
			Locale customLocale;
			Locale defaultLocale;

			if (!fAccount) {
				fPrefView->GetSettings(customLocale);
				gDatabase.AddAccount(fAccountName->Text(), ACCOUNT_BANK, "Open",
					defaultLocale == customLocale ? NULL : &customLocale);
			} else {
				if (strcmp(fAccountName->Text(), fAccount->Name()) != 0)
					gDatabase.RenameAccount(fAccount, fAccountName->Text());

				fPrefView->GetSettings(customLocale);
				if (fUseDefault->Value() != B_CONTROL_ON) {
					fAccount->UseDefaultLocale(false);
					fAccount->SetLocale(customLocale);
				} else {
					fAccount->UseDefaultLocale(true);
				}
			}

			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case M_TOGGLE_USE_DEFAULT:
		{
			bool useDefault = fUseDefault->Value() == B_CONTROL_ON;

			if (useDefault)
				fPrefView->Hide();
			else
				fPrefView->Show();

			break;
		}
		case M_NAME_CHANGED:
		{
			if (strlen(fAccountName->Text()) < 1)
				fOK->SetEnabled(false);
			else
				fOK->SetEnabled(true);

			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}
