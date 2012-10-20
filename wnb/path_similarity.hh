#ifndef _PATH_SIMILARITY_HH
# define _PATH_SIMILARITY_HH

# include "wordnet.hh"
# include "bfs.hh"

namespace wncpp
{

  namespace internal
  {
    template <typename G>
    void build_graphp(const wordnet& wn, G& g)
    {
      int V = num_vertices(wn.wordnet_graph);
      int E = num_edges(wn.wordnet_graph);
      g = G(2*(E + V));

      // half node for hypernyme relation only , half for hypo only.
      for (int i = 0; i < 2*V; i++)
        g[i] = wn.wordnet_graph[i%V];

      for (int i = 0; i < V; i++)
      {
        typename boost::graph_traits<G>::out_edge_iterator e, begin, end;
        tie(begin,end) = out_edges(i, wn.wordnet_graph);
        for(e = begin; e != end; e++)
        {
          typename G::vertex_descriptor u = source(*e, wn.wordnet_graph);
          typename G::vertex_descriptor v = target(*e, wn.wordnet_graph);
          ptr p = wn.wordnet_graph[*e];

          //hypernyme rel
          if (wn.wordnet_graph[*e].pointer_symbol == 1 ||
              wn.wordnet_graph[*e].pointer_symbol == 2)
          {
            boost::add_edge(u,v, p, g);
          }

          //hyponyme rel
          if (wn.wordnet_graph[*e].pointer_symbol == 3 ||
              wn.wordnet_graph[*e].pointer_symbol == 4)
          {
            typename G::vertex_descriptor u = source(*e, wn.wordnet_graph);
            typename G::vertex_descriptor v = target(*e, wn.wordnet_graph);

            //hyper graph to hypo graph
            boost::add_edge(u,V+v, p, g);
            //hypo graph only
            boost::add_edge(V+u,V+v, p, g);
          }
        }
      }
    }

  } // end of name space internal


  struct similarity
  {

    typedef wordnet::graph G;

    similarity(const wordnet& wn)
    {
      internal::build_graphp(wn,g);
    }

    inline
    float operator()(const synset& s1, const synset& s2, int max_length)
    {
      if (s1 == s2)
        return 1;
      if (s1.pos != s2.pos)
        return -2;

      // get vertex associated to s1 (with type wordnet::graph, u == src)
      typedef boost::graph_traits<G>::vertex_descriptor vertex;
      int u = s1.id;
      vertex src = boost::vertex(u, g);

      // distance map for bfs
      std::vector<int> distance(num_vertices(g));
      distance[src] = 1;

      // bfs_algorithm : throw dist if current_vertex == s2.
      try {
        boost::breadth_first_search(g, src,
                                    visitor(bfs::record_distance(&distance[0],s2,
                                                                 max_length)));
      }
      catch (int dist) {
        return 1. / dist;
        std::cout << "throw result" << std::endl;
      }

      return -1;
    }

    wordnet::graph g;
  };

} // end of namespace wncpp

#endif /* _PATH_SIMILARITY_HH */

