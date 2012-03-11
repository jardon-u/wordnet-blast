#include "info_helper.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace wnb
{

  // Class info_helper

  /// List of pointer symbols
  const char *
  info_helper::symbols[info_helper::NB_SYMBOLS] = {
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

  const std::string info_helper::sufx[] = {
    /* Noun suffixes */
    "s", "ses", "xes", "zes", "ches", "shes", "men", "ies",
    /* Verb suffixes */
    "s", "ies", "es", "es", "ed", "ed", "ing", "ing",
    /* Adjective suffixes */
    "er", "est", "er", "est"
  };

  const std::string info_helper::addr[] = {
    /* Noun endings */
    "", "s", "x", "z", "ch", "sh", "man", "y",
    /* Verb endings */
    "", "y", "e", "", "e", "", "e", "",
    /* Adjective endings */
    "", "", "e", "e"
  };

  const int info_helper::offsets[info_helper::NUMPARTS] = { 0, 0, 8, 16 };
  const int info_helper::cnts[info_helper::NUMPARTS]    = { 0, 8, 8, 4 };

  void
  info_helper::update_pos_maps()
  {
    // http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html#sect3
    //map.insert(std::pair<'s',????>); //FIXME: What shall we do here ?

    indice_offset[0] = 0; // A
    indice_offset[1] = pos_maps[A].size(); // N
    indice_offset[2] = indice_offset[1] + pos_maps[N].size(); // R
    indice_offset[3] = indice_offset[2] + pos_maps[R].size(); // V
    indice_offset[4] = indice_offset[3]; // S (???)
  }

  int info_helper::compute_indice(int offset, pos_t pos)
  {
    if (pos == S)
      pos = A;
    std::map<int,int>& map = pos_maps[pos];
    return indice_offset[pos] + map[offset];
  }

  // Function definitions

  // Return relation between synset indices and offsets
  static
  std::map<int,int>
  preprocess_data(const std::string& fn)
  {
    std::map<int,int> map;
    std::fstream file(fn.c_str());

    static const int MAX_LENGTH = 20480;
    char row[MAX_LENGTH];

    //skip header
    for(unsigned i = 0; i < 29; i++)
      file.getline(row, MAX_LENGTH);

    unsigned ind = 0;
    //parse data line
    while (file.getline(row, MAX_LENGTH))
    {
      std::stringstream srow(row);
      int offset;
      srow >> offset;
      map.insert(std::pair<int,int>(offset,ind));
      ind++;
    }

    file.close();
    return map;
  }

  info_helper
  preprocess_wordnet(const std::string& dn)
  {
    info_helper info;

    info.pos_maps[A] = preprocess_data((dn + "data.adj"));  // adj_map
    info.pos_maps[R] = preprocess_data((dn + "data.adv"));  // adv_map
    info.pos_maps[N] = preprocess_data((dn + "data.noun")); // noun_map
    info.pos_maps[V] = preprocess_data((dn + "data.verb")); // verb_map

    return info;
  }

} // end of namespace wnb

