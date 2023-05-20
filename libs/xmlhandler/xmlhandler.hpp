#if !defined(XML_HANDLER_HPP)
#define XML_HANDLER_HPP

#include <map>
#include <mutex>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "../audioutil/audioutil.hpp"
#include "../rs232util/rs232util.hpp"
#include "../geoutil/geoutil.hpp"

const boost::regex regexPattern_("<\\?xml\\s.*?<\\/alert>");
// the tcpSocet writes to this class
class XmlHandler
{
public:
    XmlHandler(){}; 
    ~XmlHandler(){};
    void append(std::string host, const std::vector<char>&& tempBuffer,size_t size);
    static void clear_history(){if(history_.size()) history_.clear();} 
    void set_audioutil(AudioUtil *audioutil){audioutil_=audioutil;}
    void set_rs232util(RS232Util *rs232util){rs232util_ = rs232util;}
    void set_geoutil(GeoUtil *geoutil){geoutil_ = geoutil;}
private:
    AudioUtil *audioutil_;
    RS232Util *rs232util_;
    GeoUtil *geoutil_;
    void find_match(std::string host);
    std::map<std::string,std::string> fullBuffer_;
    std::queue<std::string> matches_;
    void process_matches(std::string host);
    static std::vector<std::string> history_;
    static std::mutex mut_;
    static bool check_update_history(std::string h)
    {
        std::lock_guard<std::mutex> lk(XmlHandler::mut_);
        std::cout << "checking history" << std::endl; 
        for(auto h_ : XmlHandler::history_)
        {
            if(h_ == h)
            {
                std::cout << "ALREADY HANDLED" << std::endl;
                return true;
            }
        }
        XmlHandler::history_.push_back(h);
        return false;
    }

};

#endif // XML_HANDLER_HPP
