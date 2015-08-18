#ifndef _LOAD_WORDNET_HH
# define _LOAD_WORDNET_HH

# include "wordnet.hh"

namespace wnb
{

  /// Load the entire wordnet data base located in \p dn (typically .../dict/)
  void load_wordnet(const std::string& dn, wordnet& wn);
}

#endif /* _LOAD_WORDNET_HH */

