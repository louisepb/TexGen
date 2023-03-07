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
	
	void OnWizardPageChanging(wxWizardEvent& event);
	void OnWaleWidthChanged(wxCommandEvent& event) { m_bWaleWidthChanged = true; }
	void OnCourseHeightChanged(wxCommandEvent& event) { m_bCourseHeightChanged = true; }
	void OnThicknessChanged(wxCommandEvent& event) { m_bThicknessChanged = true; }
	void OnInit(wxInitDialogEvent& event) { m_bWaleWidthChanged = m_bCourseHeightChanged = m_bThicknessChanged = false; }

	void OnRefine(wxCommandEvent& event);
	void OnDomain(wxCommandEvent& event);

	void BuildPages();
	void RefreshGapTextBox();

	wxWizardPageSimple* BuildFirstPage();

	wxSpinCtrl *m_pWalesSpin;
	wxSpinCtrl *m_pCoursesSpin;
	wxString m_WaleWidth;
	wxString m_CourseHeight;
	wxString m_YarnThickness;
	wxString m_GapSize;

	wxWizardPageSimple *m_pFirstPage;


	bool m_bCreateDomain;
	bool m_bRefine;
	bool m_bWaleWidthChanged;
	bool m_bCourseHeightChanged;
	bool m_bThicknessChanged;

	DECLARE_EVENT_TABLE()

};

