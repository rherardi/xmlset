// XMLProc.h: interface for the CXMLProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_)
#define AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

XERCES_CPP_NAMESPACE_USE
XALAN_CPP_NAMESPACE_USE

class CXMLProc  
{
public:
	CXMLProc();
	virtual ~CXMLProc();
	int Parse();
	int MakeConfigChange();

	// Get/Set m_verbose
	bool const GetVerbose();
	void SetVerbose(const bool verbose);

	// Get/Set m_configFile
	_TCHAR const * GetConfigFile();
	void SetConfigFile(const _TCHAR *configFile);

	// Get/Set m_element
	_TCHAR const * GetElement();
	void SetElement(const _TCHAR *element);

	// Get/Set m_attribute
	_TCHAR const * GetAttribute();
	void SetAttribute(const _TCHAR *attribute);

	// Get/Set m_value
	_TCHAR const * GetValue();
	void SetValue(const _TCHAR *value);

	// Get/Set m_slashInversion
	bool IsInversible();
	void SetSlashInversion(bool inversion);
	void InverseSlash();

	// Get/Set m_removeTrailSlash
	bool IsRemoveTrailSlash();
	void SetRemoveTrailSlash(bool removeTrailSlash);
	void RemoveTrailSlash();

	// Get/Set m_readMode
	bool IsReadMode();
	void SetReadMode(bool readMode);
private:

protected:
	bool m_verbose;
	_TCHAR m_configFile[256];
	_TCHAR m_element[256];
	_TCHAR m_attribute[256];
	_TCHAR m_value[256];
	bool m_slashInversion;
	bool m_removeTrailSlash;
	bool m_readMode;

	XercesDOMParser *parser;
	SAXErrorHandler *errHandler;
	DOMDocument* theXercesDocument;
	XalanDocument* theDocument;
	XercesDOMSupport*		theDOMSupport;
	XalanDocumentPrefixResolver*		thePrefixResolver;
	XercesParserLiaison* theLiaison;
};

#endif // !defined(AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_)
