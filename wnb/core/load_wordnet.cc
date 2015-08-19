#include "load_wordnet.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <utility>

#include <boost/graph/adjacency_list.hpp>
#include <boost/progress.hpp>
#include <boost/algorithm/string.hpp>

#include "std_ext.hh"
#include "wordnet.hh"
#include "pos_t.hh"

namespace bg = boost::graph;

namespace wnb
{

  namespace
  {

    // Load synset's words
    void load_data_row_words(std::stringstream& srow, synset& synset)
    {
      srow >> std::hex >> synset.w_cnt >> std::dec;
      for (std::size_t i = 0; i < synset.w_cnt; i++)
      {
        //word lex_id

        std::string word;
        srow >> word;
        synset.words.push_back(word);

        int lex_id;
        srow >> std::hex >> lex_id >> std::dec;
        synset.lex_ids.push_back(lex_id);
      }
    }

    // Add rel to graph
    void add_wordnet_rel(std::string& pointer_symbol_,// type of relation
                         int synset_offset,           // dest offset
                         pos_t pos,                   // p.o.s. of dest
                         int src,                     // word src
                         int trgt,                    // word target
                         synset& synset,              // source synset
                         const info_helper& info,
                         wordnet::graph& wordnet_graph)
    {
      //if (pos == S || synset.pos == S)
      //  return; //FIXME: check where are s synsets.

      std::size_t u = synset.id;
      std::size_t v = info.compute_indice(synset_offset, pos);

      ptr p;
      p.pointer_symbol = info_helper::get_symbol(pointer_symbol_);
      p.source = src;
      p.target = trgt;

      boost::add_edge(u,v, p, wordnet_graph);
    }


    // load ptrs
    void load_data_row_ptrs(std::stringstream& srow, synset& synset, const info_helper& info, wordnet::graph& wordnet_graph)
    {
      srow >> synset.p_cnt;
      for (std::size_t i = 0; i < synset.p_cnt; i++)
      {
        //http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html#sect3
        //pointer_symbol  synset_offset  pos  source/target
        std::string pointer_symbol_;
        int   synset_offset;
        pos_t pos;
        int   src;
        int   trgt;

        srow >> pointer_symbol_;
        srow >> synset_offset;

        char c;
        srow >> c;
        pos = get_pos_from_char(c);

        //print extracted edges
        //std::cout << "(" << pointer_symbol << ", " << synset_offset;
        //std::cout << ", " << pos << ")" << std::endl;

        // Extract source/target words info
        std::string src_trgt;
        srow >> src_trgt;
        std::stringstream ssrc(std::string(src_trgt,0,2));
        std::stringstream strgt(std::string(src_trgt,2,2));
        ssrc >> std::hex >> src >> std::dec;
        strgt >> std::hex >> trgt >> std::dec;

        add_wordnet_rel(pointer_symbol_, synset_offset, pos, src, trgt, synset, info, wordnet_graph);
      }
    }


    // Load a synset and add it to the wordnet class.
    void load_data_row(const std::string& row, const info_helper& info, wordnet::graph& wordnet_graph)
    {
      //http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html#sect3
      // synset_offset lex_filenum ss_type w_cnt word lex_id [word lex_id...] p_cnt [ptr...] [frames...] | gloss
      synset synset;

      std::stringstream srow(row);
      int synset_offset;
      srow >> synset_offset;
      srow >> synset.lex_filenum;
      char ss_type;
      srow >> ss_type;

      // extra information
      synset.pos = get_pos_from_char(ss_type);
      synset.id  = info.compute_indice(synset_offset, synset.pos);

      // words
      load_data_row_words(srow, synset);

      // ptrs
      load_data_row_ptrs(srow, synset, info, wordnet_graph);

      //frames (skipped)
      std::string tmp;
      std::getline(srow, tmp, '|');

      // gloss
      std::getline(srow, synset.gloss);

      // extra
      synset.sense_number = 0;

      // Add synset to graph
      wordnet_graph[synset.id] = synset;
    }

    void work_wordnet_file(const std::string& file, std::function<void(const std::string&)>& row_action)
    {
        std::ifstream fin(file.c_str());
        if (!fin.is_open())
            throw std::runtime_error("File missing: " + file);

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
        while (std::getline(fin, row) && is_header_func(row)) continue;

        //parse data line    
        do {
            row_action(row);
        } while (std::getline(fin, row));

        fin.close();
    }


    //FIXME: It seems possible to replace synset_offsets with indice here.
    void load_index_row(const std::string& row, const info_helper& info, std::vector<index>& index_list)
    {
      // lemma pos synset_cnt p_cnt [ptr_symbol...] sense_cnt tagsense_cnt synset_offset [synset_offset...]
      index index;
      std::stringstream srow(row);

      char pos;
      srow >> index.lemma;
      srow >> pos;
      index.pos = get_pos_from_char(pos); // extra data
      srow >> index.synset_cnt;
      srow >> index.p_cnt;

      std::string tmp_p;
      for (std::size_t i = 0; i < index.p_cnt; i++)
      {
        srow >> tmp_p;
        index.ptr_symbols.push_back(tmp_p);
      }
      srow >> index.sense_cnt;
      srow >> index.tagsense_cnt;

      int tmp_o;
      while (srow >> tmp_o)
      {
        index.synset_offsets.push_back(tmp_o);
        index.synset_ids.push_back(info.compute_indice(tmp_o, index.pos)); // extra data
      }

      //add synset to index list
      index_list.push_back(index);
    }

    // Parse data.<pos> files
    void load_wordnet_data(const std::string& fn, const info_helper& info, wordnet::graph& wordnet_graph)
    {
        std::function<void(const std::string&)> action = [&info, &wordnet_graph](const std::string& row) {
            load_data_row(row, info, wordnet_graph);
        };
        work_wordnet_file(fn, action);
    }

    // Parse index.<pos> files
    void load_wordnet_index(const std::string& fn, const info_helper& info, std::vector<index>& index_list)
    {
      std::function<void(const std::string&)> action = [&info, &index_list](const std::string& row) {
        load_index_row(row, info, index_list);
      };
      work_wordnet_file(fn, action);
    }

    // Parse <pos>.exc files
    void load_wordnet_exc(const std::string& fn, wordnet::exc_t& exc)
    {
      std::function<void(const std::string&)> action = [&exc](const std::string& row) {        
        std::stringstream srow(row);
        std::string key, value;
        srow >> key;
        srow >> value;

        exc[key] = value;
      };
      work_wordnet_file(fn, action);
    }

    // FIXME: this file is not in all packaged version of wordnet
    void load_wordnet_index_sense(const std::string& dn, const info_helper& info, wordnet::graph& wordnet_graph)
    {
      std::string fn = dn + "index.sense";
      std::ifstream fin(fn.c_str());
      if (!fin.is_open())
        throw std::runtime_error("File Not Found: " + fn);

      std::string row;
      std::string sense_key;
      std::size_t synset_offset;
      while (std::getline(fin, row))
      {
        std::stringstream srow(row);
        srow >> sense_key;

        // Get the pos of the lemma
        std::vector<std::string> sk = ext::split(sense_key,'%');
        std::string word = sk.at(0);
        std::stringstream tmp(ext::split(sk.at(1), ':').at(0));
        std::size_t ss_type;
        tmp >> ss_type;
        pos_t pos =  (pos_t) ss_type;

        srow >> synset_offset;

        // Update synset info
        std::size_t u = info.compute_indice(synset_offset, pos);
        std::size_t sense_number;
        srow >> sense_number;
        wordnet_graph[u].sense_number += sense_number;
        std::size_t tag_cnt;
        srow >> tag_cnt;
        if (tag_cnt != 0)
          wordnet_graph[u].tag_cnts.push_back( make_pair(word,tag_cnt) );

        //if (synset_offset == 2121620)
        //  std::cout << u << " " << word << " " << synset_offset << " "
        //            <<  wn.wordnet_graph[u].tag_cnt << " "
        //            <<  wn.wordnet_graph[u].words[0] << std::endl;
      }
    }

    // wn -over used info in cntlist even if this is deprecated
    // It is ok not to FIX and use this function
    void load_wordnet_cntlist(const std::string& dn, const info_helper& info, wordnet::graph& wordnet_graph)
    {
      std::string fn = dn + "cntlist";
      std::ifstream fin(fn.c_str());
      if (!fin.is_open())
        throw std::runtime_error("File Not Found: " + fn);

      std::string sense_key;
      int sense_number;
      int tag_cnt;

      std::string row;
      while (std::getline(fin, row))
      {
        std::stringstream srow(row);

        srow >> sense_key;
        srow >> sense_number;
        srow >> tag_cnt;

        // Get the pos of the lemma
        std::string word = ext::split(sense_key,'%').at(0);
        std::stringstream tmp(ext::split(ext::split(sense_key,'%').at(1), ':').at(0));
        std::size_t ss_type;
        tmp >> ss_type;
        pos_t pos = (pos_t) ss_type;

        // Update synset info
        std::size_t synset_offset; // FIXME
        std::size_t u = info.compute_indice(synset_offset, pos);
        wordnet_graph[u].sense_number += sense_number;
        if (tag_cnt != 0)
          wordnet_graph[u].tag_cnts.push_back( make_pair(word,tag_cnt) );
      }
    }

  } // end of anonymous namespace

  void load_wordnet(const std::string& dn, wordnet& wn)
  {
    std::cout << std::endl;
    std::cout << "### Loading Wordnet";
    boost::progress_display show_progress(13);

    std::ios_base::sync_with_stdio(false);
    for (auto &item : { "adj", "noun", "adv", "verb" }) {       
        load_wordnet_data((dn + "data." + item), wn.info(), wn._wordnet_graph);
        ++show_progress;
        load_wordnet_index((dn + "index." + item), wn.info(), wn.index_list);
        ++show_progress;
        load_wordnet_exc((dn + item + ".exc"), wn.exc[get_pos_from_name(item)]);
        ++show_progress;        
    }
    std::ios_base::sync_with_stdio(true);

    load_wordnet_index_sense(dn, wn.info(), wn._wordnet_graph);
    show_progress += 1;
   
    std::cout << std::endl;
  }

} // end of namespace wnb
