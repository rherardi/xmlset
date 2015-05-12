// XMLSet.hpp: interface for the CXMLSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLSET_HPP__90BFB3A4_5935_48EB_AE2E_014859F49591__INCLUDED_)
#define AFX_XMLSET_HPP__90BFB3A4_5935_48EB_AE2E_014859F49591__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLProc;

class CXMLSet  
{
public:
	CXMLSet();
	virtual ~CXMLSet();
	int ProcessCommandLine(int argc, _TCHAR *argv[]);
	int MakeConfigChange();

protected:
	CXMLProc *m_xmlProc;
	bool m_verbose;
	
	void ShowUsage();


};

#endif // !defined(AFX_XMLSET_HPP__90BFB3A4_5935_48EB_AE2E_014859F49591__INCLUDED_)
