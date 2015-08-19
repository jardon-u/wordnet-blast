#include "info_helper.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <cassert>

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

  const std::size_t info_helper::offsets[info_helper::NUMPARTS] = { 0, 0, 8, 16, 0, 0 };
  const std::size_t info_helper::cnts[info_helper::NUMPARTS] = { 0, 8, 8, 4, 0, 0 };

  std::size_t
  info_helper::nb_synsets() const
  {
      std::size_t sum = 0;
      for (auto &m : pos_maps) {
          sum += m.second.size();
      }
      return sum;
      //return adj_map.size() + adv_map.size() + noun_map.size() + verb_map.size();
  };

  void
  info_helper::update_pos_maps()
  {
    // http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html#sect3

    indice_offset[UNKNOWN] = 0;

    indice_offset[N] = 0;
    indice_offset[V] = indice_offset[N] + pos_maps[N].size();
    indice_offset[A] = indice_offset[V] + pos_maps[V].size();
    indice_offset[R] = indice_offset[A] + pos_maps[A].size();
    indice_offset[S] = indice_offset[R] + pos_maps[R].size();

  }

  std::size_t info_helper::compute_indice(std::size_t offset, pos_t pos) const
  {
    if (pos == S) { pos = A; }
    auto map = pos_maps.at(pos);
    return indice_offset[pos] + map.at(offset);
  }

  // Function definitions

  // Return relation between synset indices and offsets
  std::map<std::size_t, std::size_t>
  preprocess_data(const std::string& fn)
  {
    std::map<std::size_t, std::size_t> map;
    std::ifstream file(fn.c_str());
    if (!file.is_open())
      throw std::runtime_error("preprocess_data: File not found: " + fn);

    std::string row;

    //skip header
    std::size_t lineno = 1;
    auto is_header_func = [&lineno](std::string& row){
        // "Each data file begins with several lines containing a copyright notice,
        //  version number and license agreement. These lines all begin with two 
        //  spaces and the line number"
        //                  http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html
        return std::atoi(row.substr(0, 3 + (lineno > 9 ? 1 : 0)).c_str()) == lineno++;
    };

    while (std::getline(file, row) && is_header_func(row)) continue;

    //parse data line    
    int ind = 0;
    do {
        std::stringstream srow(row);
        int offset;
        srow >> offset;
        map.insert(std::pair<std::size_t, std::size_t>(offset, ind));
        ind++;
    } while (std::getline(file, row));

    file.close();
    return map;
  }

  info_helper
  preprocess_wordnet(const std::string& dn)
  {
    info_helper info;
    preprocess_wordnet(dn, info);
    return info;
  }

  void
  preprocess_wordnet(const std::string& dn, info_helper& info)
  {
      info.pos_maps[N] = preprocess_data((dn + "data.noun")); // noun_map
      info.pos_maps[V] = preprocess_data((dn + "data.verb")); // verb_map
      info.pos_maps[A] = preprocess_data((dn + "data.adj"));  // adj_map
      info.pos_maps[R] = preprocess_data((dn + "data.adv"));  // adv_map

      info.update_pos_maps();
  }

} // end of namespace wnb

