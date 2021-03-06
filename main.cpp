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

struct dem10bPointData {
    double x,y,z;
    std::string type;
};

class dem10bParser{
    public:
    dem10bParser(std::string fileName_) : fileName(fileName_), inputFile(rx::file<>(fileName_.c_str())) 
    {};
    
    void generateDocumentFile()
    {
        
        try
        {
            doc.parse<0>(inputFile.data());
        }
        catch (rx:: parse_error& err)
        {
            std::cout << "failed to parser xml data" << std::endl;
            exit(9);
        }
    }

    void traversal()
    {
        traversal_ (doc.first_node());
    }

    void ceratePointMatrix()
    {
        pointMatrix = std::vector<std::vector<dem10bPointData>>(maxY,std::vector<dem10bPointData>(maxX));
        
         for(unsigned int i = 0; i < out.size(); ++i)
        {
            const int iy = i / maxX;
            const int ix = i - iy *maxX;
            
            pointMatrix[iy][ix] = out[i];
        }
    }

    // gettter
    unsigned int getMaxX()
    {
        return maxX;
    }

    unsigned int getMaxY()
    {
        return maxY;
    }
    unsigned int getStartX()
    {
        return sx;
    }
    
    unsigned int getStartY()
    {
        return sy;
    }

    std::string getTargetFileName()
    {
        return fileName;
    }
    
    auto getPointMatirx() -> const std::vector<std::vector<dem10bPointData>>& 
    {
        return pointMatrix;
    }
    private:

    void traversal_(rx::xml_node<>* node)
    {
        if(node == nullptr)
        {
            return;
        }
        
        bool isGridDataNode = false;
        bool isStartPointNode = false;
        bool isGridBoundaryInfoNode = false;
        
    
        if(node  -> name_size() > 0)
        {
            isGridDataNode = (  node -> name() == std::string("gml:tupleList") );
            isStartPointNode = (node -> name() == std::string("gml:startPoint"));
            isGridBoundaryInfoNode = ( node -> name()  == std::string("gml:high"));

            
        }
        
        if(isGridDataNode)
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
                
                out.push_back(dem10bPointData{0.0,0.0,std::stod(pos_z),typeName});
                
            }
        }
         else  if(isStartPointNode)
        {
            std::string pos_sx{}, pos_sy{};

            bool hasCehckedSeparator = false;
            const char separator = ' ';

            for(const char c : std::string(node -> value()))
            {
                if(c == separator )
                {
                    hasCehckedSeparator = true;
                } else 
                {
                    (hasCehckedSeparator  ?  pos_sx : pos_sy).push_back(c);
                }
            }
            sx = static_cast<unsigned int> (std::stoi(pos_sx));
            sy = static_cast<unsigned int> (std::stoi(pos_sy));
        }
        else if(isGridBoundaryInfoNode)
        {
            std::string str1{}, str2{};

            bool hasCehckedSeparator = false;
            const char separator = ' ';

            for(const char c : std::string(node -> value()))
            {
                if(c == separator )
                {
                    hasCehckedSeparator = true;
                } else 
                {
                    (hasCehckedSeparator  ?  str1 : str2).push_back(c);
                }
            }
            maxY = static_cast<unsigned int> (std::stoi(str1) + 1);
            maxX = static_cast<unsigned int> (std::stoi(str2) + 1);
            
            std::cout << maxX << " " << maxY << std::endl;
        }
        
        // traversal child node
        traversal_(node -> first_node());
        // traversal sibling node
        traversal_( node -> next_sibling());
        
        return;
    }

    rx::xml_document<> doc;
    rx::file<> inputFile;

    
    std::string fileName;
    unsigned int maxX,maxY;
    unsigned int sx, sy;
    std::vector<dem10bPointData>out;
    std::vector<std::vector<dem10bPointData>> pointMatrix;
    
};



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

void traversal(rx::xml_node<>* node, std::vector<elemtnType>& out, unsigned int& sx, unsigned int& sy)
{
    if(node == nullptr)
    {
        return;
    }
    
    bool isGridDataNode = false;
    bool isStartPointNode = false;
 
    if(node  -> name_size() > 0)
    {
        isGridDataNode = (  node -> name() == std::string("gml:tupleList") );
        isStartPointNode = (node -> name() == std::string("gml:startPoint"));
        
    }
    
    if(isGridDataNode)
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
    }  else  if(isStartPointNode)
    {
        std::string pos_sx{}, pos_sy{};

        bool hasCehckedSeparator = false;
        const char separator = ' ';

        for(const char c : std::string(node -> value()))
        {
            if(c == separator )
            {
                hasCehckedSeparator = true;
            } else 
            {
                (hasCehckedSeparator  ?  pos_sx : pos_sy).push_back(c);
            }
        }
        sx = static_cast<unsigned int> (std::stoi(pos_sx));
        sy = static_cast<unsigned int> (std::stoi(pos_sy));
        
    }
    
    
    // traversal child node
    traversal(node -> first_node(),out,sx,sy);
    // traversal sibling node
    traversal( node -> next_sibling(),out,sx,sy);
    
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

    dem10bParser d10Parser(intputXMLFilename);
    
    d10Parser.generateDocumentFile();
    d10Parser.traversal();
    d10Parser.ceratePointMatrix();
    
    

    std::cout << d10Parser.getTargetFileName() << std::endl;
    assert(d10Parser.getMaxX() == 1125);
    assert(d10Parser.getMaxY() == 750);
    assert(d10Parser.getStartY() ==d10Parser.getStartX() && d10Parser.getStartX() == 0);
    assert (d10Parser.getPointMatirx().size() == d10Parser.getMaxY());
    
    // std::ifstream targetXML(intputXMLFilename);

    // if(!targetXML)
    // {
    //     std::cout <<  "can not open " << intputXMLFilename << std::endl;

    //     targetXML.close();
    //     exit(9);
    // }

    // for(std::string  line; std::getline(targetXML,line);)
    // {
    //   //  std::cout << line << std::endl;
    // }
    
    // rx::xml_document<> doc;
    // rx::file<> inputFile(intputXMLFilename);

    // try
    // {
    //     doc.parse<0>(inputFile.data());
    // }
    // catch (rx:: parse_error& err)
    // {
    //     std::cout << "failed to parser xml data" << std::endl;
    //     targetXML.close();
    //     exit(9);
    // }
    // unsigned int sx  = 123, sy = 123;

    // std::vector<elemtnType> out{};

    // traversal(doc.first_node(),out,sx,sy);

    // std::cout <<"out_size " <<  out.size() << std::endl;
    // std::cout << sx << sy << std::endl;

    // const unsigned int  maxY  = 750;
    // const unsigned int  maxX = 1125;

    // std::vector<std::vector<pos>> pointMatrix(maxY, std::vector<pos>(maxX));

    // for(unsigned int i = 0; i < out.size(); ++i)
    // {
    //     const int iy = i / maxX;
    //     const int ix = i - iy *maxX;
        
    //     pointMatrix[iy][ix] = std::get<1>(out[i]);
        
    // }

    // for(auto & v : pointMatrix)
    // {
    //     for(auto [x,y,z] : v)
    //     {
    //         std::cout << ( abs(z + 9999) < 1e-6 ?  " " : "X")  << " ";
    //     }

    //     std::cout << std::endl;
    // }
    // std::cout << intputXMLFilename << std::endl;
    
}
