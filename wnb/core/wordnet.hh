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

# include "wnb_export.h"
# include "pos_t.hh"
# include "info_helper.hh"

namespace wnb
{

  /// More info here: http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html

  /// Synset
  struct synset
  {
    std::size_t  lex_filenum;
    std::size_t  w_cnt;
    std::vector<std::string> words;
    std::vector<std::size_t> lex_ids;
    std::size_t p_cnt;
    std::string gloss;

    // extra
    pos_t pos;        ///< pos (replace ss_type)
    std::size_t id;           ///< unique identifier (replace synset_offset)
    std::size_t sense_number; ///< http://wordnet.princeton.edu/man/senseidx.5WN.html
    std::vector<std::pair<std::string, std::size_t> > tag_cnts; ///< http://wordnet.princeton.edu/man/senseidx.5WN.html

    bool operator==(const synset& s) const { return (id == s.id);  }
    bool operator<(const synset& s) const { return (id < s.id);   }
  };


  /// Rel between synsets properties
  struct ptr
  {
    //std::string pointer_symbol; ///< symbol of the relation
    std::size_t pointer_symbol;
    std::size_t source; ///< source word inside synset
    std::size_t target; ///< target word inside synset
  };


  /// Index
  struct index
  {
    std::string lemma;

    std::size_t synset_cnt;
    std::size_t p_cnt;
    std::size_t sense_cnt;
    float       tagsense_cnt;
    std::vector<std::string> ptr_symbols;
    std::vector<std::size_t> synset_offsets;

    // extra
    std::vector<std::size_t> synset_ids;
    pos_t pos;

    bool operator<(const index& b) const
    {
      return (lemma.compare(b.lemma) < 0);
    }
  };


  /// Wordnet interface class
  struct WNB_EXPORT wordnet
  {
    typedef boost::adjacency_list<boost::vecS, boost::vecS,
                                  boost::bidirectionalS,
                                  synset, ptr> graph; ///< boost graph type

    /// Constructor
    wordnet(const std::string& wordnet_dir, bool verbose = false);
    wordnet(const std::string& wordnet_dir, const info_helper& info, bool verbose = false);

    /// Return synsets matching word
    std::vector<synset> get_synsets(const std::string& word, pos_t pos = pos_t::UNKNOWN) const;
    //FIXME: todo
    std::vector<synset> get_synset(const std::string& word, char pos, int i) const;

	std::pair<std::vector<index>::const_iterator, std::vector<index>::const_iterator>
    get_indexes(const std::string& word) const;

    std::string wordbase(const std::string& word, std::size_t ender) const;

    std::string morphword(const std::string& word, pos_t pos) const;

    std::vector<index> index_list;    ///< index list // FIXME: use a map
    graph              wordnet_graph; ///< synsets graph
    info_helper        info;          ///< helper object
    bool               _verbose;

    typedef std::map<std::string,std::string> exc_t;
    std::map<pos_t, exc_t> exc;
  };

} // end of namespace wnb

#endif /* _WORDNET_HH */

