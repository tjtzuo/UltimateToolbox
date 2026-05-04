// HTMLParser.h: interface for the COXHTMLParser class.
//
//////////////////////////////////////////////////////////////////////
// Version: 9.3


#if !defined(AFX_HTMLPARSER_H__45F43BB2_0F0E_11D3_AA04_62B93C000000__INCLUDED_)
#define AFX_HTMLPARSER_H__45F43BB2_0F0E_11D3_AA04_62B93C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXParser.h"

struct TagDescriptor
{
    LPTSTR szTag;
    DWORD  dwTagFlag;
    DWORD  dwTextFlag;
};

class OX_CLASS_DECL COXHTMLParser : public COXParser  
{
    friend class CTagDescriptor;

protected:
    enum TAG_TYPES {
        TAG_BLOCK      = 0x00000001L,   // Block tag (eg <p>, <table> etc)
        TAG_OPTEND     = 0x00000002L,   // Optional ending tag. Block tag will end this tag
        TAG_EMPTY      = 0x00000004L,   // Empty tag (eg <br>)
        TAG_SECTION    = 0x00000008L,   // Section tag (<body>, <head> or <html>)
        TAG_CANCENTER  = 0x00000010L,   // Tag can be wrapped by <center></center>
        TAG_HEAD       = 0x00000020L,   // Tag belongs in <head> element
    };
    enum TEXT_FLAGS {
        TEXT_BIG       = 0x00000001L,
        TEXT_BOLD      = 0x00000002L,
        TEXT_CENTER    = 0x00000004L,
        TEXT_CITE      = 0x00000008L,
        TEXT_CODE      = 0x00000010L,
        TEXT_DEFN      = 0x00000020L,
        TEXT_EMPHASIS  = 0x00000040L,
        TEXT_FORMATTED = 0x00000080L,
        TEXT_ITALIC    = 0x00000100L,
        TEXT_KEYBOARD  = 0x00000200L,
        TEXT_PRE       = 0x00000400L,
        TEXT_SAMPLE    = 0x00000800L,
        TEXT_SMALL     = 0x00001000L,
        TEXT_STRIKE    = 0x00002000L,
        TEXT_STRONG    = 0x00004000L,
        TEXT_SUB       = 0x00008000L,
        TEXT_SUP       = 0x00010000L,
        TEXT_TELETYPE  = 0x00020000L,
        TEXT_UNDERLINE = 0x00040000L,
        TEXT_VAR       = 0x00080000L,
        TEXT_FONT      = 0x10000000L,
    };
    enum HTMLErrorTypes
    {
        ERROR_HTML_FIRST = 1000,        // All error messages start
        ERROR_HTML_LAST,                // All error messages end

        WARNING_HTML_FIRST = 2000,      // All warnings start
        WARNING_UNKNOWN_TAG,            // Unknown tag found
        WARNING_HTML_LAST,              // All warnings end
    };

public:
	COXHTMLParser();
	virtual ~COXHTMLParser();

protected:
    BOOL IsEmptyTag(LPCTSTR szTag)       { return IsEmptyTag(GetTagDescriptor(szTag));       }
    BOOL IsTextModifier(LPCTSTR szTag)   { return IsTextModifier(GetTagDescriptor(szTag));   }
    BOOL IsOptionalEndTag(LPCTSTR szTag) { return IsOptionalEndTag(GetTagDescriptor(szTag)); }
    BOOL IsSectionTag(LPCTSTR szTag)     { return IsSectionTag(GetTagDescriptor(szTag));     }
    BOOL IsBlockTag(LPCTSTR szTag)       { return IsBlockTag(GetTagDescriptor(szTag));       }
    BOOL IsHeadTag(LPCTSTR szTag)        { return IsHeadTag(GetTagDescriptor(szTag));        }
    BOOL CanCenter(LPCTSTR szTag)        { return CanCenter(GetTagDescriptor(szTag));        }

    BOOL IsEmptyTag(TagDescriptor *pTag);
    BOOL IsTextModifier(TagDescriptor *pTag);
    BOOL IsOptionalEndTag(TagDescriptor *pTag);
    BOOL IsSectionTag(TagDescriptor *pTag);
    BOOL IsBlockTag(TagDescriptor *pTag);
    BOOL IsHeadTag(TagDescriptor* pTag);
    BOOL CanCenter(TagDescriptor* pTag);

    TagDescriptor* GetTagDescriptor(LPCTSTR szTag) const;

    COXParserElement* DuplicateFontElement(COXParserElement* pElement);
    COXParserElement* ConstructFontElement();

    BOOL GetValueString(COXQuickString& str);

            
    UINT GetTextStyle()                 { return m_TextStyleStack.back();   }
    void SetTextStyle(UINT nStyle)      { m_TextStyleStack.back() = nStyle; }
    void PushTextStyle()                { m_TextStyleStack.push_back(0);    }
    void PopTextStyle()                 { m_TextStyleStack.pop_back();      }


public:

	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Removes all childs object and release 
	//              all memory allocated by the object
    virtual void Clear();

	// --- In  : 
	// --- Out : 
	// --- Returns : TRUE always.
	// --- Effect : Initializes members of the class.
    virtual BOOL Initialize();

	// --- In  : 
	// --- Out : 
	// --- Returns : TRUE always.
	// --- Effect : Do nothing, override it if you need it.
    virtual BOOL Cleanup();

	// --- In  : pParent - pointer to an object to parse text from.
	// --- Out : 
	// --- Returns : pointer to an object of type COXParser::STRING on success,
	//				 NULL otherwise.
	// --- Effect : Parses text element.
    virtual COXParserObject* ParseText(COXParserElement* pParent);

	// --- In  : pElement - pointer to an element to be added to.
	//			 pObject - pointer to an object to be added to the element
	// --- Out : 
	// --- Returns : 
	// --- Effect : Adds object to the element.
    virtual void AddObjectToElement(COXParserElement* pElement, COXParserObject* pObject);

	// --- In  : pElement - pointer to an object to parse attributes.
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise.
	// --- Effect : Parses attributes of an object.
    virtual BOOL ParseAttributes(COXParserElement* pElement);

	// --- In  : pParent - the parent of the element whose tag is being parsed.
	// --- Out : bEmptyTag - TRUE, if tag does not requiere end tag, like <BR>
	// --- Returns : pointer to an new object on success, NULL otherwise.
	// --- Effect : Parses start part of a tag.
    virtual COXParserElement* ParseStartTag(COXParserElement* pParent, BOOL& bEmptyTag);

	// --- In  : pElement - unused.
	//			 strEndTag - text to be parsed as end tag
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise.
	// --- Effect : Parses end part of a tag.
    virtual BOOL ParseEndTag(COXParserElement* pElement, COXQuickString& strEndTag);

	
	// --- In  : szCurrentTag - current tag.
	//			 szNewTag - new tag.
	//			 NewTagIsEndTag - specifies whether or not szNewTag is an end tag (TRUE)
	//								or a start tag (FALSE)
	// --- Out : 
	// --- Returns : TRUE if end tag should be,
	//				 FALSE if szNewTag represents a new child element of szCurrentTag
	// --- Effect : Handles tags.
    virtual BOOL IsEndTagMissing(LPCTSTR szCurrentTag, LPCTSTR szNewTag, 
                                 BOOL NewTagIsEndTag);

	// --- In  : pElement - unused.
	//			 bEmptyTag - unused.
	// --- Out : 
	// --- Returns : FALSE always.
	// --- Effect : Override this member function if you need it.
    virtual BOOL IgnoreStartTag(COXParserElement* pElement, BOOL bEmptyTag);

	// --- In  : szEndTag - end tag.
	// --- Out : 
	// --- Returns : TRUE if text style or font has been changed.
	// --- Effect : Determines if end has changed style or font of the text.
    virtual BOOL IgnoreEndTag(LPCTSTR szEndTag);

	// --- In  : hFile - a handle of a file to write attributes to.
	//			 pElement - a pointer to an object to write attributes from
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise
	// --- Effect : Saves attributes from  an object to specified file.
    virtual BOOL WriteAttributes(HANDLE hFile, COXParserElement* pElement);
 
	// --- In  : hFile - a handle of a file to write element to.
	//			 pElement - a pointer to an element to write
	//			 nLevel - not used
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise
	// --- Effect : Saves element to specified file.
	virtual BOOL WriteElement(HANDLE hFile, COXParserElement* pElement, int nLevel);

    
	// --- In  : hFile - a handle of a file to write text to.
	//			 pObject - a pointer to an object to write text from
	//			 nLevel - not used
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise
	// --- Effect : Saves text from an object to specified file.
	virtual BOOL WriteText(HANDLE hFile, COXParserObject* pObject, int nLevel);

	// --- In  : nErrorCode - error code.
	// --- Returns : pointer to string describing the error
	// --- Effect : Returns description of the error.
    virtual LPCTSTR TranslateErrorCode(int nErrorCode);

protected:
    static TagDescriptor m_HTMLTags[];          // List of known HTML tags
    static ParserEntity m_HTMLEntity[];         // List of known special char entities

    std::vector<COXParserElement*> m_FontStack;   // Current font stack
    std::vector<UINT> m_TextStyleStack;         // Current text style stack

    COXHashTable m_HTMLTagTable;                  // Lookup Table of HTML tags
};

#endif // !defined(AFX_HTMLPARSER_H__45F43BB2_0F0E_11D3_AA04_62B93C000000__INCLUDED_)
