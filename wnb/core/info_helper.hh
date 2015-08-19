#ifndef _INFO_HELPER_HH
# define _INFO_HELPER_HH

# include <string>
# include <stdexcept>
# include <map>
# include <unordered_map>

# include "wnb_export.h"
# include "pos_t.hh"

namespace wnb
{

  /// Useful information for wordnet in-memory import
  struct info_helper
  {
    /// Symbols' size
    static const std::size_t NB_SYMBOLS = 27;
    static const std::size_t NUMPARTS = POS_ARRAY_SIZE;

    /// List of pointer symbols
    static const char *      symbols[NB_SYMBOLS];
    static const std::string sufx[];
    static const std::string addr[];

    static const std::size_t  offsets[NUMPARTS];
    static const std::size_t  cnts[NUMPARTS];

    static std::size_t get_symbol(const std::string& ps)
    {
        for (std::size_t i = 0; i < NB_SYMBOLS; i++)
            if (ps == symbols[i]) {
                return i;
            }
        throw std::runtime_error("Symbol NOT FOUND.");
    }

    typedef std::unordered_map<std::size_t, std::size_t> i2of_t;  ///< indice/offset correspondences
    //typedef std::map<pos_t, i2of_t> pos_i2of_t;  ///< pos / map  correspondences

    /// Constructor
    info_helper(const std::string& dn);

    /// Compute the number of synsets (i.e. the number of vertex in the graph)
    std::size_t nb_synsets() const;

    /// Given a pos return the starting indice in the graph
    std::size_t get_indice_offset(pos_t pos) const
    {
      return indice_offset[pos];
    };

    /// Helper function computing global indice in graph from local offset
    std::size_t compute_indice(std::size_t offset, pos_t pos) const;

    /// Update a map allowing one to get the correct map given a pos
    void update_pos_maps();



  public:
    i2of_t pos_maps[POS_ARRAY_SIZE];
    //pos_i2of_t  pos_maps;
    std::size_t indice_offset[POS_ARRAY_SIZE];
  };

} // end of namespace wnb

#endif /* _INFO_HELPER_HH */

