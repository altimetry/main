/*
* Copyright (C) 2005-2011 Collecte Localisation Satellites (CLS), France (31)
*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef _Xml_h_
#define _Xml_h_



#include "brathl.h" 

//#define XML_BUILDING_EXPAT
//#include "expat.h"

#include "Stl.h"

#include "List.h"
#include "BratObject.h"

namespace brathl
{

class CXmlNode;
class CXmlProperty;
class CXmlDocument;



//-------------------------------------------------------------
//------------------- CXmlProperty class --------------------
//-------------------------------------------------------------
// Represents node property(ies).
// Example: in <img src="hello.gif" id="3"/> "src" is property with value
//          "hello.gif" and "id" is prop. with value "3".

class CXmlProperty
{
public:
  CXmlProperty() : m_next(NULL) {}
  CXmlProperty(const string& name, const string& value,
                CXmlProperty *next = NULL)
          : m_name(name), m_value(value), m_next(next) {}
  virtual ~CXmlProperty() {}

  string GetName() const { return m_name; }
  string GetValue() const { return m_value; }
  CXmlProperty *GetNext() const { return m_next; }

  void SetName(const string& name) { m_name = name; }
  void SetValue(const string& value) { m_value = value; }
  void SetNext(CXmlProperty *next) { m_next = next; }

private:
  string m_name;
  string m_value;
  CXmlProperty *m_next;
};



//-------------------------------------------------------------
//------------------- CXmlNode class --------------------
//-------------------------------------------------------------
// Represents node in XML document. Node has name and may have content
// and properties. Most common node types are XML_TEXT_NODE (name and props
// are irrelevant) and XML_ELEMENT_NODE (e.g. in <title>hi</title> there is
// element with name="title", irrelevant content and one child (XML_TEXT_NODE
// with content="hi").
//
// All strings are encoded in the encoding UTF-8.

class CXmlNode : public CBratObject
{

public:
  // Represents XML node type.
  typedef enum   {
      // note: values are synchronized with xmlElementType from libxml
      XML_ELEMENT_NODE       =  1,
      XML_ATTRIBUTE_NODE     =  2,
      XML_TEXT_NODE          =  3,
      XML_CDATA_SECTION_NODE =  4,
      XML_ENTITY_REF_NODE    =  5,
      XML_ENTITY_NODE        =  6,
      XML_PI_NODE            =  7,
      XML_COMMENT_NODE       =  8,
      XML_DOCUMENT_NODE      =  9,
      XML_DOCUMENT_TYPE_NODE = 10,
      XML_DOCUMENT_FRAG_NODE = 11,
      XML_NOTATION_NODE      = 12,
      XML_HTML_DOCUMENT_NODE = 13
  } CXmlNodeType;


public:
  CXmlNode() : m_properties(NULL), m_parent(NULL),
                m_children(NULL), m_next(NULL) {}
  CXmlNode(CXmlNode *parent, CXmlNodeType type,
            const string& name, const string& content = "",
            CXmlProperty *props = NULL, CXmlNode *next = NULL);
  virtual ~CXmlNode();

  // copy ctor & operator=. Note that this does NOT copy syblings
  // and parent pointer, i.e. m_parent and m_next will be NULL
  // after using copy ctor and are never unmodified by operator=.
  // On the other hand, it DOES copy children and properties.
  CXmlNode(const CXmlNode& node);
  CXmlNode& operator=(const CXmlNode& node);

  void DeleteProperties();
  void DeleteChildren();

  // user-friendly creation:
  CXmlNode(CXmlNodeType type, const string& name, const string& content = "");
  virtual void AddChild(CXmlNode *child);
  virtual bool InsertChild(CXmlNode *child, CXmlNode *followingNode);
  bool InsertChildAfter(CXmlNode *child, CXmlNode *precedingNode);
  virtual bool RemoveChild(CXmlNode *child);
  virtual void AddProperty(const string& name, const string& value);
  virtual bool DeleteProperty(const string& name);

  // access methods:
  CXmlNodeType GetType() const { return m_type; }
  string GetName() const { return m_name; }
  string GetContent() const { return m_content; }

  bool IsWhitespaceOnly() const;
  int GetDepth(CXmlNode *grandparent = NULL) const;

  // Gets node content from XML_ENTITY_NODE
  // The problem is, <tag>content<tag> is represented as
  // XML_ENTITY_NODE name="tag", content=""
  //    |-- XML_TEXT_NODE or
  //        XML_CDATA_SECTION_NODE name="" content="content"
  string GetNodeContent() const;

  CXmlNode *GetParent() const { return m_parent; }
  CXmlNode *GetNext() const { return m_next; }
  CXmlNode *GetChildren() const { return m_children; }

  CXmlProperty *GetProperties() const { return m_properties; }
  bool GetPropVal(const string& propName, string *value) const;
  string GetPropVal(const string& propName, const string& defaultVal) const;
  bool HasProp(const string& propName) const;

  void SetType(CXmlNodeType type) { m_type = type; }
  void SetName(const string& name) { m_name = name; }
  void SetContent(const string& con) { m_content = con; }

  void SetParent(CXmlNode *parent) { m_parent = parent; }
  void SetNext(CXmlNode *next) { m_next = next; }
  void SetChildren(CXmlNode *child) { m_children = child; }

  void SetProperties(CXmlProperty *prop) { m_properties = prop; }
  virtual void AddProperty(CXmlProperty *prop);

private:
  CXmlNodeType m_type;
  string m_name;
  string m_content;
  CXmlProperty *m_properties;
  CXmlNode *m_parent, *m_children, *m_next;

  void DoCopy(const CXmlNode& node);
};



// special indentation value for CXmlDocument::Save
#define XML_NO_INDENTATION           (-1)


//-------------------------------------------------------------
//------------------- CXmlDocument class --------------------
//-------------------------------------------------------------
// This class holds XML data/document as parsed by XML parser.

class CXmlDocument : public CBratObject
{
public:
  // flags for CXmlDocument::Load
  typedef enum 
  {
    XMLDOC_NONE = 0,
    XMLDOC_KEEP_WHITESPACE_NODES = 1
  } CXmlDocumentLoadFlag;

  // flags for OutputStringEnt()
  typedef enum
  {
      XML_ESCAPE_QUOTES = 1
  } CXmlDocumentSaveFlag;

public:
  CXmlDocument();
  
  CXmlDocument(const string& filename, const string& encoding = CXmlDocument::m_DEFAULT_ENCODING);

  CXmlDocument(istream& stream, const string& encoding = CXmlDocument::m_DEFAULT_ENCODING);
  
  virtual ~CXmlDocument();

  CXmlDocument(const CXmlDocument& doc);
  CXmlDocument& operator=(const CXmlDocument& doc);

  void DeleteRoot();

  // Parses .xml file and loads data. Returns TRUE on success, FALSE
  // otherwise.
  virtual void Load(const string& filename, const string& encoding = CXmlDocument::m_DEFAULT_ENCODING, int flags = CXmlDocument::XMLDOC_NONE);
  virtual void Load(istream& stream, const string& encoding = CXmlDocument::m_DEFAULT_ENCODING, int flags = CXmlDocument::XMLDOC_NONE);
    
  // Saves document as .xml file.
  virtual bool Save(const string& filename, int indentstep = 1) const;
  virtual bool Save(ostream& stream, int indentstep = 1) const;

  virtual CXmlNode* FindNodeByName(const string& name, CXmlNode* parent, bool allDepths = false);
  virtual void FindAllNodesByName(const string& name, CXmlNode* parent, CObMap& mapNodes, const string& propNameKey, bool allDepths = false);
  virtual void FindAllNodesByName(const string& name, CXmlNode* parent, CObArray& arrayNodes, bool allDepths = false);

  bool IsOk() const { return m_root != NULL; }

  // Returns root node of the document.
  CXmlNode *GetRoot() const { return m_root; }

  // Returns version of document (may be empty).
  string GetVersion() const { return m_version; }
  // Returns encoding of document (may be empty).
  // Note: this is the encoding original file was saved in, *not* the
  // encoding of in-memory representation!
  string GetFileEncoding() const { return m_fileEncoding; }

  // Write-access methods:
  CXmlNode *DetachRoot() { CXmlNode *old=m_root; m_root=NULL; return old; }
  void SetRoot(CXmlNode *node) { DeleteRoot(); m_root = node; }
  void SetVersion(const string& version) { m_version = version; }
  void SetFileEncoding(const string& encoding) { m_fileEncoding = encoding; }

  // Returns encoding of in-memory representation of the document
  // (same as passed to Load or ctor, defaults to UTF-8).
  // NB: this is meaningless in Unicode build where data are stored as wchar_t*
  string GetEncoding() const { return m_encoding; }
  void SetEncoding(const string& enc) { m_encoding = enc; }


  // returns true if the given string contains only whitespaces
  static bool IsWhiteOnly(const char *buf);

  static void OutputString(ostream& stream, const string& str);
  // Same as OutputString, but create entities first.
  // Translates '<' to "&lt;", '>' to "&gt;" and '&' to "&amp;"
  static void OutputStringEnt(ostream& stream, const string& str, int flags = 0);
  static void OutputIndentation(ostream& stream, int indent);
  static void OutputNode(ostream& stream, CXmlNode *node, int indent, int indentstep);

public:
  static const string m_XML_VERSION;
  static const string m_UTF8_ENCODING;
  static const string m_DEFAULT_ENCODING;

private:
  string   m_version;
  string   m_fileEncoding;
  string   m_encoding;
  CXmlNode *m_root;

  void DoCopy(const CXmlDocument& doc);

};

}

#endif // _Xml_h_
