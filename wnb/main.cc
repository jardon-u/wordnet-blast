
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/progress.hpp>

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
    std::cout << argv[0] << " .../wordnet_dir/ word" << std::endl;
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

void batch_test(wordnet& wn,
                std::vector<std::string>& word_list)
{
  for (std::size_t i = 0; i < word_list.size(); i++)
  {
    std::string& word = word_list[i];
    std::cout << "=== " << word << " === \n";

    std::vector<synset> synsets = wn.get_synsets(word);
    std::cout << synsets.size() << "\n";
    for (std::size_t j = 0; j < synsets.size(); j++)
    {
      std::cout << "  pos " << (int)synsets[j].pos << "| ";
      for (std::size_t k = 0; k < synsets[j].words.size(); k++)
        std::cout <<  synsets[j].words[k] << ", ";
    std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}

int main(int argc, char ** argv)
{
  if (usage(argc, argv))
    return 1;

  // read command line
  std::string wordnet_dir = argv[1];
  std::string word        = argv[2];

  wordnet wn(wordnet_dir);

  // read test file
  std::string list = ext::read_file("../check/list.txt");
  std::vector<std::string> wl        =  ext::split(list);
  std::vector<std::string> word_list =  ext::s_unique(wl);

  //similarity_test(wn, word, word_list);
  batch_test(wn, word_list);
}

