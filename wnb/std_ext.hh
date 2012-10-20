#ifndef _STD_EXT_HH
# define _STD_EXT_HH

# include <string>
# include <sstream>
# include <fstream>
# include <algorithm>
# include <stdexcept>

namespace ext
{

  void to_lower(std::string& s)
  {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  }

  void to_lower(char * s)
  {
    unsigned int len = strlen(s);
    std::transform(s, s+len, s, ::tolower);
  }

  /// Read a file, return the content as a C++ string
  inline
  std::string read_file(const std::string& fn)
  {
    std::ifstream is;
    is.open(fn.c_str(), std::ios::binary);
    if (!is.is_open())
      throw std::runtime_error("File not found: " + fn);

    std::string str((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());

    return str;
  }

  /// Split a std::string
  inline
  std::vector<std::string> split(const std::string& str)
  {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter< std::vector<std::string> >(tokens));
    return tokens;
  }

  inline
  bool
  ends_with(const std::string& str, const std::string& ending)
  {
    if (str.length() >= ending.length())
    {
      int cmp = str.compare(str.length() - ending.length(),
                            ending.length(), ending);
      return (0 == cmp);
    }
    return false;
  }


  /// Sorted unique
  template <typename T>
  inline
  T s_unique(T& v)
  {
    T out;

    std::sort(v.begin(), v.end());
    typename T::iterator last = std::unique(v.begin(),v.end());

    out.resize(last - v.begin());
    std::copy(v.begin(), last, out.begin());

    return out;
  }

} // end of ext

#endif /* _STD_EXT_HH */

