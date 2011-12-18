#ifndef _INFO_HELPER_HH
# define _INFO_HELPER_HH

# include <string>
# include <stdexcept>
# include <map>

# include "pos_t.hh"

namespace wnb
{

  /// Symbols' size
  static const std::size_t NB_SYMBOLS = 27;
  /// List of pointer symbols
  static const char * symbols[NB_SYMBOLS] = {
    "!" ,  // 0 Antonym
    "@" ,  // 1 Hypernym
    "@i",  // 2 Instance Hypernym
    "~" ,  // 3 Hyponym
    "~i",  // 4 Instance Hyponym
    "#m",  // 5 Member holonym
    "#s",  // 6 Substance holonym
    "#p",  // 7 Part holonym
    "%m",  // 8 Member meronym
    "%s",  // 9 Substance meronym
    "%p",  // 10 Part meronym
    "=" ,  // 11 Attribute
    "+" ,  // 12 Derivationally related form
    ";c",  // 13 Domain of synset - TOPIC
    "-c",  // 14 Member of this domain - TOPIC
    ";r",  // 15 Domain of synset - REGION
    "-r",  // 16 Member of this domain - REGION
    ";u",  // 17 Domain of synset - USAGE
    "-u",  // 18 Member of this domain - USAGE

    //The pointer_symbol s for verbs are:
    "*",   // 19 Entailment
    ">",   // 20 Cause
    "^",   // 21 Also see
    "$",   // 22 Verb Group

    //The pointer_symbol s for adjectives are:
    "&",   // 23 Similar to
    "<",   // 24 Participle of verb
    "\\",  // 25 Pertainym (pertains to noun)
    "=",   // 26 Attribute
  };


  /// Useful information for wordnet in-memory import
  struct info_helper
  {
    typedef std::map<int,int>       mapii;       ///< indice / offset  correspondences
    typedef std::map<char, mapii*>  pos_maps_t;  ///< pos / map  correspondences

    /// Constructor
    info_helper() { update_pos_maps(); }

    /// Compute the number of synsets (i.e. the number of vertex in the graph)
    unsigned nb_synsets() {
      return adj_map.size() + adv_map.size() + noun_map.size() + verb_map.size(); 
    };

    // Given a pos return the starting indice in the graph
    //int get_indice_offset(pos_t pos);

    /// Helper function computing global indice in graph from local offset
    int compute_indice(int offset, pos_t pos);

    /// Update a map allowing one to get the correct map given a pos
    void update_pos_maps();

    int get_symbol(const std::string& ps)
    {
      unsigned i = 0;
      for (i = 0; i < NB_SYMBOLS; i++)
        if (ps == symbols[i])
          return i;
      throw std::runtime_error("Symbol NOT FOUND.");
    }

    pos_t get_pos(char c)
    {
      unsigned i = 0;

      for (i = 0;  i < POS_ARRAY_SIZE; i++)
        if (c == POS_ARRAY[i])
          return pos_t(i);
      throw std::runtime_error("pos NOT FOUND.");
    }

  public:

    mapii adj_map;
    mapii adv_map;
    mapii noun_map;
    mapii verb_map;

    pos_maps_t pos_maps;
    int indice_offset[5];
  };

  /// Create a new info_help based on wordnet data located in dn (../dict/)
  info_helper preprocess_wordnet(const std::string& dn);

} // end of namespace wncpp

#endif /* _INFO_HELPER_HH */

