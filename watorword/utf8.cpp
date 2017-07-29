#include <bitset>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define DUMP_VAR(x)                                                            \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"   \
            << std::endl;
#define HEX_VAR(x)                                                             \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<"               \
            << boost::format("0x%02x") % x << ">" << std::endl;

#define TRACE_VAR(x)

vector<string> parseUTF8(const string &words) {
#if 0
  for(unsigned int ch : words)
  {
    std::cout << boost::format("0x%02x,")%(int)ch;
  }
   std::cout << std::endl;
#endif
  vector<string> ret;
  int i = 0;
  while (i < words.size()) {
    unsigned char ch = words.at(i);
    TRACE_VAR(i);
    TRACE_VAR(bitset<8>(ch));
    TRACE_VAR(bitset<8>(ch & 0b11111100));
    if ((ch & 0b11111100) == 0b11111100) {
      ret.push_back(words.substr(i, 6));
      TRACE_VAR(words.substr(i, 6));
      i += 6;
    } else if ((ch & 0b11111000) == 0b11111000) {
      ret.push_back(words.substr(i, 5));
      TRACE_VAR(words.substr(i, 5));
      i += 5;
    } else if ((ch & 0b11110000) == 0b11110000) {
      ret.push_back(words.substr(i, 4));
      TRACE_VAR(words.substr(i, 4));
      i += 4;
    } else if ((ch & 0b11100000) == 0b11100000) {
      ret.push_back(words.substr(i, 3));
      TRACE_VAR(words.substr(i, 3));
      i += 3;
    } else if ((ch & 0b11000000) == 0b11000000) {
      ret.push_back(words.substr(i, 2));
      TRACE_VAR(words.substr(i, 2));
      i += 2;
    } else {
      ret.push_back(words.substr(i, 1));
      TRACE_VAR(words.substr(i, 1));
      i++;
    }
    // TRACE_VAR(words.size());
  }
  return ret;
}

void parseUTF8(const string &article, vector<string> &words,
               vector<int> &positions) {
#if 0
  for(unsigned int ch : article)
  {
    std::cout << boost::format("0x%02x,")%(int)ch;
  }
   std::cout << std::endl;
#endif
  int i = 0;
  while (i < article.size()) {
    unsigned char ch = article.at(i);
    TRACE_VAR(i);
    TRACE_VAR(bitset<8>(ch));
    TRACE_VAR(bitset<8>(ch & 0b11111100));
    if ((ch & 0b11111100) == 0b11111100) {
      words.push_back(article.substr(i, 6));
      positions.push_back(i);
      TRACE_VAR(article.substr(i, 6));
      i += 6;
    } else if ((ch & 0b11111000) == 0b11111000) {
      words.push_back(article.substr(i, 5));
      positions.push_back(i);
      TRACE_VAR(article.substr(i, 5));
      i += 5;
    } else if ((ch & 0b11110000) == 0b11110000) {
      words.push_back(article.substr(i, 4));
      positions.push_back(i);
      TRACE_VAR(article.substr(i, 4));
      i += 4;
    } else if ((ch & 0b11100000) == 0b11100000) {
      words.push_back(article.substr(i, 3));
      positions.push_back(i);
      TRACE_VAR(article.substr(i, 3));
      i += 3;
    } else if ((ch & 0b11000000) == 0b11000000) {
      words.push_back(article.substr(i, 2));
      positions.push_back(i);
      TRACE_VAR(article.substr(i, 2));
      i += 2;
    } else {
      words.push_back(article.substr(i, 1));
      positions.push_back(i);
      TRACE_VAR(article.substr(i, 1));
      i++;
    }
    // TRACE_VAR(article.size());
  }
}

std::deque<int> unicode_to_utf8(int charcode) {
  std::deque<int> d;
  if (charcode < 128) {
    d.push_back(charcode);
  } else {
    int first_bits = 6;
    const int other_bits = 6;
    int first_val = 0xC0;
    int t = 0;
    while (charcode >= (1 << first_bits)) {
      {
        t = 128 | (charcode & ((1 << other_bits) - 1));
        charcode >>= other_bits;
        first_val |= 1 << (first_bits);
        first_bits--;
      }
      d.push_front(t);
    }
    t = first_val | charcode;
    d.push_front(t);
  }
  return d;
}

char32_t utf8_to_unicode(std::deque<int> &coded) {
  char32_t charcode = 0;
  int t = coded.front();
  coded.pop_front();
  if (t < 128) {
    return t;
  }
  int high_bit_mask = (1 << 6) - 1;
  int high_bit_shift = 0;
  int total_bits = 0;
  const int other_bits = 6;
  while ((t & 0xC0) == 0xC0) {
    t <<= 1;
    t &= 0xff;
    total_bits += 6;
    high_bit_mask >>= 1;
    high_bit_shift++;
    charcode <<= other_bits;
    charcode |= coded.front() & ((1 << other_bits) - 1);
    coded.pop_front();
  }
  charcode |= ((t >> high_bit_shift) & high_bit_mask) << total_bits;
  return charcode;
}

vector<char32_t> utf8toCode(const string &words) {
  vector<char32_t> ret;
  auto wordStr = parseUTF8(words);

  for (auto w : wordStr) {
    std::deque<int> inputW;
    for (unsigned int ch : w) {
      inputW.push_back((int)ch);
    }
    auto code = utf8_to_unicode(inputW);
    ret.push_back(code);
#if 0
    for(unsigned int ch : w)
    {
      std::cout << (int)ch;
    }
     std::cout << std::endl;
#endif
  }
  return ret;
}
