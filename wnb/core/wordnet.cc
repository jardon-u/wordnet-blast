#include <wnb/core/wordnet.hh>
#include <wnb/std_ext.hh>

#include <string>
#include <set>
#include <algorithm>
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

    //last pos is S and must not be given to morphword.
    static const unsigned nb_pos = POS_ARRAY_SIZE-1;
    for (unsigned p = 1; p <= nb_pos; p++)
    {
      //morphword Time consuming compare to path_similarity (x6)
      std::string mword = morphword(word, (pos_t)p);
      if (mword == "" && p != nb_pos)
        continue;
      if (mword == "")
        mword = word;
      light_index.lemma = mword;
      //std::cout << std::endl << "lemma "<< light_index.lemma << "\n";

      // binary_search
      std::pair<vi::iterator,vi::iterator> bounds =
        std::equal_range(index_list.begin(), index_list.end(), light_index);

      vi::iterator it;
      for (it = bounds.first; it != bounds.second; it++)
        for (unsigned i = 0; i < it->synset_offsets.size(); i++)
        {
          int offset = it->synset_offsets[i];
          pos_t pos = it->pos;

          //std::map<int,int>& map = info.pos_maps[pos];
          //std::cout << "(" << offset << "/" << pos << ")" << std::endl;
          //std::cout << info.indice_offset[pos] << " + "
          //          << map[offset] << std::endl;

          if (pos == (pos_t)p)
          {
            int u = info.compute_indice(offset, pos);
            //std::cout << u << std::endl;
            synsets.insert(wordnet_graph[u]);
          }
        }
    }
    //std::cout << std::endl;

    //FIXME: get rid of this or return a set.
    std::vector<synset> synsetv(synsets.size());
    copy(synsets.begin(), synsets.end(), synsetv.begin());
    return synsetv;
  }

  std::string
  wordnet::wordbase(const std::string& word, int ender)
  {
    if (ext::ends_with(word, info.sufx[ender]))
    {
      int sufxlen = info.sufx[ender].size();
      std::string strOut = word.substr(0, word.size() - sufxlen);
      if (!info.addr[ender].empty())
        strOut += info.addr[ender];
      return strOut;
    }
    return word;
  }

  bool is_defined(const std::string&, pos_t)
  {
    throw "Not Implemented";
  }

  // Try to find baseform (lemma) of individual word in POS
  std::string
  wordnet::morphword(const std::string& word, pos_t pos)
  {
    // first look for word on exception list
    exc_t::iterator it = exc[pos].find(word);
    if (it != exc[pos].end())
      return it->second; // found in exception list

    std::string tmpbuf;
    std::string end;
    int cnt = 0;

    if (pos == R)
      return ""; // Only use exception list for adverbs

    if (pos == N)
    {
      if (ext::ends_with(word, "ful"))
      {
        cnt = word.size() - 3;
        tmpbuf = word.substr(0, cnt);
        end = "ful";
      }
      else
      {
        // check for noun ending with 'ss' or short words
        if (ext::ends_with(word, "ss") || word.size() <= 2)
          return "";
      }
    }

    // If not in exception list, try applying rules from tables

    if (tmpbuf.size() == 0)
      tmpbuf = word;

    //offset = offsets[pos];
    //cnt = cnts[pos];

    int offset  = info.offsets[pos];
    int pos_cnt = info.cnts[pos];

    std::string morphed;
    for  (int i = 0; i < pos_cnt; i++)
    {
      morphed = wordbase(tmpbuf, (i + offset));
  	  if (morphed != tmpbuf) // && is_defined(morphed, pos))
        return morphed + end;
    }

    return morphed;
  }

} // end of namespace wnb
