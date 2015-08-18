#ifndef _LOAD_WORDNET_HH
# define _LOAD_WORDNET_HH

# include "info_helper.hh"
# include "wordnet.hh"

namespace wnb
{

  /// Load the entire wordnet data base located in \p dn (typically .../dict/)
  void load_wordnet(const std::string& dn, wordnet& wn, info_helper& info);
}

#endif /* _LOAD_WORDNET_HH */

