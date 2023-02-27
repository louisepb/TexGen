#include "PrecompiledHeaders.h"
#include "WeftKnitWizard.h"
#include "WindowIDs.h"

#include "Wizard.xpm"

using namespace TexGen;

BEGIN_EVENT_TABLE(CWeftKnitWizard, wxWizard)


END_EVENT_TABLE()

CWeftKnitWizard::CWeftKnitWizard(wxWindow* parent, wxWindowID id)
	: wxWizard(parent, id, wxT("Weft Knit Wizard"), wxBitmap(Wizard_xpm))
	, m_bCreateDomain(true)
{

}

CWeftKnitWizard::~CWeftKnitWizard(void) {}

bool CWeftKnitWizard::RunIt()
{
	return true;
}

string CWeftKnitWizard::GetCreateTextileCommand(string ExistingTextile)
{
	string testReturn = "";
	return testReturn;
}

void CWeftKnitWizard::LoadSettings(const CTextileWeftKnit &WeftKnit)
{

}

void CWeftKnitWizard::OnInit(wxInitDialogEvent& event)
{

}
