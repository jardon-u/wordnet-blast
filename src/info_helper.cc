#include "info_helper.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace wnb
{

  // Class info_helper

  void
  info_helper::update_pos_maps()
  {
    pos_maps_t map;
    typedef std::pair<pos_t,mapii*> paircm;

    map.insert(paircm(A,&adj_map));
    map.insert(paircm(N,&noun_map));
    map.insert(paircm(R,&adv_map));
    map.insert(paircm(V,&verb_map));

    // http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html#sect3
    //map.insert(std::pair<'s',????>); //FIXME: What shall we do here ?

    indice_offset[0] = 0; // A
    indice_offset[4] = 0; // S
    indice_offset[1] = adj_map.size(); // N
    indice_offset[2] = adj_map.size() + noun_map.size(); // R
    indice_offset[3] = adj_map.size() + noun_map.size() + adv_map.size(); // V


    pos_maps = map;
  }

  int info_helper::compute_indice(int offset, pos_t pos)
  {
    if (pos == S)
      pos = A;
    std::map<int,int>& map = *pos_maps[pos];
    return indice_offset[pos] + map[offset];
  }


  // Function definitions

  static
  std::map<int,int>
  preprocess_data(std::string fn)
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

    info.adj_map = preprocess_data((dn + "data.adj"));
    info.adv_map = preprocess_data((dn + "data.adv"));
    info.noun_map = preprocess_data((dn + "data.noun"));
    info.verb_map = preprocess_data((dn + "data.verb"));

    return info;
  }

} // end of namespace wnb

