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
	void OnWaleHeightChanged(wxCommandEvent& event) { m_bWaleHeightChanged = true; }
	void OnCourseWidthChanged(wxCommandEvent& event) { m_bCourseWidthChanged = true; }
	void OnLoopHeightChanged(wxCommandEvent& event) { m_bLoopHeightChanged = true; }
	void OnThicknessChanged(wxCommandEvent& event) { m_bThicknessChanged = true; }
	void OnInit(wxInitDialogEvent& event) { m_bCourseWidthChanged = m_bWaleHeightChanged = m_bThicknessChanged = m_bLoopHeightChanged = false; }

	void OnRefine(wxCommandEvent& event);
	void OnDomain(wxCommandEvent& event);

	void BuildPages();
	void RefreshGapTextBox();

	wxWizardPageSimple* BuildFirstPage();

	wxSpinCtrl *m_pWalesSpin;
	wxSpinCtrl *m_pCoursesSpin;
	wxString m_WaleHeight;
	wxString m_CourseWidth;
	wxString m_LoopHeight;
	wxString m_YarnThickness;
	wxString m_GapSize;

	wxWizardPageSimple *m_pFirstPage;


	bool m_bCreateDomain;
	bool m_bRefine;
	bool m_bWaleHeightChanged;
	bool m_bCourseWidthChanged;
	bool m_bLoopHeightChanged;
	bool m_bThicknessChanged;

	DECLARE_EVENT_TABLE()

};

