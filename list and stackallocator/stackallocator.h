#include <cstdlib>
#include <iostream>
#include <memory>

template <size_t N>
class StackStorage {
 private:
  alignas(std::max_align_t) char memory_[N];
  size_t top_;

 public:
  StackStorage();
  StackStorage(const StackStorage& other) = delete;
  ~StackStorage() = default;
  StackStorage& operator=(const StackStorage& other) = delete;

  void* allocate(size_t count, size_t align);
  void deallocate(void*, size_t);
};

template <size_t N>
StackStorage<N>::StackStorage() : memory_(), top_(0) {}

template <size_t N>
void* StackStorage<N>::allocate(size_t count, size_t align) {
  if ((top_ % align) != 0) {
    top_ += align - (top_ % align);
  }
  top_ += count;
  return reinterpret_cast<char *>(memory_) + top_ - count;
}

template <size_t N>
void StackStorage<N>::deallocate(void*, size_t) {
  //«nothing» ©mesyarik
}

template <typename T, size_t N>
class StackAllocator {
 public:
  using value_type = T;
  using propagate_on_container_swap = std::true_type;

  StackAllocator() noexcept;
  StackAllocator(StackStorage<N>& memory) noexcept;
  ~StackAllocator();

  value_type* allocate(size_t count);
  void deallocate(value_type *ptr, size_t count);
  StackStorage<N>* get_memory_ptr() const;

  template<typename U>
  StackAllocator(const StackAllocator<U, N>& other);

  template<typename U>
  StackAllocator& operator=(const StackAllocator<U, N>& other);

  template<typename U>
  bool operator==(const StackAllocator<U, N>& other);

  template<typename U>
  bool operator!=(const StackAllocator<U, N>& other);


  template<typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

 private:
  StackStorage<N>* memory_;
};

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator() noexcept {
  memory_();
}

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& memory) noexcept {
  memory_ = &memory;
}

template <typename T, size_t N>
StackAllocator<T, N>::~StackAllocator() = default;

template <typename T, size_t N>
StackAllocator<T, N>::value_type* StackAllocator<T, N>::allocate(size_t count) {
  static const size_t T_size = sizeof(T);
  return reinterpret_cast<value_type*>(memory_->allocate(count * T_size, T_size));
}

template <typename T, size_t N>
void StackAllocator<T, N>::deallocate(value_type *pointer, size_t count) {
  memory_->deallocate(pointer, count);
}

template<typename T, size_t N>
StackStorage<N>* StackAllocator<T, N>::get_memory_ptr() const {
  return memory_;
}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& other) {
  memory_ = other.get_memory_ptr();
}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(const StackAllocator<U, N>& other) {
  memory_ = other.get_memory_ptr();
  return *this;
}

template <typename T, size_t N>
template <typename U>
bool StackAllocator<T, N>::operator==(const StackAllocator<U, N>& other) {
  return memory_ == other.get_memory_ptr;
}

template <typename T, size_t N>
template <typename U>
bool StackAllocator<T, N>::operator!=(const StackAllocator<U, N>& other) {
  return !(this==other);
}

template<typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* prev;
    BaseNode* next;

    BaseNode();
    BaseNode(BaseNode* new_prev, BaseNode* new_next);
  };

  struct Node: BaseNode {
    T value;

    Node(BaseNode* prev, BaseNode* next);
    Node(const T& new_value, BaseNode* prev, BaseNode* next);
  };

  size_t size_;
  BaseNode fakeNode_;

  using NodeAlloc = std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using NodeTraits = std::allocator_traits<NodeAlloc>;
  [[no_unique_address]] NodeAlloc alloc_;

  template <bool is_const>
  class CommonIterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::conditional_t<is_const, const T, T>;
    using difference_type = long long;
    using pointer = value_type*;
    using reference = value_type&;

    CommonIterator();
    CommonIterator(const CommonIterator<is_const>& other);
    explicit CommonIterator(BaseNode* new_node);
    explicit CommonIterator(const BaseNode* new_node);
    CommonIterator& operator=(const CommonIterator<is_const>& other);
    ~CommonIterator();

    operator CommonIterator<true>() const;
    BaseNode* get_node() const;

    CommonIterator<is_const>& operator--();
    CommonIterator<is_const>& operator++();
    CommonIterator<is_const> operator--(int);
    CommonIterator<is_const> operator++(int);

    reference operator*() const;
    pointer operator->() const;

    bool operator==(const CommonIterator<is_const>& other) const;
    bool operator!=(const CommonIterator<is_const>& other) const;

   private:
    BaseNode* node_;
  };

 public:
  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List();
  explicit List(size_t count);
  List(size_t count, const T& value);
  explicit List(const Alloc& allocator);
  explicit List(size_t count, const Alloc& allocator);
  List(size_t count, const T& value, const Alloc& allocator);
  List(const List& other);
  ~List();
  void swap(List& other);
  List& operator=(const List& other);

  Alloc get_allocator() const;

  size_t size() const;

  void insert(const_iterator iter, const T& value);
  void erase(const_iterator iter);
  void push_back(const T& value);
  void push_front(const T& value);
  void pop_back();
  void pop_front();

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;
};

template<typename T, typename Alloc>
List<T, Alloc>::List() : size_(0), fakeNode_(&fakeNode_, &fakeNode_), alloc_(Alloc()) {}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t count):  List<T, Alloc>(count, Alloc()) {}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T& value) : List<T, Alloc>(count, value, Alloc()) {}

template<typename T, typename Alloc>
List<T, Alloc>::List(const Alloc& allocator)
                    : size_(0), fakeNode_(&fakeNode_, &fakeNode_), alloc_(allocator) {}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const Alloc& allocator) : List<T, Alloc>(allocator) {
  BaseNode* new_node;
  BaseNode* prev_node = &fakeNode_;
  for (size_t i = 0; i < count; ++i) {
    new_node = NodeTraits::allocate(alloc_, 1);
    try {
      NodeTraits::construct(alloc_, reinterpret_cast<Node*>(new_node), new_node, new_node);
    } catch (...) {;
      NodeTraits::deallocate(alloc_, reinterpret_cast<Node*>(new_node), 1);
      throw;
    }
    prev_node->next = new_node;
    new_node->prev = prev_node;
    prev_node = new_node;
    ++size_;
  }
  prev_node->next = &fakeNode_;
  (&fakeNode_)->prev = prev_node;
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T& value, const Alloc& allocator) : List<T, Alloc>(allocator) {
  for (size_t i = 0; i < count; ++i) {
    push_back(value);
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(const List<T, Alloc>& other)
  : List<T, Alloc> (std::allocator_traits<Alloc>::select_on_container_copy_construction(other.get_allocator())) {
  for (auto iter = other.begin(); iter != other.end(); ++iter) {
    insert(end(), *iter);
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::~List() {
  while (size_ > 0) {
    pop_front();
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::swap(List<T, Alloc>& other) {
  std::swap(size_, other.size_);
  std::swap(fakeNode_, other.fakeNode_);
  std::swap((&fakeNode_)->prev->next, (&other.fakeNode_)->prev->next);
  std::swap((&fakeNode_)->next->prev, (&other.fakeNode_)->next->prev);
}

template<typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& other) {
  auto tmp = other;
  if constexpr (NodeTraits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.get_allocator();
  }
  swap(tmp);
  return *this;
}

template<typename T, typename Alloc>
Alloc List<T, Alloc>::get_allocator() const {
  return alloc_;
}

template<typename T, typename Alloc>
size_t List<T, Alloc>::size() const {
  return size_;
}

template<typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator iter, const T &value) {
  BaseNode* new_node;
  BaseNode* next_node = iter.get_node();
  BaseNode* prev_node = next_node->prev;
  new_node = NodeTraits::allocate(alloc_, 1);
  try {
    NodeTraits::construct(alloc_, reinterpret_cast<Node*>(new_node), value, next_node, prev_node);
  } catch (...) {
    NodeTraits::deallocate(alloc_, reinterpret_cast<Node*>(new_node), 1);
    throw;
  }
  prev_node->next = new_node;
  new_node->prev = prev_node;
  new_node->next = next_node;
  next_node->prev = new_node;
  ++size_;
}

template<typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator iter) {
  BaseNode *delete_node = iter.get_node();
  delete_node->prev->next = delete_node->next;
  delete_node->next->prev = delete_node->prev;
  NodeTraits::destroy(alloc_, static_cast<Node*>(delete_node));
  NodeTraits::deallocate(alloc_, static_cast<Node*>(delete_node), 1);
  size_--;
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& value) {
  insert(end(), value);
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& value) {
  insert(begin(), value);
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  erase(std::prev(end()));
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  erase(begin());
}

template<typename T, typename Alloc>
List<T, Alloc>::iterator List<T, Alloc>::begin() {
  return iterator((&fakeNode_)->next);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator List<T, Alloc>::begin() const {
  return cbegin();
}

template<typename T, typename Alloc>
List<T, Alloc>::iterator List<T, Alloc>::end() {
  return iterator(&fakeNode_);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator List<T, Alloc>::end() const {
  return cend();
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator List<T, Alloc>::cbegin() const {
  return const_iterator((&fakeNode_)->next);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator List<T, Alloc>::cend() const {
  return const_iterator(&fakeNode_);
}

template<typename T, typename Alloc>
List<T, Alloc>::reverse_iterator List<T, Alloc>::rbegin() {
  return std::make_reverse_iterator(end());
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rbegin() const {
  return std::make_reverse_iterator(cend());
}

template<typename T, typename Alloc>
List<T, Alloc>::reverse_iterator List<T, Alloc>::rend() {
  return std::make_reverse_iterator(end());
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rend() const {
  return std::make_reverse_iterator(cend());
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crbegin() const {
  return std::make_reverse_iterator(cend());
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crend() const {
  return std::make_reverse_iterator(cend());
}

template<typename T, typename Alloc>
List<T, Alloc>::BaseNode::BaseNode() : prev(nullptr), next(nullptr) {}

template<typename T, typename Alloc>
List<T, Alloc>::BaseNode::BaseNode(BaseNode* new_prev, BaseNode* new_next)
                                  : prev(new_prev), next(new_next) {}

template<typename T, typename Alloc>
List<T, Alloc>::Node::Node(BaseNode* prev, BaseNode* next): BaseNode(prev, next) {}

template<typename T, typename Alloc>
List<T, Alloc>::Node::Node(const T& new_value, BaseNode* prev, BaseNode* next)
                          : BaseNode(prev, next), value(new_value) {}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::CommonIterator() = default;

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::CommonIterator(const CommonIterator<is_const>& other)
                                                        : node_(other.node_) {}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::CommonIterator(BaseNode* new_node)
                                                        : node_(new_node) {}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::CommonIterator(const BaseNode* new_node)
                                                        : node_(const_cast<BaseNode*>(new_node)) {}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>& List<T, Alloc>::CommonIterator<is_const>::operator=(const CommonIterator<is_const>& other) {
  node_ = other.node_;
  return *this;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::~CommonIterator() = default;

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::operator List<T, Alloc>::CommonIterator<true>() const {
  CommonIterator<true> const_iterator(node_);
  return const_iterator;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::BaseNode* List<T, Alloc>::CommonIterator<is_const>::get_node() const {
  return node_;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>& List<T, Alloc>::CommonIterator<is_const>::operator--() {
  node_ = node_->prev;
  return *this;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>& List<T, Alloc>::CommonIterator<is_const>::operator++() {
  node_ = node_->next;
  return *this;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const> List<T, Alloc>::CommonIterator<is_const>::operator--(int) {
  CommonIterator<is_const> result(*this);
  --(*this);
  return result;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const> List<T, Alloc>::CommonIterator<is_const>::operator++(int) {
  CommonIterator<is_const> result(*this);
  ++(*this);
  return result;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::reference List<T, Alloc>::CommonIterator<is_const>::operator*() const {
  return reinterpret_cast<Node*>(node_)->value;
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::CommonIterator<is_const>::pointer List<T, Alloc>::CommonIterator<is_const>::operator->() const {
  return &(reinterpret_cast<Node*>(node_)->value);
}

template<typename T, typename Alloc>
template<bool is_const>
bool List<T, Alloc>::CommonIterator<is_const>::operator==(const CommonIterator<is_const>& other) const {
  return (node_ == other.get_node());
}

template<typename T, typename Alloc>
template<bool is_const>
bool List<T, Alloc>::CommonIterator<is_const>::operator!=(const CommonIterator<is_const>& other) const {
  return !(*this == other);
}
