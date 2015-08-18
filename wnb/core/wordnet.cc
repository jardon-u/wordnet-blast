#include <wnb/core/wordnet.hh>
#include <wnb/std_ext.hh>

#include <string>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>

# include "load_wordnet.hh"

namespace wnb
{

  //FIXME: Make (smart) use of fs::path
  wordnet::wordnet(const std::string& wordnet_dir, bool verbose)
    : _verbose(verbose)
  {
    if (_verbose)
    {
      std::cout << wordnet_dir << std::endl;
    }

    preprocess_wordnet(wordnet_dir, info);

    wordnet_graph = graph(info.nb_synsets());
    load_wordnet(wordnet_dir, *this);

    if (_verbose)
    {
      std::cout << "nb_synsets: " << info.nb_synsets() << std::endl;
    }
    //FIXME: this check is only valid for Wordnet 3.0
    //assert(info.nb_synsets() == 142335);//117659);
  }

  wordnet::wordnet(const std::string& wordnet_dir, const info_helper& info_, bool verbose)
      : _verbose(verbose),
        info(info_),
        wordnet_graph(info_.nb_synsets())
  {
      if (_verbose)
      {
          std::cout << wordnet_dir << std::endl;
      }
      load_wordnet(wordnet_dir, *this);
      if (_verbose)
      {
          std::cout << "nb_synsets: " << info.nb_synsets() << std::endl;
      }
  }

  std::vector<synset>
  wordnet::get_synsets(const std::string& word, pos_t pos) const
  {
    std::vector<synset> synsets;

    // morphing
    std::string mword = morphword(word, pos);
    if (mword == "")
      return synsets;

    // binary_search
    typedef std::vector<index> vi;
	std::pair<vi::const_iterator, vi::const_iterator> bounds = get_indexes(mword);

	vi::const_iterator it;
    for (it = bounds.first; it != bounds.second; it++)
    {
      if (pos == pos_t::UNKNOWN || it->pos == pos)
      {
        for (std::size_t i = 0; i < it->synset_ids.size(); i++)
        {
          int id = it->synset_ids[i];
          synsets.push_back(wordnet_graph[id]);
        }
      }
    }

    return synsets;
  }

  std::pair<std::vector<index>::const_iterator, std::vector<index>::const_iterator>
  wordnet::get_indexes(const std::string& word) const
  {
    index light_index;
    light_index.lemma = word;

    typedef std::vector<index> vi;
	std::pair<vi::const_iterator, vi::const_iterator> bounds =
      std::equal_range(index_list.begin(), index_list.end(), light_index);

    return bounds;
  }

  std::string
  wordnet::wordbase(const std::string& word, int ender) const
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

  bool is_defined(const std::string& word, pos_t pos)
  {
    // hack FIXME: Some verbs are built with -e suffix ('builde' is just an example).
    if (pos == V && word == "builde")
      return false;
    return true;
  }

  // Try to find baseform (lemma) of individual word in POS
  std::string
  wordnet::morphword(const std::string& word, pos_t pos) const
  {
    // first look for word on exception list
	std::map<pos_t, exc_t>::const_iterator it_pos = exc.find(pos);
	if (it_pos != exc.end()) {
		exc_t::const_iterator it = it_pos->second.find(word);
		if (it != it_pos->second.end())
			return it->second; // found in exception list
	}

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

    if (pos != pos_t::UNKNOWN) 
    {
      int offset  = info.offsets[pos];
      int pos_cnt = info.cnts[pos];

      std::string morphed;
      for  (int i = 0; i < pos_cnt; i++)
      {
        morphed = wordbase(tmpbuf, (i + offset));
        if (morphed != tmpbuf && is_defined(morphed, pos))
           return morphed + end;
      }
      return morphed;
    }
    return word;
  }

} // end of namespace wnb
