#pragma once


class CWeftKnitWizard : public wxWizard
{
public:

	CWeftKnitWizard(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CWeftKnitWizard(void);

	bool RunIt();
	string GetCreateTextileCommand(string ExistingTextile = "");
	void LoadSettings(const CTextileWeftKnit &WeftKnit);

protected:
	
	void OnInit(wxInitDialogEvent& event);

	wxString m_Wales;
	wxString m_Courses;
	wxString m_WaleWidth;
	wxString m_CourseHeight;
	wxString m_YarnThickness;

	bool m_bCreateDomain;

	DECLARE_EVENT_TABLE()

};

