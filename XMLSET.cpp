// XMLSET.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XMLSet.hpp"
#include "XMLProcSuppl.hpp"
#include "XMLProc.hpp"

#include "XGetopt.h"
#include "Resources.h"
#include "resource.h"

int wmain(int argc, _TCHAR* argv[])
{
	int retCode;

	CXMLSet *theApp = new CXMLSet();
	CResources::ShowTitle(IDS_TITLE, IDS_VERSION);

	retCode = theApp->ProcessCommandLine(argc, argv);
	if (retCode == PARAMETERS_OK) {
		retCode = theApp->MakeConfigChange();
		if (retCode == STEP_SUCCESS) {
			retCode = SUCCESS;
		}
	}
//	if (theApp->IsVerbose()){
//		::LoadString(hInst, IDS_EXIT_CODE, usage, 256);
//		::_tprintf(_TEXT("%s %d\n"), usage, retCode);
//	}

//	INI_LogWriteWithResourcePrefixNum(IDS_LOG_EXIT_CODE, retCode);

//	INI_LogClose();
	theApp->~CXMLSet();
	return retCode;
}


//////////////////////////////////////////////////////////////////////
// CXMLSet Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLSet::CXMLSet():
m_xmlProc(new CXMLProc())
{

}

CXMLSet::~CXMLSet()
{

}

// Process all command line options
int CXMLSet::ProcessCommandLine(int argc, _TCHAR *argv[])
{
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR prompt[512];
	int c;

	if (argc == 1) {
		ShowUsage();
		return ERR_NO_PARAMETERS;
	}

	while ((c = getopt(argc, argv, _TEXT("a:be:f:irtv:"))) != EOF)
	{
		 switch (c)
		 {
			 case _T('a'):
				 {
					m_xmlProc->SetAttribute(argv[optind-1]);
				 }
				 break;
			 case _T('b'):
				 {
					m_xmlProc->SetVerbose(true);
				 }
				 break;
			 case _T('e'):
				 {
					m_xmlProc->SetElement(argv[optind-1]);
				 }
				 break;
			 case _T('f'):
				 {
					m_xmlProc->SetConfigFile(argv[optind-1]);
				 }
				 break;
			 case _T('i'):
				 {
					m_xmlProc->SetSlashInversion(true);
				 }
				 break;
			 case _T('r'):
				 {
					m_xmlProc->SetReadMode(true);
				 }
				 break;
			 case _T('t'):
				 {
					m_xmlProc->SetRemoveTrailSlash(true);
				 }
				 break;
			 case _T('v'):
				 {
					m_xmlProc->SetValue(argv[optind-1]);
				 }
				 break;
			 case _T('!'):
				::LoadString(hInst, IDS_OPT_ERR_MISSEDARG, prompt, 256);
				::_tprintf(_TEXT("%s %s %d %d\n"), prompt, argv[optind-1], optind, argc);
				ShowUsage();
				return ERR_MISSEDARG;
				break;

			 case _T('?'):
				ShowUsage();
				return ERR_ILLEGAL_OPTION;
				break;

			 default:
				{
				::LoadString(hInst, IDS_OPT_ERR_NOHANDLER, prompt, 256);
				::_tprintf(_TEXT("%s %s\n"), prompt, argv[optind-1]);
				}
				return ERR_NO_OPTION_HANDLER;
				break;
		 }
	}
	//
	// check for non-option args here
	//
	if (optind != argc) {
		::LoadString(hInst, IDS_OPT_ERR_MISSEDARG, prompt, 256);
		::_tprintf(_TEXT("%s %s\n"), prompt, argv[1]);
		ShowUsage();
		return ERR_MISSEDARG;
	}

	if (m_xmlProc->IsRemoveTrailSlash()) {
		m_xmlProc->RemoveTrailSlash();
	}

	if (m_xmlProc->IsInversible()) {
		m_xmlProc->InverseSlash();
	}

	if (m_xmlProc->Parse() != STEP_SUCCESS) {
		return ERR_FAILED;
	}

	return PARAMETERS_OK;
}

// Show utility full title and copyright

void CXMLSet::ShowUsage()
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);

// Usage: XMLSET [OPTION]...
// -a attribute
// -b
// -e element
// -f file
// -i
// -r
// -t
// -v value

	::LoadString(hInst, IDS_USAGE, usage, 256);
	::_tprintf(_TEXT("%s\n\n"), usage);

	// -a attribute
	::LoadString(hInst, IDS_OPTION_ATTRIBUTE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -b
	::LoadString(hInst, IDS_OPTION_VERBOSE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -e element
	::LoadString(hInst, IDS_OPTION_ELEMENT, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -f file
	::LoadString(hInst, IDS_OPTION_FILE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -i
	::LoadString(hInst, IDS_OPTION_SLASH_INVERSION, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -i
	::LoadString(hInst, IDS_OPTION_SLASH_REMOVE_TRAIL, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -r
	::LoadString(hInst, IDS_OPTION_READ_ONLY, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -v value
	::LoadString(hInst, IDS_OPTION_VALUE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
}

int CXMLSet::MakeConfigChange()
{
	int retCode = m_xmlProc->MakeConfigChange();
	return retCode;
}
