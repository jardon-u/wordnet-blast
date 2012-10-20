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

  const unsigned int MAX_FORMS = 5; /* max # of different 'forms' word can have */

  const unsigned int ANTPTR =          1;	/* ! */
  const unsigned int HYPERPTR =        2;	/* @ */
  const unsigned int HYPOPTR =         3;	/* ~ */
  const unsigned int ENTAILPTR =       4;	/* * */
  const unsigned int SIMPTR =          5;	/* & */

  const unsigned int ISMEMBERPTR =     6;	/* #m */
  const unsigned int ISSTUFFPTR =      7;	/* #s */
  const unsigned int ISPARTPTR =       8;	/* #p */

  const unsigned int HASMEMBERPTR =    9;	/* %m */
  const unsigned int HASSTUFFPTR =    10;	/* %s */
  const unsigned int HASPARTPTR =     11;	/* %p */

  const unsigned int MERONYM =        12;  /* % (not valid in lexicographer file) */
  const unsigned int HOLONYM =        13;  /* # (not valid in lexicographer file) */
  const unsigned int CAUSETO =        14;  /* > */
  const unsigned int PPLPTR	=         15;  /* < */
  const unsigned int SEEALSOPTR	=     16;  /* ^ */
  const unsigned int PERTPTR =	    	17;  /* \ */
  const unsigned int ATTRIBUTE =	    18;  /* = */
  const unsigned int VERBGROUP =	    19;  /* $ */
  const unsigned int DERIVATION =     20;  /* + */
  const unsigned int CLASSIFICATION = 21;  /* ; */
  const unsigned int CLASS =          22;  /* - */

  const unsigned int LASTTYPE	= CLASS;

  /* Misc searches */

  const unsigned int SYNS      = (LASTTYPE + 1); 
  const unsigned int FREQ			 = (LASTTYPE + 2); 
  const unsigned int FRAMES		 = (LASTTYPE + 3); 
  const unsigned int COORDS		 = (LASTTYPE + 4); 
  const unsigned int RELATIVES = (LASTTYPE + 5); 
  const unsigned int HMERONYM  = (LASTTYPE + 6); 
  const unsigned int HHOLONYM	 = (LASTTYPE + 7); 
  const unsigned int WNGREP		 = (LASTTYPE + 8); 
  const unsigned int OVERVIEW	 = (LASTTYPE + 9); 

  const unsigned int MAXSEARCH =      OVERVIEW;

  const unsigned int CLASSIF_START =   (MAXSEARCH + 1);

  const unsigned int CLASSIF_CATEGORY = (CLASSIF_START);        /* ;c */
  const unsigned int CLASSIF_USAGE    = (CLASSIF_START + 1);    /* ;u */
  const unsigned int CLASSIF_REGIONAL = (CLASSIF_START + 2);    /* ;r */

  const unsigned int CLASSIF_END =     CLASSIF_REGIONAL;

  const unsigned int CLASS_START =     (CLASSIF_END + 1);

  const unsigned int CLASS_CATEGORY =  (CLASS_START);          /* -c */
  const unsigned int CLASS_USAGE    =  (CLASS_START + 1);      /* -u */
  const unsigned int CLASS_REGIONAL =  (CLASS_START + 2);      /* -r */

  const unsigned int CLASS_END =       CLASS_REGIONAL;

  const unsigned int INSTANCE  =       (CLASS_END + 1);        /* @i */
  const unsigned int INSTANCES =       (CLASS_END + 2);        /* ~i */

  const unsigned int MAXPTR =         INSTANCES;

  inline unsigned int bit(const unsigned int n)
  {
    return ((unsigned int)((unsigned int)1<<((unsigned int)n)));
  }

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

  struct  search_results{
    int SenseCount[MAX_FORMS];    /* number of senses word form has */
    int OutSenseCount[MAX_FORMS]; /* number of senses printed for word form */
    int numforms;                 /* number of word forms searchword has */
    int printcnt;                 /* number of senses printed by search */
    char *searchbuf;              /* buffer containing formatted results */
    synset * searchds;           /* data structure containing search results */

    search_results() : numforms(0), printcnt(0), searchbuf(0), searchds(0) { }
  };

  /// Wordnet interface class
  struct wordnet
  {
    typedef boost::adjacency_list<boost::vecS, boost::vecS,
                                  boost::directedS,
                                  synset, ptr> graph; ///< boost graph type

    /// Constructor
    wordnet(const std::string& wordnet_dir);

    index * getindex(std::string searchstr, int dbase);
    unsigned int is_defined(char *searchstr, int dbase);

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

