#ifndef _POS_T_HH
# define _POS_T_HH

namespace wnb
{

  static const std::size_t POS_ARRAY_SIZE = 5;
  static const char POS_ARRAY[POS_ARRAY_SIZE] = {'s', 'n', 'v', 'a', 'r'};

  enum pos_t
  	{
   		S       = 0,
    	N       = 1,
    	V       = 2,
    	A       = 3,
    	R       = 4,
    	UNKNOWN	= 5,
  	};


  inline pos_t get_pos_from_name(const std::string& pos)
  {
    if (pos == "adj")
      return A;
    if (pos == "noun")
      return N;
    if (pos == "adv")
      return R;
    if (pos == "verb")
      return V;
    return UNKNOWN;
  }

} // end of namespace wncpp


#endif /* _POS_T_HH */

