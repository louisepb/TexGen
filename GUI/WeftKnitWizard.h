/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2024 Louise Brown

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

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

	bool HasNextPage(wxWizardPage *page);

	wxWizardPageSimple* BuildFirstPage();
	wxWizardPageSimple* BuildSecondPage();

	wxWizardPageSimple *m_pFirstPage;
	wxRadioBox *m_pLoopModelRadio;
	wxSpinCtrl *m_pWalesSpin;
	wxSpinCtrl *m_pCoursesSpin;
	wxString m_WaleHeight;
	wxString m_CourseWidth;
	wxString m_LoopHeight;
	wxString m_YarnThickness;
	wxString m_GapSize;
	LoopModel m_LoopModel;

	
	wxWizardPageSimple *m_pSecondPage;
	wxSpinCtrl *m_pNumSlaveNodesSpin;
	wxSpinCtrl *m_pNumSectionPointsSpin;

	





	bool m_bCreateDomain;
	bool m_bRefine;
	bool m_bWaleHeightChanged;
	bool m_bCourseWidthChanged;
	bool m_bLoopHeightChanged;
	bool m_bThicknessChanged;
	bool m_bBuiltPages;

	DECLARE_EVENT_TABLE()

};

