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
    S
  };

} // end of namespace wncpp


#endif /* _POS_T_HH */

