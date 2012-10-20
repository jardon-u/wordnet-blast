#include <wnb/core/wordnet.hh>
#include <wnb/std_ext.hh>

#include <string>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <vector>
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

  char *strsubst(char *str, char from, char to)
  {
    register char *p; // lol

    for (p = str; *p != 0; ++p)
      if (*p == from)
        *p = to;
    return str;
  }

  /* 'smart' search of index file.  Find word in index file, trying different
     techniques - replace hyphens with underscores, replace underscores with
     hyphens, strip hyphens and underscores, strip periods. */
  index *
  wordnet::getindex(std::string searchstr, int dbase)
  {
    char c;
    //char strings[MAX_FORMS][256];   /* vector of search strings */

    ext::to_lower(searchstr);

    std::vector<index *>    offsets(MAX_FORMS, 0);
    typedef std::vector<index> vi;
    //static index * offsets[MAX_FORMS];

    static int offset;

    /* This works like strrok(): if passed with a non-null string, prepare
       vector of search strings and offsets.  If string is null, look at
       current list of offsets and return next one, or NULL if no more
       alternatives for this word. */
    if (searchstr != "") // != NULL
    {
      std::vector<std::string> strings(MAX_FORMS, searchstr);

      offset = 0;

      //strsubst(strings[1], '_', '-');
      std::replace(strings[1].begin(), strings[1].end(), '_', '-');
      //strsubst(strings[2], '-', '_');
      std::replace(strings[2].begin(), strings[2].end(), '-', '_');

      // remove all spaces and hyphens from last search string, then all periods
      unsigned int j = 0;
      unsigned int k = 0;
      for (unsigned int i = 0; (c = searchstr[i]) != '\0'; i++)
      {
        if (c != '_' && c != '-')
          strings[3][j++] = c;
        if (c != '.')
          strings[4][k++] = c;
      }
      strings[3][j] = '\0';
      strings[4][k] = '\0';

      // Get offset of first entry.  Then eliminate duplicates and get
      // offsets of unique strings.

      if (strings[0][0] != 0)
      {
        index light_index;
        light_index.lemma = std::string(strings[0]);
        std::pair<vi::iterator,vi::iterator> bounds =
          std::equal_range(index_list.begin(), index_list.end(), light_index);
        offsets[0] = &*bounds.first;
        //offsets[0] = index_lookup(strings[0], dbase);
      }

      for (unsigned int i = 1; i < MAX_FORMS; i++)
        if ((strings[i][0]) != 0 && strings[0] != strings[i])
        {
          index light_index;
          light_index.lemma = std::string(strings[i]);

          // binary_search
          std::pair<vi::iterator,vi::iterator> bounds =
            std::equal_range(index_list.begin(), index_list.end(), light_index);
          offsets[i] = &*bounds.first;

          //offsets[i] = index_lookup(strings[i], dbase);
        }
    }

    for (unsigned i = offset; i < MAX_FORMS; i++)
      if (offsets[i])
      {
        offset = i + 1;
        return (offsets[i]);
      }

    return (NULL);
  }


  /* Set bit for each search type that is valid for the search word passed and
     return bit mask. */
  unsigned int
  wordnet::is_defined(char *searchstr, int dbase)
  {
    index * index;
    int i;
    unsigned long retval = 0;

    search_results wnresults;

    wnresults.numforms  = wnresults.printcnt = 0;
    wnresults.searchbuf = NULL;
    wnresults.searchds  = NULL;

    while ((index = getindex(searchstr, dbase)) != NULL)
    {
      searchstr = NULL;   /* clear out for next getindex() call */

      wnresults.SenseCount[wnresults.numforms] = index->sense_cnt; //off_cnt

      /* set bits that must be true for all words */

      retval |= bit(SIMPTR) | bit(FREQ) | bit(SYNS) | bit(WNGREP) | bit(OVERVIEW);

      /* go through list of pointer characters and set appropriate bits */

      for (i = 0; i < index->ptruse_cnt; i++)
      {
        if (index->ptruse[i] <= LASTTYPE) {
          retval |= bit(index->ptruse[i]);
        } else if (index->ptruse[i] == INSTANCE) {
          retval |= bit(HYPERPTR);
        } else if (index->ptruse[i] == INSTANCES) {
          retval |= bit(HYPOPTR);
        }

        if (index->ptruse[i] == SIMPTR) {
          retval |= bit(ANTPTR);
        }

#ifdef FOOP
        if (index->ptruse[i] >= CLASSIF_START &&
            index->ptruse[i] <= CLASSIF_END)
        {
          retval |= bit(CLASSIFICATION);
        }

        if (index->ptruse[i] >= CLASS_START &&
            index->ptruse[i] <= CLASS_END)
        {
          retval |= bit(CLASS);
        }
#endif

        if (index->ptruse[i] >= ISMEMBERPTR &&
            index->ptruse[i] <= ISPARTPTR)
          retval |= bit(HOLONYM);
        else if (index->ptruse[i] >= HASMEMBERPTR &&
                 index->ptruse[i] <= HASPARTPTR)
          retval |= bit(MERONYM);

      }

      if (dbase == NOUN)
      {

        /* check for inherited holonyms and meronyms */
        if (HasHoloMero(index, HMERONYM))
          retval |= bit(HMERONYM);
        if (HasHoloMero(index, HHOLONYM))
          retval |= bit(HHOLONYM);

        /* if synset has hypernyms, enable coordinate search */
        if (retval & bit(HYPERPTR))
          retval |= bit(COORDS);
      }
      else if (dbase == VERB)
      {
        /* if synset has hypernyms, enable coordinate search */
        if (retval & bit(HYPERPTR))
          retval |= bit(COORDS);

        /* enable grouping of related synsets and verb frames */
        retval |= bit(RELATIVES) | bit(FRAMES);
      }

      free_index(index);
      wnresults.numforms++;
    }
    return (retval);
  }

} // end of namespace wnb
