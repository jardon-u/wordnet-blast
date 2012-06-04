#ifndef _WORDNET_HH
# define _WORDNET_HH

# include <iostream>
# include <string>
# include <cassert>
# include <vector>
//# include <boost/filesystem.hpp>

//Possible https://bugs.launchpad.net/ubuntu/+source/boost/+bug/270873
# include <boost/graph/graph_traits.hpp>
# include <boost/graph/adjacency_list.hpp>

# include "load_wordnet.hh"
# include "pos_t.hh"

namespace wnb
{

  /// More info here: http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html

  struct info_helper;

  /// Synset
  struct synset
  {
    int  synset_offset_;  ///< ***Deprecated*** @deprecated in-file wordnet
    int  lex_filenum;
    char ss_type_;       ///< ***Deprecated*** @deprecated unsafe pos
    int  w_cnt;
    std::vector<std::string> words;
    std::vector<int> lex_ids;
    int p_cnt;
    std::string gloss;

    // extra
    pos_t pos;        ///< pos
    int id;           ///< unique identifier

    bool operator==(const synset& s) const { return (id == s.id);  }
    bool  operator<(const synset& s) const { return (id < s.id);   }
  };


  /// Rel between synsets properties
  struct ptr
  {
    //std::string pointer_symbol; ///< symbol of the relation
    int pointer_symbol;
    int source; ///< source word inside synset
    int target; ///< target word inside synset
  };


  /// Index
  struct index
  {
    std::string lemma;
    char  pos_;        ///< ***Deprecated*** @deprecated unsafe pos
    int   synset_cnt;
    int   p_cnt;
    int   sense_cnt;
    float tagsense_cnt;
    std::vector<std::string> ptr_symbols;
    std::vector<int>         synset_offsets;

    // extra
    std::vector<int> ids;
    pos_t pos;

    bool operator<(const index& b) const
    {
      return (lemma.compare(b.lemma) < 0);
    }
  };


  /// Wordnet interface class
  struct wordnet
  {
    typedef boost::adjacency_list<boost::vecS, boost::vecS,
                                  boost::directedS,
                                  synset, ptr> graph; ///< boost graph type

    /// Constructor
    wordnet(const std::string& wordnet_dir);

    /// Return synsets matching word
    std::vector<synset> get_synsets(const std::string& word);

    //FIXME: todo
    std::vector<synset> get_synsets(const std::string& word, char pos);

    //FIXME: todo
    std::vector<synset> get_synset(const std::string& word, char pos, int i);

    std::string wordbase(const std::string& word, int ender);

    std::string morphword(const std::string& word, pos_t pos);

    std::vector<index> index_list;    ///< index list
    graph              wordnet_graph; ///< synsets graph
    info_helper        info;          ///< helper object

    typedef std::map<std::string,std::string> exc_t;
    std::map<pos_t, exc_t> exc;
  };

} // end of namespace wnb

#endif /* _WORDNET_HH */

