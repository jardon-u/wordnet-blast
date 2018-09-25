
#include <string>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/null.hpp"

#include "wordnet.hh"
#include "std_ext.hh"
#include "load_wordnet.hh"

namespace wnb
{
    namespace {
        bool index_compare(const index& lhs, const index& rhs)
        {
            return (lhs.lemma.compare(rhs.lemma) < 0);
        }
    }


  //FIXME: Make (smart) use of fs::path
  wordnet::wordnet(const std::string& wordnet_dir, bool verbose)
      : _info(wordnet_dir),
        _wordnet_graph(_info.nb_synsets()),
        _verbose(verbose)
  {
    auto oldbuf = std::cout.rdbuf();
    boost::iostreams::stream_buffer< boost::iostreams::null_sink > null_buff{ boost::iostreams::null_sink() };
    if (!_verbose) {
        std::cout.rdbuf(&null_buff);
    }
    std::cout << wordnet_dir << std::endl;

    load_wordnet(wordnet_dir, *this);
    std::stable_sort(index_list.begin(), index_list.end(), index_compare);

    std::cout << "nb_synsets: " << _info.nb_synsets() << std::endl;
    if (!_verbose) {
        std::cout.rdbuf(oldbuf);
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
          std::size_t id = it->synset_ids[i];
          synsets.push_back(_wordnet_graph[id]);
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
    std::equal_range(index_list.begin(), index_list.end(), light_index, index_compare);

    return bounds;
  }

  std::string
  wordnet::wordbase(const std::string& word, std::size_t ender) const
  {
    if (ext::ends_with(word, _info.sufx[ender]))
    {
      std::size_t sufxlen = _info.sufx[ender].size();
      std::string strOut = word.substr(0, word.size() - sufxlen);
      if (!_info.addr[ender].empty())
        strOut += _info.addr[ender];
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
    exc_t::const_iterator it = exc[pos].find(word);
    if (it != exc[pos].end()) {
        return it->second; // found in exception list
	}

    std::string tmpbuf;
    std::string end;
    std::size_t cnt = 0;

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
      std::size_t offset = _info.offsets[pos];
      std::size_t pos_cnt = _info.cnts[pos];

      std::string morphed;
      for (std::size_t i = 0; i < pos_cnt; i++)
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
