#include "wordnet.hh"
#include "wn.h"

#include <string>
#include <set>
#include <stdexcept>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>

namespace wnb
{

  //FIXME: Make (smart) use of fs::path
  wordnet::wordnet(const std::string& wordnet_dir)
  {
    wordnet& wn = *this;
    //std::string wordnet_dir = wordnet_path.file_string();
    //std::cout <<  wordnet_dir << std::endl;

    info = preprocess_wordnet(wordnet_dir);
    info.update_pos_maps();

    wordnet_graph = graph(info.nb_synsets());
    load_wordnet(wordnet_dir, wn, info);

    //FIXME: this check is only valid for Wordnet 3.0
    assert(info.nb_synsets() == 117659);

    // FIXME: Provide our path (via env var) ?
    // FIXME: rewrite this. not thread safe and slow
    //if(wninit())
    //  throw std::runtime_error("C wordnet lib initialization failed");
    //if (morphinit())
    //  throw std::runtime_error("C wordnet lib initialization failed");
  }


  std::vector<synset>
  wordnet::get_synsets(const std::string& word)
  {
    typedef std::vector<index> vi;
    std::set<synset> synsets;

    index light_index;
    light_index.lemma = word;

    // const_cast for compability reason with libwordnet (morphword)
    // (safe if cword not modified) no need to free memory
    char * cword = const_cast<char*>(word.c_str());

    //last pos is S and must not be given to morphword.
    static const unsigned nb_pos = POS_ARRAY_SIZE-1; 
    for (unsigned p = 1; p <= nb_pos; p++)
    {
      //morphword Time consuming compare to path_similarity (x6)
      const char * mword = cword;//morphword(cword,p);
      if (mword == NULL && p != nb_pos)
        continue;
      if (mword == NULL)
        mword = word.c_str();
      light_index.lemma = std::string(mword);
      //std::cout <<       light_index.lemma << " ";

      // binary_search
      std::pair<vi::iterator,vi::iterator> bounds =
        std::equal_range(index_list.begin(), index_list.end(),
                         light_index);

      vi::iterator it;
      for (it = bounds.first; it != bounds.second; it++)
        for (unsigned i = 0; i < it->synset_offsets.size(); i++)
        {
          int u = info.compute_indice(it->synset_offsets[i], it->pos);
          synsets.insert(wordnet_graph[u]);
        }
    }
    //std::cout << std::endl;

    //FIXME: get rid of this or return a set.
    std::vector<synset> synsetv(synsets.size());
    copy(synsets.begin(), synsets.end(), synsetv.begin());
    return synsetv;
  }

  std::vector<std::string> morphword(std::string word, int pos)
  {
  }

} // end of namespace wnb
