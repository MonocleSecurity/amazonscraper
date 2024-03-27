///// Includes /////

#include "parse.hpp"

#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

///// Functions /////

void TraverseList(xmlNode* node)
{
  if (node == nullptr)
  {
    // Ignore
    return;
  }
  for (xmlNode* cur_node = node; cur_node != nullptr; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {

      const xmlChar* href = xmlGetProp(cur_node, reinterpret_cast<const xmlChar*>("href"));
      if (href)
      {
        if (::xmlStrcmp(cur_node->name, reinterpret_cast<const xmlChar*>("a")) == 0) // Must be an "<a>" element
        {
          printf("%s\n\n", reinterpret_cast<const char*>(href));
        }


        //TODO class name needs to be "a"
        //TODO add to the list?

        std::string tmp(reinterpret_cast<const char*>(href));
        //TODO if (tmp.find("AmazonBasics") != std::string::npos)
        {
          //TODO printf("%s\n\n", reinterpret_cast<const char*>(href));
        }
      }
    }
    TraverseList(cur_node->children);
  }
}

void TraverseProduct(xmlNode* node)
{
  if (node == nullptr)
  {
    // Ignore
    return;
  }
  for (xmlNode* cur_node = node; cur_node != nullptr; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      //TODO trim

      const xmlChar* href = xmlGetProp(cur_node, reinterpret_cast<const xmlChar*>("href"));
      if (href)
      {
        std::string tmp(reinterpret_cast<const char*>(href));
        if (tmp.find("AmazonBasics") != std::string::npos)
          printf("%s\n\n", reinterpret_cast<const char*>(href));
      }
      int p = 0;//TODO printf("Node type: Text, name: %s\n", cur_node->name);
    }
    else if (cur_node->type == XML_TEXT_NODE)
    {
      //TODO trim

      //TODO printf("node type: Text, node content: %s\n", (char*)cur_node->name);
    }

    //TODO find attribute with hrefs

    TraverseProduct(cur_node->children);
  }
}

void ParseList(const std::string& html)
{
  xmlDocPtr doc = ::htmlReadMemory(html.data(), html.size(), nullptr, nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
  if (doc == nullptr)
  {
    //TODO output
    return;
  }
  const xmlNodePtr root_element = ::xmlDocGetRootElement(doc);
  if (root_element == nullptr)
  {
    //TODO output
    ::xmlFreeDoc(doc);
    return;
  }
  TraverseList(root_element);
  ::xmlFreeDoc(doc);
}

void ParseProduct(const std::string& html)
{
  xmlDocPtr doc = ::htmlReadMemory(html.data(), html.size(), nullptr, nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
  if (doc == nullptr)
  {
    //TODO output
    return;
  }
  const xmlNodePtr root_element = ::xmlDocGetRootElement(doc);
  if (root_element == nullptr)
  {
    //TODO output
    ::xmlFreeDoc(doc);
    return;
  }
  TraverseProduct(root_element);
  ::xmlFreeDoc(doc);
}
