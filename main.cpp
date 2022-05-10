#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

using pos = std::tuple<double,double,double>;

using elemtnType = std::tuple<std::string,pos>; 

//the namespace of rapidxml
namespace rx = rapidxml;



//NOTE: DO NOT USE THIS FUNC WHEN YOU DEPLOY THIS 
void insertTab(unsigned int indent)
{
    for(int i = 0; i < indent + 1; ++i)
    {
        std::cout << ' ';
    }
    return;
}
void printNode(rx::xml_node<>* node, unsigned int indent = 0)
{
    if(node == nullptr) return;

    if( node -> name_size()  > 0)
    {
        insertTab(indent);
        std::cout << node-> name() << std::endl;
    }
     else
    {
        return;
    }
    
    if(node -> value_size() > 0 ) 
    {
      // insertTab(indent);
    //    std::cout << "Value: " << node -> value() << std::endl;
    }
    
    printNode(node -> first_node(), indent + 1);
    printNode(node -> next_sibling(), indent );
    
}

void traversal(rx::xml_node<>* node, std::vector<elemtnType>& out)
{
    if(node == nullptr)
    {
        return;
    }
    
    bool isGridData = false;
 
    if(node  -> name_size() > 0)
    {
        isGridData = (  node -> name() == std::string("gml:tupleList") );
        
    }
    
    if(isGridData)
    {
        std::cout << "found!" <<std::endl;

        std::stringstream ss{ std::string(node -> value())};
        // std::cout << ss.str() << std::endl;
        const char separator = '\n';
        
        std::string line{};
        
        while(getline(ss,line, separator ))
        {
            // WTF
            if(line.find(",")  == std::string::npos)
            continue;
            
             const std::string typeName = line.substr(0, line.find(","));

             // std::cout <<  typeName << std::endl;
             
             const std::string pos_z = line.substr(line.find(",") + 1,line.size() - line.find(","));
             
             // std::cout << typeName  << " <->  " << pos_z << std::endl;
             
             pos  t{0.0,0.0,std::stod(pos_z)};
             out.emplace_back(typeName,t);

             
        }
        return;
    }
    
    
    // traversal child node
    traversal(node -> first_node(),out);
    // traversal sibling node
    traversal( node -> next_sibling(),out);
    
    return;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "invalid arg. pleasse check inputs." << std::endl;
        exit(9);
    }

    const char* intputXMLFilename = argv[1];

    std::ifstream targetXML(intputXMLFilename);

    if(!targetXML)
    {
        std::cout <<  "can not open " << intputXMLFilename << std::endl;

        targetXML.close();
        exit(9);
    }

    for(std::string  line; std::getline(targetXML,line);)
    {
      //  std::cout << line << std::endl;
    }
    
    rx::xml_document<> doc;
    rx::file<> inputFile(intputXMLFilename);

    try
    {
        doc.parse<0>(inputFile.data());
    }
    catch (rx:: parse_error& err)
    {
        std::cout << "failed to parser xml data" << std::endl;
        targetXML.close();
        exit(9);
    }

    std::vector<elemtnType> out{};

    traversal(doc.first_node(),out);

    std::cout <<"out_size " <<  out.size() << std::endl;
    
    std::cout << intputXMLFilename << std::endl;
    
}
