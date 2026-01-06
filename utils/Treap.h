#pragma once

#include <algorithm>
#include <memory>
#include <semaphore>
#include <thread>
#include <vector>

template <typename Value, typename Compare>
concept comparable = requires(Value a, Value b, Compare cmp) {
  { cmp(a, b) } -> std::convertible_to<bool>;
};

template <typename Func, typename Arg>
concept nonVoidFunction = requires(Func f, Arg a) {
  { f(a) } -> std::same_as<std::invoke_result_t<Func, Arg>>;
} && (!std::same_as<std::invoke_result_t<Func, Arg>, void>);

template <typename Value, typename Compare>
concept TreapRequirements = std::copyable<Value> && comparable<Value, Compare>;

template <typename Value, typename Compare>
  requires TreapRequirements<Value, Compare>
class Treap
{
public:
  using value_type = Value;
  using comparator = Compare;
  using size_type  = size_t;

private:
  struct node;
  using node_ptr = std::unique_ptr<node>;
  struct node
  {
    value_type val;
    size_type pri, sz;
    node_ptr l, r;
    node(value_type v)
    {
      val = v;
      pri = rand();
      sz  = 1;
    }
  };

  comparator mCmp;

  // functions can't be static because of these
  inline bool value_lt(const value_type &a, const value_type &b) const
  {
    return mCmp(a, b);
  }
  inline bool value_gt(const value_type &a, const value_type &b) const
  {
    return mCmp(b, a);
  }
  inline bool value_le(const value_type &a, const value_type &b) const
  {
    return !mCmp(b, a);
  }
  inline bool value_ge(const value_type &a, const value_type &b) const
  {
    return !mCmp(a, b);
  }
  inline bool value_eq(const value_type &a, const value_type &b) const
  {
    return value_le(a, b) && value_ge(a, b);
  }

  inline size_type sz(const node_ptr &t) const { return t ? t->sz : 0; }
  inline void upd(node_ptr &t) const
  {
    if (t) t->sz = sz(t->l) + sz(t->r) + 1;
  }

  template <typename Func> void inorder(Func f, const node_ptr &t) const
  {
    if (!t) return;
    inorder(f, t->l);
    f(t->val);
    inorder(f, t->r);
  }

  // Func f needs to be thread-safe
  template <typename Func>
  void idx_traversal(const node_ptr &t, Func f, std::counting_semaphore<> &sem,
                     size_t min_tree_size, size_t base_idx = 0) const
  {
    if (!t) return;

    bool spawn_left = (t->l && sz(t) >= min_tree_size && sem.try_acquire());

    f(t->val, base_idx + sz(t->l));

    std::thread left_thread;
    if (spawn_left)
    {
      left_thread = std::thread(
          [&]()
          {
            idx_traversal(t->l, f, sem, min_tree_size, base_idx);
            sem.release();
          });
    }
    else if (t->l)
      idx_traversal(t->l, f, sem, min_tree_size, base_idx);

    if (t->r)
      idx_traversal(t->r, f, sem, min_tree_size, base_idx + sz(t->l) + 1);

    if (left_thread.joinable()) left_thread.join();
  }

  // split a treap into 2 treaps based on values
  void split(node_ptr &l, node_ptr &r, node_ptr &&t, value_type val) const
  { // <= val in l
    if (!t)
    {
      l = nullptr;
      r = nullptr;
      return;
    }
    node_ptr n = nullptr;
    if (value_le(t->val, val))
    {
      split(n, r, std::move(t->r), val);
      l    = std::move(t);
      l->r = std::move(n);
      upd(l);
    }
    else
    {
      split(l, n, std::move(t->l), val);
      r    = std::move(t);
      r->l = std::move(n);
      upd(r);
    }
  }

  // split treap into 2 treaps based on number of elements
  void splitat(node_ptr &l, node_ptr &r, node_ptr &&t, size_type pos) const
  { // first pos elements at l
    if (!t)
    {
      l = nullptr;
      r = nullptr;
      return;
    }
    node_ptr n;
    if (sz(t->l) < pos)
    {
      splitat(n, r, std::move(t->r), pos - sz(t->l) - 1);
      l    = std::move(t);
      l->r = std::move(n);
      upd(l);
    }
    else
    {
      splitat(l, n, std::move(t->l), pos);
      r    = std::move(t);
      r->l = std::move(n);
      upd(r);
    }
  }

  // find kth value in a treap
  value_type &kth(const node_ptr &t, size_type k) const
  {
    if (!t) throw std::out_of_range("cannot find element");
    if (sz(t->l) == k - 1) return t->val;
    if (sz(t->l) < k - 1)
      return kth(t->r, k - sz(t->l) - 1);
    else
      return kth(t->l, k);
  }

  // check if key is in treap
  void find(const node_ptr &t, value_type key, bool &found) const
  {
    if (!t) return;
    if (value_eq(t->val, key))
    {
      found = true;
      return;
    }
    if (value_le(t->val, key))
      return find(t->r, key, found);
    else
      return find(t->l, key, found);
  }
  void find(const node_ptr &t, value_type key, bool &found,
            size_type &ind) const
  {
    if (!t) return;
    if (value_eq(t->val, key))
    {
      found = true;
      ind += sz(t->l);
      return;
    }
    if (value_le(t->val, key))
    {
      ind += sz(t->l) + 1;
      return find(t->r, key, found);
    }
    else
      return find(t->l, key, found);
  }

  // insert new node (only for fresh node l=r=nullptr)
  void insert(node_ptr &t, node_ptr &&n) const
  {
    if (!t)
    {
      t = std::move(n);
      return;
    }
    if (n->pri > t->pri)
    {
      split(n->l, n->r, std::move(t), n->val);
      t = std::move(n);
    }
    else if (value_le(t->val, n->val))
      insert(t->r, std::move(n));
    else
      insert(t->l, std::move(n));
    upd(t);
  }

  // concat two treaps
  void concat(node_ptr &t, node_ptr &&l, node_ptr &&r) const
  {
    if (!l)
    {
      t = std::move(r);
      return;
    }
    if (!r)
    {
      t = std::move(l);
      return;
    }
    if (l->pri > r->pri)
    {
      concat(l->r, std::move(l->r), std::move(r));
      t = std::move(l);
    }
    else
    {
      concat(r->l, std::move(r->l), std::move(l));
      t = std::move(r);
    }
    upd(t);
  }

  // merge two treaps
  void merge(node_ptr &t, node_ptr &&l, node_ptr &&r)
  {
    if (!l)
    {
      t = std::move(r);
      return;
    }
    if (!r)
    {
      t = std::move(l);
      return;
    }
    node_ptr a, b;
    if (l->pri < r->pri) std::swap(l, r);
    split(a, b, std::move(r), l->val);
    merge(l->l, std::move(l->l), std::move(a));
    merge(l->r, std::move(l->r), std::move(b));
    t = std::move(l);
    upd(t);
  }

public:
  Treap(Compare c) : mCmp{c}, mTop{nullptr} { srand(time(NULL)); }
  Treap(Treap<Value, Compare> &&t)
      : mCmp{std::move(t.mCmp)}, mTop{std::move(t.mTop)}
  {
    srand(time(NULL));
  }

  size_type size() const { return sz(mTop); }

  void insert(value_type &&v)
  {
    insert(mTop, std::make_unique<node>(std::move(v)));
  }

  void restrict(size_type target_size)
  {
    if (target_size > size())
    {
      throw std::runtime_error("cannot expand using restrict");
    }
    node_ptr l, r;
    splitat(l, r, std::move(mTop), target_size);
    mTop = std::move(l);
  }

  bool contains(const value_type &v) const
  {
    bool found = false;
    find(mTop, v, found);
    return found;
  }

  int indexOf(const value_type &v) const
  {
    bool found    = false;
    size_type ind = 0;
    find(mTop, v, found, ind);
    if (!found) return -1;
    return ind;
  }

  const value_type &operator[](size_type k) const { return kth(mTop, k + 1); }

  void merge(Treap<Value, Compare> &&other)
  {
    merge(mTop, std::move(mTop), std::move(other.mTop));
  }

  template <typename Func> void iterate(Func f) const { inorder(f, mTop); }

  // like fmap but return is not sorted
  template <typename Func>
    requires nonVoidFunction<Func, value_type>
  auto gather(Func f, size_t max_thread = 8, size_t min_tree_size = 1) const
  {
    using ResultType = std::invoke_result_t<Func, value_type>;

    std::mutex mtx;
    std::vector<std::pair<size_t, ResultType>> results;

    auto store = [&](const value_type &val, size_t idx)
    {
      auto r = f(val);
      std::lock_guard<std::mutex> lock(mtx);
      results.emplace_back(idx, std::move(r));
    };

    std::counting_semaphore thread_sem(max_thread);
    idx_traversal(mTop, store, thread_sem, min_tree_size);

    return results;
  }

  template <typename Func>
    requires nonVoidFunction<Func, value_type>
  auto fmap(Func f, size_t max_thread = 8, size_t min_tree_size = 1) const
  {
    using ResultType = std::invoke_result_t<Func, value_type>;
    std::vector<ResultType> mapped;

    auto results = gather(f, max_thread, min_tree_size);
    std::sort(results.begin(), results.end(),
              [](auto &a, auto &b) { return a.first < b.first; });

    mapped.reserve(results.size());
    for (auto &p : results) mapped.push_back(std::move(p.second));
    return mapped;
  }

  template <typename Func>
  const value_type &max(Func f, const value_type &def) const
  {
    const value_type *best = &def;
    inorder(
        [&](const value_type &v)
        {
          if (f(v) > f(*best)) best = &v;
        },
        mTop);
    return *best;
  }

  friend std::ostream &operator<<(std::ostream &os, const Treap &t)
  {
    t.inorder([&os](const value_type &v) { os << v << ' '; }, t.mTop);
    return os;
  }

private:
  node_ptr mTop;
};
