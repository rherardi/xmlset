// XMLProc.cpp: implementation of the CXMLProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <eh.h>

#include <xercesc/util/XercesDefs.hpp>

#include <xalanc/Include/PlatformDefinitions.hpp>

#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/framework/LocalFileInputSource.hpp>
//#include <xercesc/framework/MemBufInputSource.hpp>

#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanElement.hpp>
#include <xalanc/XalanDOM/XalanNode.hpp>
#include <xalanc/XalanDOM/XalanNamedNodeMap.hpp>
#include <xalanc/XalanDOM/XalanNodeList.hpp>
#include <xalanc/XalanDOM/XalanDOMException.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>

#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMException.hpp>

#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/MutableNodeRefList.hpp>

#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>

#include "SAXErrorHandler.hpp"

#include "XMLProc.hpp"
//#include "XMLNameValue.hpp"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLProc::CXMLProc()
{
	m_configFile[0] = _T('\0');
	m_element[0] = _T('\0');
	m_attribute[0] = _T('\0');
	m_value[0] = _T('\0');
	m_slashInversion = false;
	m_removeTrailSlash = false;
	m_readMode = false;
	m_verbose = false;

	// Initialize the XML4C2 system
	try
	{

		XMLPlatformUtils::Initialize();

		XPathEvaluator::initialize();


		//
		//  Create our parser, then attach an error handler to the parser.
		//  The parser will call back to methods of the ErrorHandler if it
		//  discovers errors during the course of parsing the XML document.
		//
		parser = new XercesDOMParser();

	//	parser->setValidationScheme(gValScheme);
	//	parser->setDoNamespaces(gDoNamespaces);
	//	parser->setDoSchema(gDoSchema);
	//	parser->setValidationSchemaFullChecking(gSchemaFullChecking);
	//	parser->setCreateEntityReferenceNodes(gDoCreate);

		parser->setDoNamespaces(true);
		parser->setDoSchema(false);

		errHandler = new SAXErrorHandler();
		parser->setErrorHandler(errHandler);

	}

	catch(const XMLException &toCatch)
	{
//		XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
//			 << "  Exception message:"
//			 << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
//		return 1;
		_tprintf(_T("XMLException: %s\n"), toCatch.getMessage());
	}


}

CXMLProc::~CXMLProc()
{
/*
	//
	//  Delete the parser itself.  Must be done prior to calling Terminate, below.
	//
	delete parser;
*/
	XPathEvaluator::terminate();

	// And call the termination method
	XMLPlatformUtils::Terminate();
}

int CXMLProc::Parse()
{
	int retCode = STEP_SUCCESS;
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CXMLProc::Parse()");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

	//
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	bool errorsOccured = false;
	try
	{
		parser->parse(m_configFile);
	}
	catch (const OutOfMemoryException& e)
	{
//		XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
		errorsOccured = true;
	}
	catch (const XMLException& e)
	{
//		XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
//			 << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
		_tprintf(_T("error: %s\n"), e.getMessage());
		errorsOccured = true;
	}

	catch (const DOMException& e)
	{
//		const unsigned int maxChars = 2047;
//		XMLCh errText[maxChars + 1];

//		XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << gXmlFile << "'\n"
//			 << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

//		if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
//			 XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

		_tprintf(_T("error: %s\n"), e.getMessage());
		errorsOccured = true;
	}

	catch (...)
	{
//		XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
		_tprintf(_T("Uncatched exception!\n"));
		errorsOccured = true;
	}

	// If the parse was successful, output the document data from the DOM tree
	if (!errorsOccured && !errHandler->getSawErrors()) {


//		XALAN_USING_XERCES(LocalFileInputSource)

		XALAN_USING_XALAN(XalanDocument)
		XALAN_USING_XALAN(XalanDocumentPrefixResolver)
		XALAN_USING_XALAN(XalanDOMString)
		XALAN_USING_XALAN(XalanNode)
		XALAN_USING_XALAN(XObjectPtr)

		XALAN_USING_XALAN(XercesParserLiaison)
		XALAN_USING_XALAN(XercesDOMSupport)

//		_tprintf(_T("err20: %d %d\n"), errorsOccured, errHandler->getSawErrors());
//		_tprintf(_T("err20\n"));
		theDOMSupport = new XercesDOMSupport();

		try {
//		_tprintf(_T("err200\n"));
		theXercesDocument = parser->getDocument();
		assert(theXercesDocument != NULL);
//		_tprintf(_T("err201\n"));
//		XercesParserLiaison *theLiaison = new XercesParserLiaison(*theDOMSupport);
		theLiaison = new XercesParserLiaison(*theDOMSupport);
//		XercesParserLiaison *theLiaison = new XercesParserLiaison();
//		XercesDocumentWrapper *wrapper = theLiaison->doCreateDocument(theXercesDocument, false, false, false, false);
//		MemoryManagerType theManager = XALAN_DEFAULT_MEMMGR;
//		XercesDocumentWrapper *wrapper = new XercesDocumentWrapper(*XMLPlatformUtils::fgMemoryManager, theXercesDocument);
//		_tprintf(_T("err2010\n"));

//		XercesDocumentWrapper *wrapper = new XercesDocumentWrapper(theXercesDocument);
//		_tprintf(_T("err2011:\n"));
		assert(theLiaison != NULL);
		theDocument =
				theLiaison->createDocument(theXercesDocument, true, true, true);
		assert(theDocument != 0);

//		_tprintf(_T("err202:\n"));
		thePrefixResolver = new XalanDocumentPrefixResolver(theDocument);
		}
		catch (const XMLException& e)
		{
			_tprintf(_T("XMLException: %s\n"), e.getMessage());
		}
		catch (const DOMException& e)
		{
			_tprintf(_T("DOMException: %s\n"), e.getMessage());
		}
		catch (const SAXParseException& e)
		{
			_tprintf(_T("SAXParseException: %s\n"), e.getMessage());
		}
		catch (const XercesDOMException& e)
		{
			_tprintf(_T("XercesDOMException: %d\n"), e.getExceptionCode());
		}
		catch (const XalanDOMException& e)
		{
			_tprintf(_T("XalanDOMException: %d\n"), e.getExceptionCode());
		}
		
		catch(exception e) {
			_tprintf(_T("exception\n"));
			retCode = ERR_FAILED;
			goto cleanup;
//			printf(e.what());
		}
		catch(...) {
			_tprintf(_T("Uncatched exception!\n"));
			retCode = ERR_FAILED;
			goto cleanup;
		}

	}
	else {
//		_tprintf(_T("err2: %d %d\n"), errorsOccured, errHandler->getSawErrors());
		retCode = ERR_FAILED;
		goto cleanup;
	}
cleanup:
	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	return retCode;
}

/*
void CXMLProc::SetSchemaName(const _TCHAR *schemaPrefix, const _TCHAR *schemaName)
{

	// For later - try to use MemBufInputSource and loadGrammar
	::_tcscpy(m_schemaName, schemaName);

    parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	_TCHAR externalSchemaLocation[512];
	_tcscpy(externalSchemaLocation, schemaPrefix);
	_tcscat(externalSchemaLocation, _T(" "));
	_tcscat(externalSchemaLocation, schemaName);

	parser->setExternalSchemaLocation(externalSchemaLocation);
	parser->setValidationScheme(XercesDOMParser::Val_Always);

	errHandler = new SAXErrorHandler();
	parser->setErrorHandler(errHandler);

}
*/

// Get/Set m_configFile
_TCHAR const * CXMLProc::GetConfigFile()
{
	return &m_configFile[0];
}
void CXMLProc::SetConfigFile(const _TCHAR *configFile)
{
	::_tcscpy(m_configFile, configFile);
}

// Get/Set m_element
_TCHAR const * CXMLProc::GetElement()
{
	return &m_element[0];
}
void CXMLProc::SetElement(const _TCHAR *element)
{
	::_tcscpy(m_element, element);
}

// Get/Set m_attribute
_TCHAR const * CXMLProc::GetAttribute()
{
	return &m_attribute[0];
}
void CXMLProc::SetAttribute(const _TCHAR *attribute)
{
	::_tcscpy(m_attribute, attribute);
}

// Get/Set m_value
_TCHAR const * CXMLProc::GetValue()
{
	return &m_value[0];
}
void CXMLProc::SetValue(const _TCHAR *value)
{
	::_tcscpy(m_value, value);
}

// Get/Set m_slashInversion
bool CXMLProc::IsInversible() 
{
	return m_slashInversion;
}

void CXMLProc::SetSlashInversion(bool inversion) 
{
	m_slashInversion = inversion;
}

void CXMLProc::InverseSlash()
{
	for (int i=0; i<_tcslen(m_value); i++) {
		if (m_value[i] == _T('\\'))
			m_value[i] = _T('/');
	}
}

// Get/Set m_removeTrailSlash
bool CXMLProc::IsRemoveTrailSlash()
{
	return m_removeTrailSlash;
}

void CXMLProc::SetRemoveTrailSlash(bool removeTrailSlash)
{
	m_removeTrailSlash = removeTrailSlash;
}

void CXMLProc::RemoveTrailSlash()
{
	int len = _tcslen(m_value);
//	_tprintf(_T("->here: %c %c\n"), m_value[len - 1], m_value[len - 2]);
	if (len >1) {
		if (m_value[len - 1] == _T('\"')) { // in case we have "C:\TEST\"
			m_value[len - 1] = _T('\0');
		}
		else if (m_value[len - 1] == _T('\\')) {
			m_value[len - 1] = _T('\0');
		}
	}
}

// Get/Set m_readMode
bool CXMLProc::IsReadMode()
{
	return m_readMode;
}
void CXMLProc::SetReadMode(bool readMode)
{
	m_readMode = readMode;
}

// Get/Set m_verbose
bool const CXMLProc::GetVerbose()
{
	return m_verbose;
}
void CXMLProc::SetVerbose(const bool verbose)
{
	m_verbose = verbose;
}

int CXMLProc::MakeConfigChange()
{
	int retCode = STEP_SUCCESS;
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CXMLProc::MakeConfigChange()");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

	try {
	assert(theDocument != 0);
	assert(theDOMSupport != 0);
	assert(thePrefixResolver != 0);
	XPathEvaluator	*theEvaluator = new XPathEvaluator();

//	_tprintf(_T("-->0\n"));
//	_tprintf(_T("Element: %s\n"), this->GetElement());
	assert(theEvaluator != 0);

//	XalanNode* theNode = NULL;

//	DOMNode* const	theClonedNode = theXercesDocument->getDocumentElement()->cloneNode(true);

//	XalanNode* const	theClonedNode = theDocument->cloneNode(true);
//	_tprintf(_T("-->01\n"));

//	_tprintf(_T("-->10\n"));
//	XercesParserLiaison *theLiaison = new XercesParserLiaison(*theDOMSupport);
//	_tprintf(_T("-->11\n"));

	// see here:
	// http://www.nabble.com/XercesDOMSupport:-Converting-between-Xalan-nodes-and-Xerces-DOM-nodes-for-removal-t606584.html
	XercesDocumentWrapper* docWrapper = 
	theLiaison->mapDocumentToWrapper(theDocument); 

	XalanNode* const	theNode =
							theEvaluator->selectSingleNode(
							*theDOMSupport,
							theDocument,
							XalanDOMString(this->GetElement()).c_str(),
							*thePrefixResolver); //XalanDocumentPrefixResolver(docWrapper)); 

//	theXercesDocument->getDocumentElement();

//	NodeRefList	theResult;
//	theEvaluator->selectNodeList(
//							theResult,
//							*theDOMSupport,
//							theDocument,
//							XalanDOMString(this->GetElement()).c_str(),
//							*thePrefixResolver);
//	MutableNodeRefList *nodes = new MutableNodeRefList(theResult, theResult.getMemoryManager());

//	if (nodes != NULL) {
//		theNode = nodes->item(0);
//	}

//	_tprintf(_T("-->1 attribute: %s, %s\n"), this->GetAttribute(), theNode->getNodeName());
	if (theNode != NULL)
	{
//		_tprintf(_T("-->12\n"));
		const DOMNode * xerces_node = docWrapper->mapNode(theNode); 

//		const XalanNamedNodeMap* attrs = theNode->getAttributes();
//		XalanNode* valueNode = attrs->getNamedItem(XalanDOMString(this->GetAttribute()));

//		_tprintf(_T("-->13\n"));
		const DOMNamedNodeMap* attrs = xerces_node->getAttributes();
		DOMNode* valueNode = attrs->getNamedItem(this->GetAttribute());
//		_tprintf(_T("-->2\n"));
		if (valueNode != NULL) {
//			_tprintf(_T("-->21 value: %s\n"), this->GetValue());
			if (!m_readMode)
				valueNode->setNodeValue(this->GetValue());
			else {
				_tprintf(_T("\n%s\n"), valueNode->getNodeValue());
				goto done;
			}
//			_tprintf(_T("-->22\n"));
		}

//		_tprintf(_T("-->2\n"));
//		if (valueNode != NULL) {
//			_tprintf(_T("-->21 value: %s\n"), this->GetValue());
//			valueNode->setNodeValue(XalanDOMString(_T("bbb")));
//			_tprintf(_T("-->22\n"));
//		}
//		else {
//			theNode->setAttribute(this->GetAttribute(), XalanDOMString(this->GetValue()));
//		}

			// serialize whole tree back
			// http://www-128.ibm.com/developerworks/xml/library/x-serial.html

			// DOMImplementationLS contains factory methods for creating objects
			// that implement the DOMBuilder and the DOMWriter interfaces
//			_tprintf(_T("-->25\n"));
			static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
			DOMImplementation *impl = 
				DOMImplementationRegistry::getDOMImplementation(gLS);

//			_tprintf(_T("-->3\n"));
			// construct the DOMWriter
			DOMWriter* myWriter = ((DOMImplementationLS*)impl)->createDOMWriter();

			// optionally, set some DOMWriter features
			// set the format-pretty-print feature
			if (myWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
				myWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

			// set the byte-order-mark feature      
//			if (myWriter->canSetFeature(XMLUni::fgDOMWRTBOM, true))
//				myWriter->setFeature(XMLUni::fgDOMWRTBOM, true);

			// serialize the DOMNode to a UTF-16 string
//			XMLCh* theXMLString_Unicode = myWriter->writeToString(*theXercesDocument->getDocumentElement());
//			XMLCh* theXMLString_Unicode = myWriter->writeToString(*theXercesDocument);

			// construct the LocalFileFormatTarget
			XMLFormatTarget *myFormatTarget = new LocalFileFormatTarget(this->GetConfigFile());

			// serialize a DOMNode to the local file "myXMLFile.xml"
			myWriter->writeNode(myFormatTarget, *theXercesDocument);

			// optionally, you can flush the buffer to ensure all contents are written
			myFormatTarget->flush();

			// release the memory
//			XMLString::release(&theXMLString_Unicode); 
			myWriter->release();
			delete myFormatTarget;
//		}
//		else {
//			// invalid attribute selector
//			::LoadString(hInst, IDS_ERR_ELEMENT, usage, 256);
//			::_tprintf(_TEXT("%s\n"), usage);
//		}
	}
	else {
		// invalid element selector
		::LoadString(hInst, IDS_ERR_ELEMENT, usage, 256);
		::_tprintf(_TEXT("%s\n"), usage);
	}

done:
	theEvaluator->~XPathEvaluator();
	}
	catch (const XMLException& e)
	{
		_tprintf(_T("XMLException: %s\n"), e.getMessage());
	}
	catch (const DOMException& e)
	{
		_tprintf(_T("DOMException: %s\n"), e.getMessage());
	}
	catch (const SAXParseException& e)
	{
		_tprintf(_T("SAXParseException: %s\n"), e.getMessage());
	}
	catch (const XercesDOMException& e)
	{
		_tprintf(_T("XercesDOMException: %d\n"), e.getExceptionCode());
	}
	catch (const XalanDOMException& e)
	{
		_tprintf(_T("XalanDOMException: %d\n"), e.getExceptionCode());
	}
	catch (...)
	{
		_tprintf(_T("Uncatched exception!\n"));
	}

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	return retCode;
}
