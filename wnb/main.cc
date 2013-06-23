
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/progress.hpp>
#include <boost/algorithm/string.hpp>

#include <wnb/core/wordnet.hh>
#include <wnb/core/load_wordnet.hh>
#include <wnb/core/info_helper.hh>
#include <wnb/nltk_similarity.hh>
#include <wnb/std_ext.hh>

using namespace wnb;

bool usage(int argc, char ** argv)
{
  std::string dir;
  if (argc >= 2)
    dir = std::string(argv[1]);
  if (argc != 3 || dir[dir.length()-1] != '/')
  {
    std::cout << argv[0] << " .../wordnet_dir/ word_list_file" << std::endl;
    return true;
  }
  return false;
}

struct ws
{
  std::string w;
  float       s;

  bool operator<(const ws& a) const {return s > a.s;}
};


/// Compute similarity of word with words in word list
std::vector<ws>
compute_similarities(wordnet& wn,
                     const std::string& word,
                     const std::vector<std::string>& word_list)
{
  std::vector<ws> wslist;
  std::vector<synset> synsets1 = wn.get_synsets(word);

  for (unsigned i = 0; i < synsets1.size(); i++)
    for (unsigned k = 0; k < synsets1[i].words.size(); k++)
      std::cout << " - " << synsets1[i].words[k] << std::endl;

  nltk_similarity path_similarity(wn);
  {
    boost::progress_timer t;
    boost::progress_display show_progress(word_list.size());

    for (unsigned k = 0; k < word_list.size(); k++)
    {
      const std::string& w = word_list[k];
      float max = 0;
      std::vector<synset> synsets2 = wn.get_synsets(w);
      for (unsigned i = 0; i < synsets1.size(); i++)
      {
        for (unsigned j = 0; j < synsets2.size(); j++)
        {
          float s = path_similarity(synsets1[i], synsets2[j], 6);
          if (s > max)
            max = s;
        }
      }
      ws e = {w, max};
      wslist.push_back(e);
      ++show_progress;
    }
  }

  return wslist;
}

void similarity_test(wordnet&                  wn,
                     const std::string&        word,
                     std::vector<std::string>& word_list)
{
  std::vector<ws> wslist = compute_similarities(wn, word, word_list);

  std::stable_sort(wslist.begin(), wslist.end());
  for (unsigned i = 0; i < std::min(wslist.size(), size_t(10)); i++)
    std::cout << wslist[i].w << " " << wslist[i].s << std::endl;
}


void wn_like(wordnet& wn, std::string& word)
{
  static const unsigned nb_pos = POS_ARRAY_SIZE-1;
  for (unsigned p = 1; p <= nb_pos; p++)
  {
    std::vector<synset> synsets = wn.get_synsets(word, (pos_t)p);
    if (synsets.size() == 0)
      continue;

    std::string mword = wn.morphword(word, (pos_t)p);
    std::cerr << word << " " << mword << std::endl;

    std::cout << "\nOverview of "
              << get_name_from_pos((pos_t)p)
              << " " << mword << "\n\n";

    std::cout << "The "
              << get_name_from_pos((pos_t)p)
              << " " << mword << " has "
              << synsets.size()
              << " senses";

    int tagsense_cnt = 0; // FIXME: already in index_list
    for (std::size_t j = 0; j < synsets.size(); j++)
      tagsense_cnt += (synsets[j].tag_cnt != 0);

    if (tagsense_cnt != 0)
      std::cout << " (first " << tagsense_cnt << " from tagged texts)";

    std::cout << "\n";
    std::cout << "                                      \n";

    for (std::size_t j = 0; j < synsets.size(); j++)
    {
      std::cout << j+1 << ". "; // << (int)synsets[j].pos << "| ";
      if (synsets[j].tag_cnt != 0)
        std::cout << "(" << synsets[j].tag_cnt << ") ";
      std::cout << synsets[j].words[0];
      for (std::size_t k = 1; k < synsets[j].words.size(); k++)
        std::cout << ", " << synsets[j].words[k];
      boost::algorithm::trim(synsets[j].gloss);
      std::cout << " -- (" << synsets[j].gloss << ")";
      std::cout << std::endl;
    }
  }
}

void batch_test(wordnet& wn, std::vector<std::string>& word_list)
{
  for (std::size_t i = 0; i < word_list.size(); i++)
  {
    wn_like(wn, word_list[i]);
  }
}

int main(int argc, char ** argv)
{
  if (usage(argc, argv))
    return 1;

  // read command line
  std::string wordnet_dir = argv[1];
  std::string test_file   = argv[2];

  wordnet wn(wordnet_dir);

  // read test file
  std::string list = ext::read_file(test_file);
  std::vector<std::string> wl        =  ext::split(list);

  batch_test(wn, wl);
}

