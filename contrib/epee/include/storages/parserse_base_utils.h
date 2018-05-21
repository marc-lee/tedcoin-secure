#pragma once 

namespace epee 
{
namespace misc_utils
{
  namespace parse
  {
    inline std::string transform_to_escape_sequence(const std::string& src)
    {
      //std::stringstream res;
      std::string res;
      for(std::string::const_iterator it = src.begin(); it!=src.end(); ++it)
      {
        switch(*it)
        {
        case '\b':  //Backspace (ascii code 08)
          res+="\\b"; break;
        case '\f':  //Form feed (ascii code 0C)
          res+="\\f"; break;
        case '\n':  //New line
          res+="\\n"; break;
        case '\r':  //Carriage return
          res+="\\r"; break;
        case '\t':  //Tab
          res+="\\t"; break;
        case '\v':  //Vertical tab
          res+="\\v"; break;
        //case '\'':  //Apostrophe or single quote
        //  res+="\\'"; break;
        case '"':  //Double quote
          res+="\\\""; break;
        case '\\':  //Backslash caracter
          res+="\\\\"; break;
        case '/':  //Backslash caracter
          res+="\\/"; break;
        default:
          res.push_back(*it);
        }
      }
      return res;
    }
    /*
      
      \b  Backspace (ascii code 08)
      \f  Form feed (ascii code 0C)
      \n  New line
      \r  Carriage return
      \t  Tab
      \v  Vertical tab
      \'  Apostrophe or single quote
      \"  Double quote
      \\  Backslash character

      */
      inline void match_string2(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        val.clear();
        bool escape_mode = false;
        std::string::const_iterator it = star_end_string;
        ++it;
        for(;it != buf_end;it++)
        {
          if(escape_mode/*prev_ch == '\\'*/)
          {
            switch(*it)
            {
            case 'b':  //Backspace (ascii code 08)
              val.push_back(0x08);break;
            case 'f':  //Form feed (ascii code 0C)
              val.push_back(0x0C);break;
            case 'n':  //New line
              val.push_back('\n');break;
            case 'r':  //Carriage return
              val.push_back('\r');break;
            case 't':  //Tab
              val.push_back('\t');break;
            case 'v':  //Vertical tab
              val.push_back('\v');break;
            case '\'':  //Apostrophe or single quote
              val.push_back('\'');break;
            case '"':  //Double quote
              val.push_back('"');break;
            case '\\':  //Backslash character
              val.push_back('\\');break;
            case '/':  //Slash character
              val.push_back('/');break;
            default:
              val.push_back(*it);
              LOG_PRINT_L0("Unknown escape sequence :\"\\" << *it << "\"");
            }
            escape_mode = false;
          }else if(*it == '"')
          {
            star_end_string = it;
            return;
          }else if(*it == '\\')
          {
            escape_mode = true;
          }          
          else
          {
            val.push_back(*it); 
          }
        }
        ASSERT_MES_AND_THROW("Failed to match string in json entry: " << std::string(star_end_string, buf_end));
      }
      inline bool match_string(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        try
        {

          match_string2(star_end_string, buf_end, val);
          return true;
        }
        catch(...)
        {
          return false;
        }
      }
      inline void match_number2(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val, bool& is_float_val, bool& is_signed_val)
      {
        val.clear();
        is_float_val = false;
        for(std::string::const_iterator it = star_end_string;it != buf_end;it++)
        {
          if(isdigit(*it) || (it == star_end_string && *it == '-') || (val.size() && *it == '.' ) || (is_float_val && (*it == 'e' || *it == 'E' || *it == '-' || *it == '+' )) )
          {
            if(!val.size() &&  *it == '-')
              is_signed_val = true;
            if(*it == '.' ) 
              is_float_val = true;
            val.push_back(*it);
          }
          else
          {
            if(val.size())
            {
              star_end_string = --it;
              return;
            }
            else 
              ASSERT_MES_AND_THROW("wrong number in json entry: " << std::string(star_end_string, buf_end));
          }
        }
        ASSERT_MES_AND_THROW("wrong number in json entry: " << std::string(star_end_string, buf_end));
      }
      inline bool match_number(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        try
        {
          bool is_v_float = false;bool is_signed_val = false;
          match_number2(star_end_string, buf_end, val, is_v_float, is_signed_val);
          return !is_v_float;
        }
        catch(...)
        {
          return false;
        }
      }
      inline void match_word2(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        val.clear();

        for(std::string::const_iterator it = star_end_string;it != buf_end;it++)
        {
          if(!isalpha(*it))
          {
            val.assign(star_end_string, it);
            if(val.size())
            {
              star_end_string = --it;
              return;
            }else 
              ASSERT_MES_AND_THROW("failed to match word number in json entry: " << std::string(star_end_string, buf_end));
          }
        }
        ASSERT_MES_AND_THROW("failed to match word number in json entry: " << std::string(star_end_string, buf_end));
      }
      inline bool match_word(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        try
        {
          match_word2(star_end_string, buf_end, val);
          return true;
        }
        catch(...)
        {
          return false;
        }
      }
      inline bool match_word_with_extrasymb(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        val.clear();

        for(std::string::const_iterator it = star_end_string;it != buf_end;it++)
        {
          if(!isalnum(*it) && *it != '-' && *it != '_')
          {
            val.assign(star_end_string, it);
            if(val.size())
            {
              star_end_string = --it;
              return true;
            }else 
              return false;
          }
        }
        return false;
      }
      inline bool match_word_til_equal_mark(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string::const_iterator& word_end)
      {
        word_end = star_end_string;

        for(std::string::const_iterator it = star_end_string;it != buf_end;it++)
        {
          if(isspace(*it))
          {

            continue;
          }else if( *it == '=' )
          {            
            star_end_string = it;
            word_end = it;
            return true;
          }
        }
        return false;
      }
  }
}
}