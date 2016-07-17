#include "pmd2_configloader.hpp"
#include <utils/pugixml_utils.hpp>
#include <utils/parse_utils.hpp>
#include <utils/library_wide.hpp>
#include <iostream>
#include <functional>
#include <deque>
using namespace std;
using namespace pugi;


namespace pmd2
{
    const std::array<std::string, static_cast<uint32_t>(eStringBlocks::NBEntries)> StringBlocksNames=
    {
        "Pokemon Names",
        "Pokemon Categories",
        "Move Names",
        "Move Descriptions",
        "Item Names",
        "Item Short Descriptions",
        "Item Long Descriptions",
        "Ability Names",
        "Ability Descriptions",
        "Type Names",
        "Portrait Names",
    };

    const std::array<std::string, static_cast<uint32_t>(eBinaryLocations::NbLocations)> BinaryLocationNames=
    {
        "Entities",           
        "Events",             
        "ScriptOpCodes",      
        "StartersHeroIds",    
        "StartersPartnerIds", 
        "StartersStrings",    
        "QuizzQuestionStrs",  
        "QuizzAnswerStrs",    
    };


    const std::array<std::string, static_cast<uint32_t>(eGameConstants::NbEntries)> GameConstantNames=
    {
        "NbPossibleHeros",
        "NbPossiblePartners",
        "NbUniqueEntities",
        "NbTotalEntities",

        //"Overlay11LoadAddress",
        //"Overlay13LoadAddress",
    };


    namespace ConfigXML
    {
        const string ROOT_PMD2      = "PMD2";

        const string NODE_GameEd    = "GameEditions";
        const string NODE_Game      = "Game";
        const string NODE_GConsts   = "GameConstants";
        const string NODE_Binaries  = "Binaries";
        const string NODE_Bin       = "Bin";
        const string NODE_Block     = "Block";
        const string NODE_StrIndex  = "StringIndexData";
        const string NODE_Language  = "Language";
        const string NODE_Languages = "Languages";
        const string NODE_StrBlk    = "StringBlock";
        const string NODE_StrBlks   = "StringBlocks";
        const string NODE_Value     = "Value";

        const string NODE_ExtFile   = "External";   //For external files to load config from

        const string ATTR_ID        = "id";
        const string ATTR_ID2       = "id2";
        const string ATTR_GameCode  = "gamecode";
        const string ATTR_Version   = "version";
        const string ATTR_Version2  = "version2";
        const string ATTR_Region    = "region";
        const string ATTR_ARM9Off   = "arm9off14";
        const string ATTR_DefLang   = "defaultlang";
        const string ATTR_Support   = "issupported";
        const string ATTR_Val       = "value";
        const string ATTR_Name      = "name";
        const string ATTR_Beg       = "beg";
        const string ATTR_End       = "end";
        const string ATTR_Loc       = "locale";
        const string ATTR_FName     = "filename";
        const string ATTR_FPath     = "filepath";
        const string ATTR_LoadAddr  = "loadaddress";
    };





    //GameVersionsData::GameVersionsData(gvcnt_t && gvinf)
    //    :BaseGameKeyValData(std::forward<gvcnt_t>(gvinf))
    //{}

    //GameVersionsData::const_iterator GameVersionsData::GetVersionForArm9Off14(uint16_t arm9off14) const
    //{
    //    for( auto it= m_kvdata.begin(); it != m_kvdata.end(); ++it )
    //    {
    //        if( it->second.arm9off14 == arm9off14 )
    //            return it;
    //    }
    //    return m_kvdata.end();
    //}

    //GameVersionsData::const_iterator GameVersionsData::GetVersionForId(const std::string & id) const
    //{
    //    return m_kvdata.find(id);
    //}

//
//
//
    //GameBinaryOffsets::GameBinaryOffsets(gvcnt_t && gvinf)
    //    :BaseGameKeyValData<GameBinLocCatalog>(), m_kvdata(std::forward<gvcnt_t>(gvinf))
    //{
    //    
    //}
    //GameBinaryOffsets::const_iterator GameBinaryOffsets::GetOffset(const std::string & gameversionid) const
    //{
    //    return m_kvdata.find(gameversionid);
    //}

//
//
//
    //GameConstantsData::GameConstantsData(gvcnt_t && gvinf)
    //{
    //}

    //GameConstantsData::const_iterator GameConstantsData::GetConstant(eGameVersion vers, const std::string & name) const
    //{
    //    return const_iterator();
    //}

//
//
//
    //class ConfigLoaderImpl
    //{
    //public:
    //    ConfigLoaderImpl( const string & file )
    //    {
    //    }
    //};


//
//
//
    class ConfigXMLParser
    {
    public:
        ConfigXMLParser(const std::string & configfile)
        {
            pugi::xml_parse_result result = m_doc.load_file( configfile.c_str() );
            if( !result )
                throw std::runtime_error("ConfigXMLParser::ConfigXMLParser(): Couldn't parse configuration file!");
        }

        void ParseDataForGameVersion( uint16_t arm9off14, ConfigLoader & target )
        {
            if( FindGameVersion(arm9off14) )
                DoParse(target);
            else
            {
                throw std::runtime_error("ConfigXMLParser::ParseDataForGameVersion(): Couldn't find a matching game version "
                                         "from comparing the value in the arm9.bin file at offset 0xE, to the values in the "
                                         "configuration file!");
            }
        }

        void ParseDataForGameVersion( eGameVersion version, eGameRegion region, ConfigLoader & target )
        {
            if( FindGameVersion(version, region) )
                DoParse(target);
            else
            {
                throw std::runtime_error("ConfigXMLParser::ParseDataForGameVersion(): Couldn't find a matching game version "
                                         "from comparing the region and version specified, to the values in the "
                                         "configuration file!");
            }
        }

    private:

        inline void ParseAllFields(xml_node & pmd2n)
        {
            using namespace ConfigXML;
            ParseLanguages(pmd2n);
            ParseBinaries (pmd2n);
            ParseConstants(pmd2n);

            for( auto & xternal : pmd2n.children(NODE_ExtFile.c_str()) )
            {
                xml_attribute xfp = xternal.attribute(ATTR_FPath.c_str());
                if(xfp)
                    HandleExtFile(xfp.value());
            }
        }

        void DoParse(ConfigLoader & target)
        {
            using namespace ConfigXML;
            xml_node pmd2node = m_doc.child(ROOT_PMD2.c_str());
            ParseAllFields(pmd2node);
            target.m_langdb      = std::move(LanguageFilesDB(std::move(m_lang)));
            target.m_binblocks   = std::move(m_bin);
            target.m_constants   = std::move(m_constants); 
            target.m_versioninfo = std::move(m_curversion); //Done in last
        }

        bool FindGameVersion( eGameVersion version, eGameRegion region )
        {
            using namespace pugi;
            using namespace ConfigXML;
            auto lambdafind = [version, region]( xml_node curnode )->bool
            {
                xml_attribute reg = curnode.attribute(ATTR_Region.c_str());
                xml_attribute ver = curnode.attribute(ATTR_Version.c_str());
                return (reg && ver) && ( ( StrToGameVersion(ver.value()) == version ) && ( StrToGameRegion(reg.value()) == region ) );
            };
            return FindGameVersion( lambdafind );
        }

        bool FindGameVersion(uint16_t arm9off14)
        {
            using namespace pugi;
            using namespace ConfigXML;
            auto lambdafind = [&]( xml_node curnode )->bool
            {
                xml_attribute foundattr = curnode.attribute(ATTR_ARM9Off.c_str());
                return foundattr && (static_cast<decltype(m_curversion.arm9off14)>(foundattr.as_uint()) == arm9off14);
            };
            return FindGameVersion( lambdafind );
        }
        
        bool FindGameVersion( std::function<bool(pugi::xml_node)> && predicate )
        {
            using namespace pugi;
            using namespace ConfigXML;
            xml_node gamed = m_doc.child(ROOT_PMD2.c_str()).child(NODE_GameEd.c_str());

            if( !gamed )
                return false;

            for( auto & curvernode : gamed.children(NODE_Game.c_str()) )
            {
                //GameVersionInfo curver;
                //xml_attribute foundattr = curvernode.attribute(ATTR_ARM9Off);

                //if( foundattr && static_cast<decltype(m_curversion.arm9off14)>(foundattr.as_uint()) == arm9off14 )
                if( predicate(curvernode) )
                {
                    for( auto & curattr : curvernode.attributes() )
                    {
                        if( curattr.name() == ATTR_ID )
                            m_curversion.id = curattr.value();
                        else if( curattr.name() == ATTR_GameCode )
                            m_curversion.code = curattr.value();
                        else if( curattr.name() == ATTR_Version )
                            m_curversion.version = StrToGameVersion(curattr.value());
                        else if( curattr.name() == ATTR_Region )
                            m_curversion.region = StrToGameRegion(curattr.value());
                        else if( curattr.name() == ATTR_ARM9Off )
                            m_curversion.arm9off14 = static_cast<decltype(m_curversion.arm9off14)>(curattr.as_uint());
                        else if( curattr.name() == ATTR_DefLang)
                            m_curversion.defaultlang = StrToGameLang( curattr.value() );
                        else if( curattr.name() == ATTR_Support)
                            m_curversion.issupported = curattr.as_bool();
                    }
                    return true;
                }
            }
            return false;
        }

        LanguageFilesDB::blocks_t::blkcnt_t ParseStringBlocks( pugi::xml_node & parentn )
        {
            using namespace ConfigXML;
            LanguageFilesDB::blocks_t::blkcnt_t stringblocks;
            for( auto strblk : parentn.child(NODE_StrBlks.c_str()).children(NODE_StrBlk.c_str()) )
            {
                string      blkn;
                strbounds_t bnd;
                for( auto att : strblk.attributes() )
                {
                    if( att.name() == ATTR_Name )
                        blkn = att.value();
                    else if( att.name() == ATTR_Beg )
                        bnd.beg = att.as_uint();
                    else if( att.name() == ATTR_End )
                        bnd.end = att.as_uint();
                }
                eStringBlocks strblock = StrToStringBlock(blkn);

                if( strblock != eStringBlocks::Invalid )
                    stringblocks.emplace( strblock, std::move(bnd) );
                else
                    clog <<"Ignored invalid string block \"" <<blkn <<"\".\n";
            }

            if( stringblocks.size() != static_cast<size_t>(eStringBlocks::NBEntries) )
            {
                stringstream sstr;
                sstr <<"ConfigXMLParser::ParseALang(): The ";
                for( size_t i = 0; i < static_cast<size_t>(eStringBlocks::NBEntries); ++i )
                {
                    if( stringblocks.find( static_cast<eStringBlocks>(i) ) == stringblocks.end() )
                    {
                        if( i != 0 )
                            sstr << ",";
                        sstr << StringBlocksNames[i];
                    }
                }
                if( (static_cast<size_t>(eStringBlocks::NBEntries) - stringblocks.size()) > 1 )
                    sstr << " string blocks for " <<m_curversion.id <<" are missing from the configuration file!";
                else
                    sstr << " string block for " <<m_curversion.id <<" is missing from the configuration file!";
                
                if(utils::LibWide().isLogOn())
                    clog << sstr.str();
                cout << sstr.str();
            }
            return std::move(stringblocks);
        }

        void ParseLanguageList( pugi::xml_node                       & gamev, 
                                LanguageFilesDB::blocks_t::blkcnt_t  & blocks, 
                                LanguageFilesDB::strfiles_t          & dest )
        {
            using namespace ConfigXML;
            using namespace pugi;

            for( auto lang : gamev.child(NODE_Languages.c_str()).children(NODE_Language.c_str()) )
            {
                xml_attribute fname    = lang.attribute(ATTR_FName.c_str());
                xml_attribute langname = lang.attribute(ATTR_Name.c_str());
                xml_attribute loc      = lang.attribute(ATTR_Loc.c_str());
                eGameLanguages elang   = StrToGameLang(langname.value());
                        
                if(elang >= eGameLanguages::NbLang )
                {
                    throw std::runtime_error("ConfigXMLParser::ParseLang(): Unexpected language string \"" + 
                                                std::string(langname.value()) + "\" in config file!!");
                }

                dest.emplace( fname.value(), std::move(LanguageFilesDB::blocks_t(fname.value(), loc.value(), elang, blocks )) );
            }
        }

        void ParseLanguages(xml_node & pmd2n)
        {
            using namespace ConfigXML;
            using namespace pugi;
            //LanguageFilesDB::strfiles_t dest;
            xml_node                    gamevernode = pmd2n.child(NODE_StrIndex.c_str());

            for( auto gamev : gamevernode.children(NODE_Game.c_str()) )
            {
                xml_attribute id   = gamev.attribute(ATTR_ID.c_str());
                xml_attribute id2  = gamev.attribute(ATTR_ID2.c_str());
                if( id.value() == m_curversion.id || id2.value() == m_curversion.id )   //Ensure one of the compatible game ids matches the parser's!
                    ParseLanguageList(gamev,ParseStringBlocks(gamev),m_lang);
            }
            //return std::move(dest);
        }

        void ParseBinaries(xml_node & pmd2n)
        {
            using namespace pugi;
            using namespace ConfigXML;
            xml_node binnode  = pmd2n.child(NODE_Binaries.c_str());
            xml_node foundver = binnode.find_child_by_attribute( NODE_Game.c_str(), ATTR_ID.c_str(), m_curversion.id.c_str() );

            if( !foundver )
            {
                throw std::runtime_error("ConfigXMLParser::ParseBinaries(): Couldn't find binary offsets for game version \"" + 
                                         m_curversion.id + "\"!" );
            }
            //GameBinariesInfo dest;

            for( auto curbin : foundver.children(NODE_Bin.c_str()) )
            {
                xml_attribute xfpath = curbin.attribute(ATTR_FPath.c_str());
                xml_attribute xlad   = curbin.attribute(ATTR_LoadAddr.c_str());
                binaryinfo binfo;
                binfo.loadaddress = xlad.as_uint();

                for( auto curblock : curbin.children(NODE_Block.c_str()) )
                {
                    binlocation curloc;
                    string      blkname;
                    for( auto att : curblock.attributes() )
                    {
                        if( att.name() == ATTR_Name )
                            blkname = att.value();
                        else if( att.name() == ATTR_Beg )
                            curloc.beg = att.as_uint();
                        else if( att.name() == ATTR_End )
                            curloc.end = att.as_uint();
                    }
                    binfo.blocks.emplace( StrToBinaryLocation(blkname), std::move(curloc) );
                }
                m_bin.AddBinary( std::string(xfpath.value()), std::move(binfo) );
            }

            //return std::move(dest);
        }


        void ParseConstants( xml_node & pmd2n )
        {
            using namespace ConfigXML;
            xml_node constnode  = pmd2n.child(NODE_GConsts.c_str());
            //ConfigLoader::constcnt_t dest;

            for( auto ver : constnode.children(NODE_Game.c_str()) )
            {
                xml_attribute xv1 = ver.attribute(ATTR_Version.c_str());
                xml_attribute xv2 = ver.attribute(ATTR_Version2.c_str());
                if( StrToGameVersion(xv1.value()) == m_curversion.version || StrToGameVersion(xv2.value()) == m_curversion.version )
                {
                    for( auto value : ver.children(NODE_Value.c_str()) )
                    {
                        xml_attribute xid  = value.attribute(ATTR_ID.c_str());
                        xml_attribute xval = value.attribute(ATTR_Val.c_str());
                        eGameConstants gconst = StrToGameConstant(xid.value());
                        if( gconst != eGameConstants::Invalid )
                            m_constants.emplace( gconst, std::move(std::string(xval.value())) );
                        else
                            clog << "<!>- Ignored unknown constant " <<xid.value() <<"\n";
                    }
                }
            }
            //return std::move(dest);
        }

        void HandleExtFile( const std::string & extfile )
        {
            using namespace ConfigXML;
            //When we hid an external file node, parse it too if possible

            bool bfoundsubdoc=false;
            for( const auto & entry: m_subdocs )
            {
                if( entry == extfile )
                {
                    bfoundsubdoc = true;
                    break;
                }
            }

            if( m_doc.path() != extfile && !bfoundsubdoc )
            {
                xml_document doc;
                m_subdocs.push_back(extfile);
                if( !(doc.load_file(extfile.c_str())) )
                {
                    m_subdocs.pop_back();
                    clog<<"<!>- ConfigXMLParser::HandleExtFile(): Couldn't open sub configuration file \"" <<extfile <<"\"!";
                    return;
                }
                xml_node pmd2node = doc.child(ROOT_PMD2.c_str());

                //Parse the data fields of the sub-file
                if(pmd2node)
                    ParseAllFields(pmd2node);
                m_subdocs.pop_back();
            }
            else
                clog<<"<!>- ConfigXMLParser::HandleExtFile(): External file node with same name as original document encountered! Ignoring to avoid infinite loop..\n";
        }

    private:
        GameVersionInfo                 m_curversion;
        pugi::xml_document              m_doc;
        std::deque<std::string>         m_subdocs;

        ConfigLoader::constcnt_t        m_constants;
        LanguageFilesDB::strfiles_t     m_lang;
        GameBinariesInfo                m_bin;
    };


//
//
//
    ConfigLoader::ConfigLoader(uint16_t arm9off14, const std::string & configfile)
        :m_conffile(configfile),m_arm9off14(arm9off14)
    {
        Parse(arm9off14);
    }

    ConfigLoader::ConfigLoader(eGameVersion version, eGameRegion region, const std::string & configfile)
    {
        Parse(version,region);
    }

    int ConfigLoader::GetGameConstantAsInt(eGameConstants gconst) const
    {
        return m_constants.GetConstAsInt<int>(gconst);
    }

    unsigned int ConfigLoader::GetGameConstantAsUInt(eGameConstants gconst) const
    {
        return m_constants.GetConstAsInt<unsigned int>(gconst);
    }

    void ConfigLoader::Parse( uint16_t arm9off14 )
    {
        ConfigXMLParser(m_conffile).ParseDataForGameVersion(arm9off14,*this);
    }

    void ConfigLoader::Parse(eGameVersion version, eGameRegion region)
    {
        ConfigXMLParser(m_conffile).ParseDataForGameVersion(version, region, *this);
    }


//
//
//
    //ConfigInstance ConfigInstance::s_instance;

};