#ifndef _POS_T_HH
# define _POS_T_HH

#include <string>

namespace wnb
{

  static const std::size_t POS_ARRAY_SIZE = 6;
  static const char POS_ARRAY[POS_ARRAY_SIZE] = { 'u', 'n', 'v', 'a', 'r', 's' };
  static const std::string POS_NAME[POS_ARRAY_SIZE] = { "UNKNOWN", "noun", "verb", "adj", "adv", "adj sat" };

  enum pos_t
  	{
        UNKNOWN = 0,
        N       = 1,
        V       = 2,
        A       = 3,
        R       = 4,
        S       = 5,
  	};


  inline pos_t get_pos_from_name(const std::string& pos)
  {
      for (std::size_t i = 0; i < POS_ARRAY_SIZE; ++i) {
          if (pos.compare(POS_NAME[i]) == 0) {
              return static_cast<pos_t>(i);
          }
      }
      return pos_t::UNKNOWN;    
  }

  inline std::string get_name_from_pos(const pos_t& pos)
  {
    return POS_NAME[pos];    
  }

  inline pos_t get_pos_from_char(const char& c)
  {
      for (std::size_t i = 0; i < POS_ARRAY_SIZE; ++i) {
          if (c == POS_ARRAY[i]) {
              return static_cast<pos_t>(i);
          }
      }
      return pos_t::UNKNOWN;
  }

} // end of namespace wncpp


#endif /* _POS_T_HH */

