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
	BuildPages();
	GetPageAreaSizer()->Add(m_pFirstPage);
}

CWeftKnitWizard::~CWeftKnitWizard(void) {}

void CWeftKnitWizard::BuildPages()
{
	m_pFirstPage = BuildFirstPage();
}


wxWizardPageSimple* CWeftKnitWizard::BuildFirstPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);

	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("This wizard will create a weft knit model for you.")), SizerFlags);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	pSubSizer = new wxFlexGridSizer(2);

	{
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Wales:")), SizerFlags);
		pSubSizer->Add(m_pWalesSpin = new wxSpinCtrl(pPage, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pWalesSpin->SetToolTip(wxT("Controls the number of wales contained in the unit cell."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Courses:")), SizerFlags);
		pSubSizer->Add(m_pCoursesSpin = new wxSpinCtrl(pPage, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pCoursesSpin->SetToolTip(wxT("Controls the number of courses contained in the unit cell."));

		wxTextCtrl* pControl;

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Wale Width:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Spacing, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WaleWidth)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the width of one wale."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Course Height:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Width, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_CourseHeight)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the height of one course."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Thickness:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Thickness, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_YarnThickness)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the thickness of the yarn."));
	}

	pMainSizer->Add(pSubSizer, SizerFlags);
	SizerFlags.Align(0);

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	m_pWalesSpin->SetValue(2);
	m_pCoursesSpin->SetValue(2);

	return pPage;
}

bool CWeftKnitWizard::RunIt()
{
	return RunWizard(m_pFirstPage);
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
