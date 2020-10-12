/*************************************************************************
> File Name:  darts.h
> Author: shenming
> Created Time: Sat Oct 10 17:28:46 2020

   DARTS: Double-ARray Trie System。
   Copy自CRF++中darts实现。

   通过build插入的key数组，必须满足字典序要求，否则build失败

   fetch 获取当前节点下一层要插入的节点内容，
   insert 执行插入，递归调用fetch，执行插入
************************************************************************/

#pragma once
#include <cstring>
#include <vector>
#include <cstdio>
#include <string>
#include <algorithm>
#include <fstream>
#include "Logger/log_spd.h"

namespace DARTS {

template <class T>
inline T _max(T x, T y) { return(x > y) ? x : y; }

template <class T>
inline T* _resize(T* ptr, size_t n, size_t l, T v)
{
  T *tmp = new T[l];
  for (size_t i = 0; i < n; ++i) tmp[i] = ptr[i];
  for (size_t i = n; i < l; ++i) tmp[i] = v;
  delete [] ptr;
  return tmp;
}

template <class T>
class Length
{
public:
  size_t operator()(const T *key) const
  {size_t i; for (i = 0; key[i] != (T)0; ++i) {} return i;}
};

template <> class Length<char> {
public:
  size_t operator()(const char* key) const
  {return std::strlen(key);}
};

template <class node_type_, class node_u_type_,
          class array_type_, class array_u_type_,
          class length_func_ = Length<node_type_>>
class DoubleArrayImpl
{
private:
  struct node_t {
    array_u_type_ code;
    size_t  depth;
    size_t  left;
    size_t  right;
  };

  struct unit_t {
    array_type_  base;
    array_u_type_ check;
  };

  unit_t         *array_;     // array_ 中为 base check 双数组
  unsigned char  *used_;       // 辅助数组
  size_t        alloc_size_;   // array_ used_ 分配长度
  size_t        size_;
  int           error_;

  node_type_    **key_;         //  node_type_ *key     字典数中插入的实际对象
  size_t        key_size_;      //  key 数组长度
  size_t        *length_;       //  记录每个对应 node_type_ *key 的长度
  array_type_   *value_;        //  每个 key  可以选择附带一个value
  size_t        progress_;
  size_t        next_check_pos_;

  int (*progress_func_)(size_t, size_t);

  size_t resize(const size_t new_size) {
    unit_t tmp;
    tmp.base = 0;
    tmp.check = 0;
    array_ = _resize(array_, alloc_size_, new_size, tmp);
    used_  = _resize(used_, alloc_size_, new_size,
                     static_cast<unsigned char>(0));
    alloc_size_ = new_size;
    return new_size;
  }

  size_t fetch(const node_t &parent, std::vector<node_t> &siblings) {
    if (error_ < 0) return 0;

    array_u_type_ prev = 0;

    for (size_t i = parent.left; i < parent.right; ++i) {
      if ((length_ ? length_[i] : length_func_()(key_[i])) < parent.depth)
        continue;
      const node_u_type_ *tmp = reinterpret_cast<node_u_type_ *>(key_[i]);
      array_u_type_ cur = 0;
      if ((length_ ? length_[i] : length_func_()(key_[i])) != parent.depth)
        cur = (array_u_type_)tmp[parent.depth] + 1;

      if (prev > cur) {    // prev 可以等于 cur
        error_ = -3;
        return 0;
      }

      // 当 length == parent.depth时，cur == 0，此节点也作为一个siblings。
      if (cur != prev || siblings.empty()) {
        node_t tmp_node;
        tmp_node.depth = parent.depth + 1;
        tmp_node.code  = cur;      // 为实际的code+1
        tmp_node.left  = i;
        if (!siblings.empty()) siblings[siblings.size()-1].right = i;

        siblings.push_back(tmp_node);
      }

      prev = cur;

    }

    if (!siblings.empty())
      siblings[siblings.size()-1].right = parent.right;

    return siblings.size();
  }

  // 深度优先遍历，进行插入。
    size_t insert(const std::vector <node_t> &siblings) {
        if (error_ < 0) return 0;

        size_t begin = 0;
        size_t pos   = _max((size_t)siblings[0].code + 1, next_check_pos_) - 1;
        size_t nonzero_num = 0;
        int    first = 0;

        if (alloc_size_ <= pos) resize(pos + 1);

        while (true) {
        next:
            ++pos;

            if (alloc_size_ <= pos) resize(pos + 1);

            if (array_[pos].check) {
                ++nonzero_num;
                continue;
            } else if (!first) {
                next_check_pos_ = pos;
                first = 1;
            }

            begin = pos - siblings[0].code;
            if (alloc_size_ <= (begin + siblings[siblings.size()-1].code)) {
                // progress_ 大多数情况下为0，alloc_size_初始是8192，正常字符code不会进入，如果进入，且progress_未更新，的确会core
                // progress_ 在每次完整插入一个key时就会更新。
                // 应该不可能出现core情况
                resize(static_cast<size_t>(alloc_size_ *
                                           _max(1.05, 1.0 * key_size_ / progress_)));
            }

            if (used_[begin]) continue;

            // 直至找到一块能够放下所有siblings的pos
            for (size_t i = 1; i < siblings.size(); ++i)
                if (array_[begin + siblings[i].code].check != 0) goto next;

            break;
        }

        // -- Simple heuristics --
        // if the percentage of non-empty contents in check between the index
        // 'next_check_pos' and 'check' is greater than some constant
        // value(e.g. 0.9),
        // new 'next_check_pos' index is written by 'check'.
        if (1.0 * nonzero_num/(pos - next_check_pos_ + 1) >= 0.95)
            next_check_pos_ = pos;

        used_[begin] = 1;    // used_ only used in this func, mark if begin is used;
        size_ = _max(size_,
                     begin +
                     static_cast<size_t>(siblings[siblings.size() - 1].code + 1));

        for (size_t i = 0; i < siblings.size(); ++i)
            array_[begin + siblings[i].code].check = begin;

        for (size_t i = 0; i < siblings.size(); ++i) {
            std::vector <node_t> new_siblings;

            // 只有code为1，即到达结尾标记，才会进入此判断
            if (!fetch(siblings[i], new_siblings)) {
                // 如果设置了value，以其负值作为value，防止value为0，再减去1
                array_[begin + siblings[i].code].base =
                value_ ?
                static_cast<array_type_>(-value_[siblings[i].left]-1) :
                static_cast<array_type_>(-siblings[i].left-1);

                if (value_ && (array_type_)(-value_[siblings[i].left]-1) >= 0) {
                    error_ = -2;
                    return 0;
                }

                ++progress_;
                if (progress_func_)(*progress_func_)(progress_, key_size_);

            } else {
                size_t h = insert(new_siblings);
                array_[begin + siblings[i].code].base = h;   // parent node
            }
        }

        return begin;
    }

public:
  typedef array_type_  value_type;
  typedef node_type_   key_type;
  typedef array_type_  result_type;  // for compatibility

  struct result_pair_type {
    value_type value;
    size_t     length;
  };

  explicit DoubleArrayImpl(): array_(0), used_(0),
                              size_(0), alloc_size_(0),
                              no_delete_(0), error_(0) {}
  ~DoubleArrayImpl() { clear(); }

  void set_result(value_type& x, value_type r, size_t) const {
    x = r;
  }

  void set_result(result_pair_type& x, value_type r, size_t l) const {
    x.value = r;
    x.length = l;
  }

  void set_array(void *ptr, size_t size = 0) {
    clear();
    array_ = reinterpret_cast<unit_t *>(ptr);
    no_delete_ = true;
    size_ = size;
  }

  const void *array() const {
    return const_cast<const void *>(reinterpret_cast<void *>(array_));
  }

  void clear() {
    if (!no_delete_)
      delete [] array_;
    delete [] used_;
    array_ = 0;
    used_ = 0;
    alloc_size_ = 0;
    size_ = 0;
    no_delete_ = false;
  }

  size_t unit_size()  const { return sizeof(unit_t); }
  size_t size()       const { return size_; }
  size_t total_size() const { return size_ * sizeof(unit_t); }

  size_t nonzero_size() const {
    size_t result = 0;
    for (size_t i = 0; i < size_; ++i)
      if (array_[i].check) ++result;
    return result;
  }

  // key 数组的长度 应该等于 value数组长度。  value可为0;
  // length 是每个 *key 的长度，如果为 0, 则通过 strlen 计算
  int build(size_t     key_size,
            key_type   **key,
            value_type *value = 0,    // value的值需要大于等于0，否则match会出问题
            size_t     *length = 0,
            int (*progress_func)(size_t, size_t) = 0) {
    if (!key_size || !key) return 0;

    progress_func_ = progress_func;
    key_           = key;
    length_        = length;
    key_size_      = key_size;
    value_         = value;
    progress_      = 0;

    resize(8192);

    array_[0].base = 1;
    next_check_pos_ = 0;

    node_t root_node;
    root_node.left  = 0;
    root_node.right = key_size;
    root_node.depth = 0;

    std::vector <node_t> siblings;

    fetch(root_node, siblings);
    insert(siblings);

    size_ += (1 << 8 * sizeof(key_type)) + 1;    // TODO check
    if (size_ >= alloc_size_) resize(size_);

    delete [] used_;
    used_ = 0;

    return error_;
  }

  int open(const char *file,
           const char *mode = "rb",
           size_t size = 0) {
    std::FILE *fp = std::fopen(file, mode);
    if (!fp) return -1;
    if (std::fseek(fp, 0, SEEK_SET) != 0) return -1;

    if (!size) {
      if (std::fseek(fp, 0L,     SEEK_END) != 0) return -1;     // 根据文件大小得到 size
      size = std::ftell(fp);
      if (std::fseek(fp, 0, SEEK_SET) != 0) return -1;
    }

    clear();

    size_ = size;
    size_ /= sizeof(unit_t);
    array_ = new unit_t[size_];
    if (size_ != std::fread(reinterpret_cast<unit_t *>(array_),
                            sizeof(unit_t), size_, fp)) return -1;
    std::fclose(fp);

    return 0;
  }

  int save(const char *file,
           const char *mode = "wb") {
    if (!size_) return -1;
    std::FILE *fp = std::fopen(file, mode);
    if (!fp) return -1;
    if (size_ != std::fwrite(reinterpret_cast<unit_t *>(array_),
                             sizeof(unit_t), size_, fp))
      return -1;
    std::fclose(fp);
    return 0;
  }

  template <class T>
  // 严格匹配 key，得到对应的value， len可以指定匹配key的长度，node_pos指定开始匹配位置
  inline void exactMatchSearch(const key_type *key,
                               T & result,
                               size_t len = 0,
                               size_t node_pos = 0) const {
    result = exactMatchSearch<T>(key, len, node_pos);
    return;
  }

  template <class T>
  inline T exactMatchSearch(const key_type *key,
                            size_t len = 0,
                            size_t node_pos = 0) const {
    if (!len) len = length_func_()(key);

    T result;
    set_result(result, -1, 0);

    array_type_  b = array_[node_pos].base;
    array_u_type_ p;

    for ( size_t i = 0; i < len; ++i) {
      p = b +(node_u_type_)(key[i]) + 1;       // build 时，所有code默认+1了
      if (static_cast<array_u_type_>(b) == array_[p].check)
        b = array_[p].base;                    // 跳转到下一层节点
      else
        return result;                        // check 失败
    }

    p = b;
    array_type_ n = array_[p].base;
    if (static_cast<array_u_type_>(b) == array_[p].check && n < 0)
      set_result(result, -n-1, len);      // len 是有效key的长度  -n-1 还原实际的value

    return result;
  }

  template <class T>
  // 搜索最长匹配Prefix, 对比 exactMatchSearch, 会记录中间匹配上的内容
  T prefixSearch(const key_type *key,
                 size_t node_pos = 0) const {
    T result;
    int len = length_func_()(key);
    set_result(result, -1, 0);

    array_type_ b = array_[node_pos].base;
    array_u_type_ p;
    array_type_  n;
    size_t num = 0;

    for ( size_t i = 0; i < len; ++i) {
      p = b; // + 0
      n = array_[p].base;
      if ((array_u_type_) b == array_[p].check && n < 0) {
        set_result(result, -n-1, i);
        ++num;
      }

      p = b +(node_u_type_)(key[i]) + 1;
      if ((array_u_type_) b == array_[p].check)
        b = array_[p].base;
      else
        return result;
    }

    p = b;
    n = array_[p].base;
    if ((array_u_type_) b == array_[p].check && n < 0) {
      set_result(result, -n-1, len);
    }
    return result;
  }


  // 搜索最长Contain长度，不追求非得match到有效的词
  template <class T>
  T containLen(const key_type *key, bool& is_word,
               size_t node_pos = 0) const {
    T result;
    int len = length_func_()(key);
    set_result(result, 0, 0);

    array_type_ b = array_[node_pos].base;
    array_u_type_ p;
    array_type_  n;
    size_t num = 0;

    for ( size_t i = 0; i < len; ++i) {
      p = b; // + 0
      n = array_[p].base;
      if ((array_u_type_) b == array_[p].check && n < 0) {
        set_result(result, i, i);     // 这个操作其实多余
        is_word = true;
        ++num;
      }

      p = b +(node_u_type_)(key[i]) + 1;
      if ((array_u_type_) b == array_[p].check) {
        b = array_[p].base;
        set_result(result, i+1, i+1);
        is_word = false;
      }
      else
        return result;
    }

    p = b;
    n = array_[p].base;
    if ((array_u_type_) b == array_[p].check && n < 0) {
      set_result(result, len, len);
      is_word = true;
    }
    return result;
  }

  // 搜索darts中key所有的前缀
  template <class T>
  size_t commonPrefixSearch(const key_type *key,
                            T* result,
                            size_t result_len,
                            size_t len = 0,
                            size_t node_pos = 0) const {
    if (!len) len = length_func_()(key);

    array_type_  b   = array_[node_pos].base;
    size_t     num = 0;
    array_type_  n;
    array_u_type_ p;

    for ( size_t i = 0; i < len; ++i) {
      p = b;  // + 0;
      n = array_[p].base;
      if ((array_u_type_) b == array_[p].check && n < 0) {
        // result[num] = -n-1;
        // 此处判断的 b == array_[p].check means code = 0，即为结尾标记
        if (num < result_len) set_result(result[num], -n-1, i);
        ++num;
      }

      p = b +(node_u_type_)(key[i]) + 1;
      if ((array_u_type_) b == array_[p].check)
        b = array_[p].base;
      else
        return num;
    }

    p = b;
    n = array_[p].base;

    if ((array_u_type_)b == array_[p].check && n < 0) {
      if (num < result_len) set_result(result[num], -n-1, len);
      ++num;
    }

    return num;
  }

  // 从指定位置 key_pos 开始匹配key
  value_type traverse(const key_type *key,
                      size_t &node_pos,
                      size_t &key_pos,
                      size_t len = 0) const {
    if (!len) len = length_func_()(key);

    array_type_  b = array_[node_pos].base;
    array_u_type_ p;

    for (; key_pos < len; ++key_pos) {
      p = b +(node_u_type_)(key[key_pos]) + 1;
      if (static_cast<array_u_type_>(b) == array_[p].check) {
        node_pos = p;
        b = array_[p].base;
      } else {
        return -2;  // no node
      }
    }

    p = b;
    array_type_ n = array_[p].base;
    if (static_cast<array_u_type_>(b) == array_[p].check && n < 0)
      return -n-1;

    return -1;  // found, but no value
  }

  int save_dart(std::ofstream& ofs) const {
    if (! ofs.is_open()) {
      return 1;
    }

    ofs.write((char*)&alloc_size_, sizeof(size_t));
    for (size_t i = 0; i < alloc_size_; i++) {
      ofs.write((char*)&(array_[i]), sizeof(unit_t));
    }
    return 0;
  }

  int load_dart(std::ifstream& ifs) {
    if (! ifs.is_open()) {
      return 1;
    }
    size_t new_alloc_size = 0;
    ifs.read((char*)&new_alloc_size, sizeof(size_t));
    resize(new_alloc_size);

    for (size_t i = 0; i < alloc_size_; i++) {
      ifs.read((char*)&(array_[i]), sizeof(unit_t));
    }
    return 0;
  }
};

#if 4 == 2
typedef DARTS::DoubleArrayImpl<char, unsigned char, short,
                               unsigned short> DoubleArray;
#define DARTS_ARRAY_SIZE_IS_DEFINED 1
#endif

#if 4 == 4 && !defined(DARTS_ARRAY_SIZE_IS_DEFINED)
typedef DARTS::DoubleArrayImpl<char, unsigned char, int,
                               unsigned int> DoubleArray;
#define DARTS_ARRAY_SIZE_IS_DEFINED 1
#endif

#if 4 == 4 && !defined(DARTS_ARRAY_SIZE_IS_DEFINED)
typedef DARTS::DoubleArrayImpl<char, unsigned char, long,
                               unsigned long> DoubleArray;
#define DARTS_ARRAY_SIZE_IS_DEFINED 1
#endif

#if 4 == 8 && !defined(DARTS_ARRAY_SIZE_IS_DEFINED)
typedef DARTS::DoubleArrayImpl<char, unsigned char, long long,
                               unsigned long long> DoubleArray;
#endif

}
