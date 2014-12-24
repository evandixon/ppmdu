#include <ppmdu/fmts/content_type_analyser.hpp>
#include <limits>

namespace pmd2 { namespace filetypes
{
//==================================================================
// CContentHandler
//==================================================================
    CContentHandler::CContentHandler()
        :m_current_ruleid(1) //begin at 1, because 0 is reserved
    {

    }

    CContentHandler::~CContentHandler()
    {

    }

    CContentHandler & CContentHandler::GetInstance()
    {
        //Instantiate the singleton
        static CContentHandler s_singleton;
        return s_singleton;
    }

    //Rule registration handling
    content_rule_id_t CContentHandler::RegisterRule( IContentHandlingRule * rule )
    {
        if( rule )
        {
            //Check if rule already in!
            for( auto &arule : m_vRules )
            {
                if( arule.get() == rule )
                    return arule->getRuleID();
            }

            //If not already in, put it in the rule list!
            //Set the rule id
            rule->setRuleID( ++m_current_ruleid );
            m_vRules.push_back( std::unique_ptr<IContentHandlingRule>( rule ) );

            return m_current_ruleid;
        }

        return std::numeric_limits<unsigned int>::max();// -1;
    }

    bool CContentHandler::UnregisterRule( content_rule_id_t ruleid )
    {
        for( auto &arule : m_vRules )
        {
            if( arule->getRuleID() == ruleid )
            {
                delete arule.release();
                return true;
            }
        }
        return false;
    }

    //File analysis
    //ContentBlock CContentHandler::AnalyseContent( types::constitbyte_t itdatabeg, types::constitbyte_t itdataend )
    ContentBlock CContentHandler::AnalyseContent( const analysis_parameter & parameters )
    {
        ContentBlock contentdetails;

        //Feed the data through all the rules we have and check which one returns true
        for( auto &rule : m_vRules )
        {
            if( rule->isMatch( parameters._itdatabeg, parameters._itdataend, parameters._filextension ) )
            {
                contentdetails = rule->Analyse( parameters );
                break;
            }
        }

        return contentdetails;
    }

    bool CContentHandler::isValidRule( content_rule_id_t theid )const 
    { 
        return (theid != INVALID_RULE_ID && theid < m_current_ruleid); 
    }

};};