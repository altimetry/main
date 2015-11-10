/*
* 
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

#include <fstream>
#include <sstream>
#include <cassert>

#define XML_BUILDING_EXPAT
#include "expat.h"

#include "Xml.h"

#include "Exception.h" 
#include "Tools.h"
using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CXmlNode class --------------------
//-------------------------------------------------------------

CXmlNode::CXmlNode(CXmlNode *parent,CXmlNodeType type,
                     const std::string& name, const std::string& content,
                     CXmlProperty *props, CXmlNode *next)
    : m_type(type), m_name(name), m_content(content),
      m_properties(props), m_parent(parent),
      m_children(NULL), m_next(next)
{
  if (m_parent)
  {
    if (m_parent->m_children)
    {
      m_next = m_parent->m_children;
      m_parent->m_children = this;
    }
    else
    {
      m_parent->m_children = this;
    }
  }
}

//----------------------------------------
CXmlNode::CXmlNode(CXmlNodeType type, const std::string& name,
                     const std::string& content)
    : m_type(type), m_name(name), m_content(content),
      m_properties(NULL), m_parent(NULL),
      m_children(NULL), m_next(NULL)
{
}

//----------------------------------------
CXmlNode::CXmlNode(const CXmlNode& node)
{
  m_next = NULL;
  m_parent = NULL;
  DoCopy(node);
}

//----------------------------------------
CXmlNode::~CXmlNode()
{
  CXmlNode *c, *c2;
  for (c = m_children; c; c = c2)
  {
    c2 = c->m_next;
    delete c;
  }

  CXmlProperty *p, *p2;
  for (p = m_properties; p; p = p2)
  {
    p2 = p->GetNext();
    delete p;
  }
}

//----------------------------------------
CXmlNode& CXmlNode::operator=(const CXmlNode& node)
{
  DeleteProperties();
  DeleteChildren();
  DoCopy(node);
  return *this;
}


//----------------------------------------
void CXmlNode::DeleteProperties()
{
  if ( m_properties != NULL ) 
  { 
    delete m_properties;
    m_properties = NULL; 
  }
}
//----------------------------------------
void CXmlNode::DeleteChildren()
{
  if ( m_children != NULL ) 
  { 
    delete m_children;
    m_children = NULL; 
  }
}

//----------------------------------------
void CXmlNode::DoCopy(const CXmlNode& node)
{
  m_type = node.m_type;
  m_name = node.m_name;
  m_content = node.m_content;
  m_children = NULL;

  CXmlNode *n = node.m_children;
  while (n)
  {
      AddChild(new CXmlNode(*n));
      n = n->GetNext();
  }

  m_properties = NULL;
  CXmlProperty *p = node.m_properties;
  while (p)
  {
      AddProperty(p->GetName(), p->GetValue());
      p = p->GetNext();
  }
}

//----------------------------------------
bool CXmlNode::HasProp(const std::string& propName) const
{
  CXmlProperty *prop = GetProperties();

  while (prop)
  {
    if (prop->GetName() == propName) 
    {
      return true;
    }

    prop = prop->GetNext();
  }

  return false;
}

//----------------------------------------
bool CXmlNode::GetPropVal(const std::string& propName, std::string *value) const
{
  if (value == NULL)
  {
    throw CXMLException("Error in CXmlNode::GetPropVal - value argument must not be NULL", BRATHL_LOGIC_ERROR);
  }

  CXmlProperty *prop = GetProperties();

  while (prop)
  {
    if (prop->GetName() == propName)
    {
      *value = prop->GetValue();
      return true;
    }
    prop = prop->GetNext();
  }

  return false;
}

//----------------------------------------
std::string CXmlNode::GetPropVal(const std::string& propName, const std::string& defaultVal) const
{
  std::string tmp;
  if (GetPropVal(propName, &tmp))
  {
    return tmp;
  }

  return defaultVal;
}

//----------------------------------------
void CXmlNode::AddChild(CXmlNode *child)
{
  if (m_children == NULL)
  {
    m_children = child;
  }
  else
  {
    CXmlNode *ch = m_children;
    while (ch->m_next) ch = ch->m_next;
    ch->m_next = child;
  }
  child->m_next = NULL;
  child->m_parent = this;
}

//----------------------------------------
bool CXmlNode::InsertChild(CXmlNode *child, CXmlNode *before_node)
{
  if ((before_node != NULL) && (before_node->GetParent() != this))
  {
    throw CXMLException("Error in CXmlNode::InsertChild - the node has incorrect parent", BRATHL_LOGIC_ERROR);
  }
  
  if (child == NULL)
  {
    throw CXMLException("Error in CXmlNode::InsertChild - Cannot insert a NULL node", BRATHL_LOGIC_ERROR);
  }

  if (m_children == before_node)
  {
    m_children = child;
  }
  else if (m_children == NULL)
  {
    if (before_node != NULL)
    {
      return false;       // we have no children so we don't need to search
    }
    m_children = child;
  }
  else if (before_node == NULL)
  {
    // prepend child
    child->m_parent = this;
    child->m_next = m_children;
    m_children = child;
    return true;
  }
  else
  {
    CXmlNode *ch = m_children;
    while (ch && ch->m_next != before_node) ch = ch->m_next;
    if (!ch)
    {
      return false;       // before_node not found
    }
    ch->m_next = child;
  }

  child->m_parent = this;
  child->m_next = before_node;
  return true;
}

//----------------------------------------
// inserts a new node right after 'precedingNode'
bool CXmlNode::InsertChildAfter(CXmlNode *child, CXmlNode *precedingNode)
{
  if (child == NULL)
  {
    throw CXMLException("Error in CXmlNode::InsertChildAfter - Cannot insert a NULL node", BRATHL_LOGIC_ERROR);
  }
  if (child->m_parent != NULL)
  {
    throw CXMLException("Error in CXmlNode::InsertChildAfter - node has already a parent", BRATHL_LOGIC_ERROR);
  }
  if (child->m_next != NULL)
  {
    throw CXMLException("Error in CXmlNode::InsertChildAfter - node has already m_next", BRATHL_LOGIC_ERROR);
  }
  if ((precedingNode != NULL) && (precedingNode->m_parent != this))
  {
    throw CXMLException("Error in CXmlNode::InsertChildAfter - precedingNode has wrong parent", BRATHL_LOGIC_ERROR);
  }

  if ( precedingNode != NULL )
  {
    child->m_next = precedingNode->m_next;
    precedingNode->m_next = child;
  }
  else // precedingNode == NULL
  {
    if (m_children != NULL)
    {
      throw CXMLException("Error in CXmlNode::InsertChildAfter - NULL precedingNode only makes sense when there are no children", BRATHL_LOGIC_ERROR);
    }

    child->m_next = m_children;
    m_children = child;
  }

  child->m_parent = this;
  return true;
}

//----------------------------------------
bool CXmlNode::RemoveChild(CXmlNode *child)
{
  if (m_children == NULL)
  {
    return false;
  }
  else if (m_children == child)
  {
    m_children = child->m_next;
    child->m_parent = NULL;
    child->m_next = NULL;
    return true;
  }
  else
  {
    CXmlNode *ch = m_children;
    while (ch->m_next)
    {
      if (ch->m_next == child)
      {
        ch->m_next = child->m_next;
        child->m_parent = NULL;
        child->m_next = NULL;
        return true;
      }
      ch = ch->m_next;
    }
    return false;
  }
}

//----------------------------------------
void CXmlNode::AddProperty(const std::string& name, const std::string& value)
{
  AddProperty(new CXmlProperty(name, value, NULL));
}
//----------------------------------------
void CXmlNode::AddProperty(CXmlProperty *prop)
{
  if (m_properties == NULL)
  {
    m_properties = prop;
  }
  else
  {
    CXmlProperty *p = m_properties;
    while (p->GetNext()) p = p->GetNext();
    p->SetNext(prop);
  }
}

//----------------------------------------
bool CXmlNode::DeleteProperty(const std::string& name)
{
  CXmlProperty *prop;

  if (m_properties == NULL)
  {
    return false;
  }
  else if (m_properties->GetName() == name)
  {
    prop = m_properties;
    m_properties = prop->GetNext();
    prop->SetNext(NULL);
    delete prop;
    return true;
  }
  else
  {
    CXmlProperty *p = m_properties;
    while (p->GetNext())
    {
      if (p->GetNext()->GetName() == name)
      {
        prop = p->GetNext();
        p->SetNext(prop->GetNext());
        prop->SetNext(NULL);
        delete prop;
        return true;
      }
      p = p->GetNext();
    }
    return false;
  }
}

//----------------------------------------
std::string CXmlNode::GetNodeContent() const
{
  CXmlNode *n = GetChildren();

  while (n)
  {
    if (n->GetType() == CXmlNode::XML_TEXT_NODE ||
        n->GetType() == CXmlNode::XML_CDATA_SECTION_NODE) 
    {
      return n->GetContent();
    }
    n = n->GetNext();
  }
  return "";
}
//----------------------------------------
int CXmlNode::GetDepth(CXmlNode *grandparent) const
{
  const CXmlNode *n = this;
  int ret = -1;

  do
  {
    ret++;
    n = n->GetParent();
    if (n == grandparent)
    {
        return ret;
    }

  } while (n);

  return -1;
}

//----------------------------------------
bool CXmlNode::IsWhitespaceOnly() const
{
    return CXmlDocument::IsWhiteOnly(m_content.c_str());
}
//----------------------------------------



//-------------------------------------------------------------
//------------------- CXmlDocument class --------------------
//-------------------------------------------------------------

const std::string CXmlDocument::m_XML_VERSION = "1.0";

const std::string CXmlDocument::m_UTF8_ENCODING = "UTF-8";
const std::string CXmlDocument::m_DEFAULT_ENCODING = CXmlDocument::m_UTF8_ENCODING;


CXmlDocument::CXmlDocument()
    : m_version(CXmlDocument::m_XML_VERSION), m_fileEncoding(CXmlDocument::m_DEFAULT_ENCODING), m_root(NULL)
{
    m_encoding = CXmlDocument::m_DEFAULT_ENCODING;
}

//----------------------------------------
CXmlDocument::CXmlDocument(const std::string& filename, const std::string& encoding)
              :CBratObject(), m_root(NULL)
{
  try
  {
    Load(filename, encoding);
  }
  catch(CException& e)
  {
    DeleteRoot();
    throw e;
  }
  catch(std::exception& e)
  {
    DeleteRoot();
    throw e;
  }
  catch(...)
  {
    DeleteRoot();
    throw CException("Error in CXmlDocument ctor - Unexpected error while loading the XML document - No Context and no message have been set for this error", BRATHL_ERROR);
  }



 }
//----------------------------------------
CXmlDocument::CXmlDocument(std::istream& stream, const std::string& encoding)
              :CBratObject(), m_root(NULL)
{
  try
  {
    Load(stream, encoding);
  }
  catch(CException& e)
  {
    DeleteRoot();
    throw e;
  }
  catch(std::exception& e)
  {
    DeleteRoot();
    throw e;
  }
  catch(...)
  {
    DeleteRoot();
    throw CException("Error in CXmlDocument ctor - Unexpected error while loading XML stream  - No Context and no message have been set for this error", BRATHL_ERROR);
  }
}
//----------------------------------------
CXmlDocument::CXmlDocument(const CXmlDocument& doc)
              :CBratObject()
{
    DoCopy(doc);
}
//----------------------------------------
CXmlDocument::~CXmlDocument()
{
  DeleteRoot();
}
//----------------------------------------
CXmlDocument& CXmlDocument::operator=(const CXmlDocument& doc)
{
    DeleteRoot();
    DoCopy(doc);
    return *this;
}
//----------------------------------------
void CXmlDocument::DeleteRoot()
{
  if ( m_root != NULL ) 
  { 
    delete m_root;
    m_root = NULL; 
  }
}
//----------------------------------------
void CXmlDocument::DoCopy(const CXmlDocument& doc)
{
  m_version = doc.m_version;
  m_encoding = doc.m_encoding;
  m_fileEncoding = doc.m_fileEncoding;

  if (doc.m_root)
  {
    m_root = new CXmlNode(*doc.m_root);
  }
  else
  {
    m_root = NULL;
  }

}

//----------------------------------------
void CXmlDocument::Load(const std::string& filename, const std::string& encoding, int flags)
{
  if (!CTools::FileExists(filename))
  {
    throw CFileException(CTools::Format("Error in CXmlDocument::Load - File '%s' doesn't exist", 
                                    filename.c_str()),
                      filename,                      
                      BRATHL_ERROR);

  }

  std::ifstream stream;    
  stream.open(filename.c_str());
    
  if (!stream.good()) 
  {
    std::ostringstream myStream;
    std::string errorCode = "?";
    if (myStream << stream.rdstate())
    {
      errorCode = myStream.str();
    }

    throw CException(CTools::Format("Error in CXmlDocument::Load while opening XML file '%s': the current internal error state flags of the stream is '%s'", 
                                    filename.c_str(),
                                    errorCode.c_str()),
                     BRATHL_ERROR);
  }
  
  Load(stream, encoding, flags);
}

//----------------------------------------
bool CXmlDocument::Save(const std::string& filename, int indentstep) const
{
  std::ofstream stream(filename.c_str());
  if (!stream.good())
  {
    return false;
  }

  return Save(stream, indentstep);
}

//----------------------------------------
bool CXmlDocument::IsWhiteOnly(const char *buf)
{
  for (const char *c = buf; *c != '\0'; c++)
  {
    if ((*c != ' ') && (*c != '\t') && (*c != '\n') && (*c != '\r')) 
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
//  CXmlDocument loading routines
//-----------------------------------------------------------------------------

struct structXmlParsingContext
{
  structXmlParsingContext()
      : //conv(NULL),
        root(NULL),
        node(NULL),
        lastChild(NULL),
        lastAsText(NULL),
        removeWhiteOnlyNodes(false)
  {}

  CXmlNode* root;
  CXmlNode* node;                    // the node being parsed
  CXmlNode* lastChild;               // the last child of "node"
  CXmlNode* lastAsText;              // the last _text_ child of "node"
  std::string encoding;
  std::string version;
  bool removeWhiteOnlyNodes;
};
//----------------------------------------
// checks that ctx->lastChild is in consistent state
#define ASSERT_LAST_CHILD_OK(ctx)                                   \
  assert( ctx->lastChild == NULL ||                             \
            ctx->lastChild->GetNext() == NULL );                  \
  assert( ctx->lastChild == NULL ||                             \
            ctx->lastChild->GetParent() == ctx->node )
//----------------------------------------
extern "C" {
static void StartElementHnd(void *userData, const char *name, const char **atts)
{
  structXmlParsingContext *ctx = (structXmlParsingContext*)userData;

  CXmlNode *node = new CXmlNode(CXmlNode::XML_ELEMENT_NODE, CTools::ToString(name));

  const char **a = atts;

  while (*a)
  {
    node->AddProperty(CTools::ToString(a[0]), CTools::ToString(a[1]));
    a += 2;
  }
  if (ctx->root == NULL)
  {
    ctx->root = node;
  }
  else
  {
    ASSERT_LAST_CHILD_OK(ctx);
    ctx->node->InsertChildAfter(node, ctx->lastChild);
  }

  ctx->lastAsText = NULL;
  ctx->lastChild = NULL; // our new node "node" has no children yet

  ctx->node = node;
}
}

//----------------------------------------
extern "C" {
static void EndElementHnd(void *userData, const char* name)
{
  structXmlParsingContext *ctx = (structXmlParsingContext*)userData;

  // we're exiting the last children of ctx->node->GetParent() and going
  // back one level up, so current value of ctx->node points to the last
  // child of ctx->node->GetParent()
  ctx->lastChild = ctx->node;

  ctx->node = ctx->node->GetParent();
  ctx->lastAsText = NULL;
}
}

//----------------------------------------
extern "C" {
static void TextHnd(void *userData, const char *s, int len)
{
  structXmlParsingContext *ctx = (structXmlParsingContext*)userData;
  std::string str = CTools::ToString(s, len);

  if (ctx->lastAsText)
  {
    ctx->lastAsText->SetContent(ctx->lastAsText->GetContent() + str);
  }
  else
  {
    bool whiteOnly = false;
    if (ctx->removeWhiteOnlyNodes)
    {
      whiteOnly = CXmlDocument::IsWhiteOnly(str.c_str());
    }

    if (!whiteOnly)
    {
      CXmlNode *textnode = new CXmlNode(CXmlNode::XML_TEXT_NODE, "text", str);
      ASSERT_LAST_CHILD_OK(ctx);
      ctx->node->InsertChildAfter(textnode, ctx->lastChild);
      ctx->lastChild= ctx->lastAsText = textnode;
    }
  }
}
}

//----------------------------------------
extern "C" {
static void StartCdataHnd(void *userData)
{
  structXmlParsingContext *ctx = (structXmlParsingContext*)userData;

  CXmlNode *textnode = new CXmlNode(CXmlNode::XML_CDATA_SECTION_NODE, "cdata", "");
  ASSERT_LAST_CHILD_OK(ctx);
  ctx->node->InsertChildAfter(textnode, ctx->lastChild);
  ctx->lastChild= ctx->lastAsText = textnode;
}
}

//----------------------------------------
extern "C" {
static void CommentHnd(void *userData, const char *data)
{
  structXmlParsingContext *ctx = (structXmlParsingContext*)userData;

  if (ctx->node != NULL)
  {
    // VS: ctx->node == NULL happens if there is a comment before
    //     the root element. We ignore such
    //     comments, no big deal...
    CXmlNode *commentnode = new CXmlNode(CXmlNode::XML_COMMENT_NODE, "comment", CTools::ToString(data));
    ASSERT_LAST_CHILD_OK(ctx);
    ctx->node->InsertChildAfter(commentnode, ctx->lastChild);
    ctx->lastChild = commentnode;
  }
  ctx->lastAsText = NULL;
}
}

//----------------------------------------
extern "C" {
static void DefaultHnd(void *userData, const char *s, int len)
{
  // XML header:
  if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
  {
    structXmlParsingContext *ctx = (structXmlParsingContext*)userData;

    std::string buf = CTools::ToString(s, (size_t)len);
    int pos = buf.find("encoding=");
    if (pos != -1)
    {
      std::string str = buf.substr(pos + 10);
      ctx->encoding = CTools::BeforeFirst(str, buf[(size_t)pos+9]);
    }
    pos = buf.find("version=");
    if (pos != -1) 
    {
      std::string str = buf.substr(pos + 9);
      ctx->version = CTools::BeforeFirst(str, buf[(size_t)pos+8]);
    }
  }
}
}


//----------------------------------------
void CXmlDocument::Load(std::istream& stream, const std::string& encoding, int flags)
{
    m_encoding = encoding;
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    structXmlParsingContext ctx;
    
    bool done = false;
    
    XML_Parser parser = XML_ParserCreate(NULL);

    ctx.root = NULL;
    ctx.node = NULL;
    ctx.encoding = CXmlDocument::m_DEFAULT_ENCODING; // default in absence of encoding=""
    if (!(CTools::CompareNoCase(encoding, CXmlDocument::m_DEFAULT_ENCODING)))
    {
      throw CUnImplementException(CTools::Format("Error in CXmlDocument::Load - '%s' encoding is not implemented , only '%s' is supported",
                                                  encoding.c_str(), 
                                                  CXmlDocument::m_DEFAULT_ENCODING.c_str()),
                                  BRATHL_UNIMPLEMENT_ERROR);
    }
    ctx.removeWhiteOnlyNodes = (flags & CXmlDocument::XMLDOC_KEEP_WHITESPACE_NODES) == 0;

    XML_SetUserData(parser, (void*)&ctx);
    XML_SetElementHandler(parser, StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(parser, TextHnd);
    XML_SetStartCdataSectionHandler(parser, StartCdataHnd);
    XML_SetCommentHandler(parser, CommentHnd);
    XML_SetDefaultHandler(parser, DefaultHnd);

    do
    {
        stream.read(buf, BUFSIZE);
        size_t len = static_cast<size_t>(stream.gcount());
        done = (len < BUFSIZE);
        if (!XML_Parse(parser, buf, len, done))
        {
            std::string error = XML_ErrorString(XML_GetErrorCode(parser));
            std::string msg = CTools::Format("XML parsing error: '%s' at line %lu",
                                        error.c_str(),
                                        static_cast<long>(XML_GetCurrentLineNumber(parser)));
            if ( ctx.root != NULL ) 
            { 
              delete ctx.root;
              ctx.root = NULL; 
            }

            XML_ParserFree(parser);

            throw CXMLParseException(msg, BRATHL_LOGIC_ERROR);
        }
    } while (!done);

    if (!ctx.version.empty())
    {
        SetVersion(ctx.version);
    }
    if (!ctx.encoding.empty())
    {
        SetFileEncoding(ctx.encoding);
    }

    SetRoot(ctx.root);
    
    XML_ParserFree(parser);

}



//-----------------------------------------------------------------------------
//  CXmlDocument saving routines
//-----------------------------------------------------------------------------

// write std::string to output:
inline void CXmlDocument::OutputString(std::ostream& stream, const std::string& str)
{
  if (str.empty())
  {
    return;
  }
  
  stream.write(str.c_str(), str.length());
}

//----------------------------------------
void CXmlDocument::OutputStringEnt(std::ostream& stream, const std::string& str, int flags /*= 0*/)
{
    std::string buf;
    size_t i;
    size_t last;
    size_t len;
    char c;

    len = str.length();
    last = 0;
    for (i = 0; i < len; i++)
    {
        c = str.at(i);
        if (c == '<' || c == '>' ||
          (c == '&' && str.substr(i+1, 4).compare("amp;") != 0 ) ||
            ((flags & CXmlDocument::XML_ESCAPE_QUOTES) && c == '"'))
        {
            CXmlDocument::OutputString(stream, str.substr(last, i - last));
            switch (c)
            {
                case '<':
                    CXmlDocument::OutputString(stream, "&lt;");
                    break;
                case '>':
                    CXmlDocument::OutputString(stream, "&gt;");
                    break;
                case '&':
                    CXmlDocument::OutputString(stream, "&amp;");
                    break;
                case '"':
                    CXmlDocument::OutputString(stream, "&quot;");
                    break;
                default:
                    break;
            }
            last = i + 1;
        }
    }
    CXmlDocument::OutputString(stream, str.substr(last, i - last));
}

//----------------------------------------
inline void CXmlDocument::OutputIndentation(std::ostream& stream, int indent)
{
    std::ostringstream ostrStream;
    ostrStream << "\n"; 
    for (int i = 0; i < indent; i++)
    {
      ostrStream << ' ' << ' ';
    }
    CXmlDocument::OutputString(stream, ostrStream.str());
}

//----------------------------------------
void CXmlDocument::OutputNode(std::ostream& stream, CXmlNode *node, int indent, int indentstep)
{
    CXmlNode *n = NULL;
    CXmlNode *prev = NULL;
    CXmlProperty *prop = NULL;

    switch (node->GetType())
    {
      //------------------------------------------
      case CXmlNode::XML_CDATA_SECTION_NODE:
      //------------------------------------------
      {
        CXmlDocument::OutputString( stream, "<![CDATA[");
        CXmlDocument::OutputString( stream, node->GetContent() );
        CXmlDocument::OutputString( stream, "]]>" );
        break;
      }

      //------------------------------------------
      case CXmlNode::XML_TEXT_NODE:
      //------------------------------------------
      {
        CXmlDocument::OutputStringEnt(stream, node->GetContent());
        break;
      }

      //------------------------------------------
      case CXmlNode::XML_ELEMENT_NODE:
      //------------------------------------------
      {
        CXmlDocument::OutputString(stream, "<");
        CXmlDocument::OutputString(stream, node->GetName());

        prop = node->GetProperties();
        while (prop)
        {
          CXmlDocument::OutputString(stream, " " + prop->GetName() +  "=\"");
          CXmlDocument::OutputStringEnt(stream, prop->GetValue(),
                          CXmlDocument::XML_ESCAPE_QUOTES);
          CXmlDocument::OutputString(stream, "\"");
          prop = prop->GetNext();
        }

        if (node->GetChildren())
        {
          CXmlDocument::OutputString(stream, ">");
          prev = NULL;
          n = node->GetChildren();
          while (n)
          {
            if (indentstep >= 0 && n && n->GetType() != CXmlNode::XML_TEXT_NODE)
            {
              CXmlDocument::OutputIndentation(stream, indent + indentstep);
            }
            CXmlDocument::OutputNode(stream, n, indent + indentstep, indentstep);
            prev = n;
            n = n->GetNext();
          }
          if (indentstep >= 0 && prev && prev->GetType() != CXmlNode::XML_TEXT_NODE)
          {
            CXmlDocument::OutputIndentation(stream, indent);
          }
          CXmlDocument::OutputString(stream, "</");
          CXmlDocument::OutputString(stream, node->GetName());
          CXmlDocument::OutputString(stream, ">");
        }
        else
        {
          CXmlDocument::OutputString(stream, "/>");
        }
        break;
      }

      //------------------------------------------
      case CXmlNode::XML_COMMENT_NODE:
      //------------------------------------------
      {
        CXmlDocument::OutputString(stream, "<!--");
        CXmlDocument::OutputString(stream, node->GetContent());
        CXmlDocument:: OutputString(stream, "-->");
        break;
      }

        
      //------------------------------------------
      default:
      //------------------------------------------
      {
        throw CXMLException(CTools::Format("Error in CXmlDocument::OutputNode - unsupported node type '%d'", node->GetType()), BRATHL_LOGIC_ERROR);
      }
    }
}

//----------------------------------------
bool CXmlDocument::Save(std::ostream& stream, int indentstep) const
{
    if ( !IsOk() ) 
    {
      return false;
    }

    std::string s = CTools::Format("<?xml version=\"%s\" encoding=\"%s\"?>\n",
                                GetVersion().c_str(), GetFileEncoding().c_str());
    CXmlDocument::OutputString(stream, s);

    CXmlDocument::OutputNode(stream, GetRoot(), 0, indentstep);
    CXmlDocument::OutputString(stream, "\n");

    return true;
}

//----------------------------------------------------
CXmlNode* CXmlDocument::FindNodeByName(const std::string& name, CXmlNode* parent, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    return NULL;
  }

  
  //wxXmlNode* node = NULL;
  CXmlNode* child =  parent->GetChildren();

  while (child != NULL)
  {
    if (CTools::CompareNoCase(child->GetName(), name))
    {
      break;
    }
    if ((allDepths) && (child->GetChildren() != NULL))
    {
      CXmlNode* newChild = FindNodeByName(name, child, allDepths);
      
      if (newChild != NULL)
      {
        child = newChild;
        break;
      }
    }

    child = child->GetNext();
  }

  return child;

}

//----------------------------------------------------
void CXmlDocument::FindAllNodesByName(const std::string& name, CXmlNode* parent, CObMap& mapNodes, const std::string& propNameKey, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    throw CXMLException("Error in CXmlDocument::FindAllNodesByName - Unable to get nodes because parent argument is NULL", BRATHL_LOGIC_ERROR);
  }

  mapNodes.SetDelete(false);

  
  CXmlNode* child =  parent->GetChildren();
  std::string value;

  while (child != NULL)
  {
    if (CTools::CompareNoCase(child->GetName(), name))
    {
      bool bOk = child->GetPropVal(propNameKey, &value);
      if (bOk)
      {
        mapNodes.Insert(value, child);
      }
      else
      {
        std::string msg = CTools::Format("Error in CXmlDocument::FindAllNodesByName - Unable to store node because node '%s' has no '%s' attribute", 
                                     child->GetName().c_str(),
                                     propNameKey.c_str());
        throw CXMLException(msg, BRATHL_LOGIC_ERROR);
      }
    }
    else
    {
      if ((allDepths) && (child->GetChildren() != NULL))
      {
        FindAllNodesByName(name, child, mapNodes, propNameKey, allDepths);
      }
    }

    child = child->GetNext();
  }

}

//----------------------------------------------------
void CXmlDocument::FindAllNodesByName(const std::string& name, CXmlNode* parent, CObArray& arrayNodes, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    throw CXMLException("Error in CXmlDocument::FindAllNodesByName - Unable to get nodes because parent argument is NULL", BRATHL_LOGIC_ERROR);
  }

  arrayNodes.SetDelete(false);

  
  CXmlNode* child =  parent->GetChildren();
  std::string value;

  while (child != NULL)
  {
    if (CTools::CompareNoCase(child->GetName(), name))
    {
      arrayNodes.Insert(child);
    }
    else
    {
      if ((allDepths) && (child->GetChildren() != NULL))
      {
        FindAllNodesByName(name, child, arrayNodes, allDepths);
      }
    }

    child = child->GetNext();
  }

}
} // end namespace