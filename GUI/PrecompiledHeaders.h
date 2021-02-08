/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

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

#ifdef __WIN32
	#include <wx/wxprec.h>
#else
	//#include <wx/wx.h>
	#include <C:\wxWidgets-3.0.4\include\wx\wx.h>
#endif

// Prevent python.h from adding python24_d.lib to the linker in debug mode (that library does not exist)
/*#ifdef WIN32
#	ifdef _DEBUG
#		undef _DEBUG
#		include <Python.h>
#		define _DEBUG
#	else
#		include <Python.h>
#	endif
#else*/
#include <Python.h>
//#endif

#include <vector>
#include <list>
#include <map>
#include <string>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <deque>
#include <set>

#pragma warning( disable : 4273 )
#pragma warning( disable : 4996 ) //Deprecated strcpy

//Define this to make sure it compiles in unicode mode
//#define wxUSE_UNICODE 1



#include <C:/wxWidgets-3.0.4/include/wx/imaglist.h>
#include <C:/wxWidgets-3.0.4/include/wx/artprov.h>
#include <C:/wxWidgets-3.0.4/include/wx/stdpaths.h>
#include <C:/wxWidgets-3.0.4/include/wx/cmdline.h>
#include <C:/wxWidgets-3.0.4/include/wx/colordlg.h>
#include <C:/wxWidgets-3.0.4/include/wx/valgen.h>
#include <C:/wxWidgets-3.0.4/include/wx/choicebk.h>
#include <C:/wxWidgets-3.0.4/include/wx/xrc/xmlres.h>
#include <C:/wxWidgets-3.0.4/include/wx/tglbtn.h>
#include <C:/wxWidgets-3.0.4/include/wx/spinctrl.h>
#include <C:/wxWidgets-3.0.4/include/wx/wizard.h>
#include <C:/wxWidgets-3.0.4/include/wx/listctrl.h>
#include <C:/wxWidgets-3.0.4/include/wx/dnd.h>
#include <C:/wxWidgets-3.0.4/include/wx/laywin.h>
#include <C:/wxWidgets-3.0.4/include/wx/treectrl.h>
#include <C:/wxWidgets-3.0.4/include/wx/grid.h>
#include <C:/wxWidgets-3.0.4/include/wx/aui/auibook.h>
#include <C:/wxWidgets-3.0.4/include/wx/config.h>
#include <C:/wxWidgets-3.0.4/include/wx/dcbuffer.h>
#include <C:/wxWidgets-3.0.4/include/wx/print.h>
#include <C:/wxWidgets-3.0.4/include/wx/printdlg.h>
#include <C:/wxWidgets-3.0.4/include/wx/hyperlink.h>

using namespace std;

// Texgen Core

#define TIXML_USE_STL
#include "../tinyxml/tinyxml.h"
#include "../Core/mymath.h"
#include "../Core/TexGen.h"

using namespace TexGen;

inline string ConvertString(wxString String)
{
	return string(String.mb_str());
}

inline wxString ConvertString(string String)
{
	return wxString(String.c_str(), wxConvUTF8);
}

inline wxColour ConvertColor(COLOR Color)
{
	return wxColour((unsigned char)(Color.r*255), (unsigned char)(Color.g*255), (unsigned char)(Color.b*255));
}







