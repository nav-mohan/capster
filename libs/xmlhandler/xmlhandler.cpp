#include "xmlhandler.hpp"
std::vector<std::string> XmlHandler::history_ = {};
std::mutex XmlHandler::mut_;

void XmlHandler::append(std::string host, const std::vector<char>&& tempBuffer,size_t size)
{
    fullBuffer_[host].insert(fullBuffer_[host].end(),tempBuffer.begin(),tempBuffer.begin() + size);
    find_match(host);
    process_matches(host);
}

void XmlHandler::find_match(std::string host)
{
    boost::sregex_iterator iter(fullBuffer_[host].begin(), fullBuffer_[host].end(), regexPattern_);
    boost::sregex_iterator end;
    while (iter != end)
    {
        matches_.emplace((fullBuffer_[host].substr(iter->position(),iter->length())));
        // std::cout << "ERASING " << " ON " << host << " " << iter->position() << " to " << iter->length() << std::endl;
        fullBuffer_[host].erase(iter->position(),iter->length()); 
        ++iter;
    }
}
void XmlHandler::process_matches(std::string host)
{
    if(matches_.empty()) return;
    // std::cout << "Processing " << host << std::endl;

    std::string y(std::move(matches_.front()));
    matches_.pop();
    std::stringstream ss(y);
    boost::property_tree::ptree tree;
    try
    {
        boost::property_tree::read_xml(ss,tree);
    }
    catch(const std::exception& e)
    {
        std::cout << "################# " << e.what() << " #################" << std::endl;
        std::cout << y << std::endl;
        std::cout << "#############################################" << std::endl;
    }
    
    
    boost::property_tree::ptree alert;
    boost::property_tree::ptree identifier;
    boost::property_tree::ptree sender;
    boost::property_tree::ptree sent;
    boost::property_tree::ptree status;
    boost::property_tree::ptree msgType;
    boost::property_tree::ptree scope;
    std::string resourceDesc;
    std::string mimeType;
    std::string uri;
    std::string size;
    std::string encodedData;
    try
    {
        alert = tree.get_child("alert");
        if(alert.find("identifier") != alert.not_found())
            identifier = alert.get_child("identifier");
        if(alert.find("sender") != alert.not_found())
            sender = alert.get_child("sender");
        if(alert.find("sent") != alert.not_found())
            sent = alert.get_child("sent");
        if(alert.find("status") != alert.not_found())
            status = alert.get_child("status");
        if(alert.find("msgType") != alert.not_found()) 
            msgType = alert.get_child("msgType");
        if(alert.find("scope") != alert.not_found())
            scope = alert.get_child("scope");

        if(status.data() == "System")
        {
            // std::cout << host << " Heartbeat " << identifier.data() << std::endl;
            return;
        }

        if(status.data() != "Actual")
        {
            std::cout << host << " received probably test " << status.data() << std::endl;
            return;
        }
        if(XmlHandler::check_update_history(identifier.data())) return;

        std::vector<boost::property_tree::ptree::iterator> infoNodes;
        for (auto it = alert.begin(); it != alert.end(); ++it) {
            if (it->first == "info")
                infoNodes.push_back(it);
        }
        if(infoNodes.size() == 0)
        {
            std::cout << "NO INFO NODES" << std::endl;
            return;
        } 
        for(auto info : infoNodes)
        {
            std::string language = "en-US"; //default language
            if(info->second.find("language") != info->second.not_found())
                language = info->second.get_child("language").data();
            
            // gather all <resource> nodes
            std::vector<boost::property_tree::ptree::iterator> resourceNodes;
            for(auto it = info->second.begin(); it!= info->second.end(); ++it)
            {
                if(it->first == "resource")
                    resourceNodes.push_back(it);
            }
            if(resourceNodes.size() == 0)
            {
                std::cout << "NO RESOURCE NODES" << std::endl;
                continue;
            }

            // gather all <area> nodes
            std::vector<boost::property_tree::ptree::iterator> areaNodes;
            for(auto it = info->second.begin(); it!= info->second.end(); it++)
            {
                if(it->first == "area")
                    areaNodes.push_back(it);
            }

            bool areaCovered;
            if(areaNodes.size() == 0 )
            {
                std::cout << " NO AREA NODES. ASSUME AREA RELEVANT " <<  std::endl;
                areaCovered = 1;
            }
            else
            {
                areaCovered = 0;
            }
            
            for(auto area : areaNodes)
            {
                if(area->second.find("polygon") != area->second.not_found())
                {
                    // check if polygon encomasses radio station
                    geoutil_->set_alertArea(area->second.get_child("polygon").data());
                    if (geoutil_->is_inside())
                    {
                        areaCovered = 1;
                        break;
                    }
                }
            }
            if(!areaCovered) 
            {
                std::cout << "NOT COVERED AREA" << std::endl;
                return;
            }
            std::cout << "ARE IS COVERED \n";
            // std::cout << y;
            // exit(1);

            // std::cout << "RESOURCE " << host << std::endl;
            for(auto resource : resourceNodes)
            {
                if(resource->second.find("resourceDesc") != resource->second.not_found())
                    resourceDesc = resource->second.get_child("resourceDesc").data(); // must be Broadcast Audio
                if(resource->second.find("mimeType") != resource->second.not_found())
                    mimeType = resource->second.get_child("mimeType").data();         // must be audio/mpeg, audio/wav, audio/x-wav, audio/x-ms-wma
                if(resource->second.find("size") != resource->second.not_found())
                    size = resource->second.get_child("size").data();
                if(resource->second.find("uri") != resource->second.not_found())
                {
                    uri = resource->second.get_child("uri").data().substr();         
                    // const std::regex url_re(R"!!(<\s*A\s+[^>]*href\s*=\s*"([^"]*)")!!", std::regex::icase);
                    // uri = std::regex_replace(uri, std::regex("^ +"), "");
                    boost::algorithm::trim(uri);

                }
                
                if(mimeType.substr(0,5)!="audio") 
                {
                    std::cout << "NOT AUDIO" << std::endl;
                    continue;
                }
                if(resourceDesc != "Broadcast Audio")
                {
                    std::cout << "NOT BROADCAST AUDIO" << std::endl;
                    continue;
                }


                std::string audioFileName = sent.data()+"-"+identifier.data()+"-"+language+".mp3";
                // audioFileName.replace(audioFileName.begin(),audioFileName.end(),'X','1');
                if(resource->second.find("derefUri") != resource->second.not_found())
                {
                    std::cout << "BASE64 ENCODED AUDIO" << std::endl;
                    encodedData = resource->second.get_child("derefUri").data();
                    audioutil_->decode_and_write(encodedData.c_str(),audioFileName.c_str());
                    rs232util_->set_pin();
                    audioutil_->play_mpeg(audioFileName.c_str());
                    rs232util_->clear_pin();
                    encodedData.clear();
                }
                if(uri.substr(0,4)=="http")
                {
                    std::cout << "DOWNLOAD REMOTE AUDIO" << std::endl;
                    audioutil_->download_file(uri.c_str(),audioFileName.c_str());
                    rs232util_->set_pin();
                    audioutil_->play_mpeg(audioFileName.c_str());
                    rs232util_->clear_pin();
                }


                std::cout << "resourceDesc = " << resourceDesc << std::endl;
                std::cout << "mimeType = " << mimeType << std::endl;
                std::cout << "size = " << size << std::endl;
                std::cout << "uri = " << uri << std::endl;
                std::cout << (encodedData.size()) << "ENCODED BYTES" << std::endl;


            }
        }
    }
    catch(boost::wrapexcept<boost::property_tree::ptree_bad_path> e)
    {
        std::cerr << "****************" << e.what() << "*****************" << std::endl;
        std::cout << identifier.data() << std::endl;
        std::cout << y << std::endl;
        std::cerr << "***************************************************" << std::endl;
    }
    
}
