#ifndef _POS_T_HH
# define _POS_T_HH

namespace wnb
{

  static const int  POS_ARRAY_SIZE = 5;
  static const char POS_ARRAY[POS_ARRAY_SIZE] = {'a', 'n', 'r', 'v', 's'};

  enum pos_t {
    A = 0,
    N,
    R,
    V,
    S,
    UNKNOWN
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

