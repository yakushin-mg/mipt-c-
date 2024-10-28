#include <iostream>
#include <cstring>

class String {
  private:
    char* array_ = new char[1];
    size_t size_ = 0;
    size_t capacity_ = 1;

    void swap(String& other) {
      std::swap(array_, other.array_);
      std::swap(size_, other.size_);
      std::swap(capacity_, other.capacity_);
    }

    void reallocate() {
      char* new_array_ = new char[capacity_];
      memcpy(new_array_, array_, size_);
      delete[] array_;
      array_ = new_array_;
    }
    
  public:
    String(size_t count): array_(new char[count + 1]), size_(count), capacity_(count + 1) {
      array_[size_] = '\0';
    }

    String(): String(static_cast<size_t>(0)) {}

    String(const char* line): String(strlen(line)) {
      memcpy(array_, line, size_);
    }

    String(size_t count, char symbol): String(count) {
      memset(array_, symbol, count);
    }

    String(const String& other): String(other.size_) {
      memcpy(array_, other.array_, size_);
    }

    String& operator=(String other) {
      swap(other);
      return *this;
    }

    ~String() { delete[] array_; }

    const char& operator[] (size_t index) const { return array_[index]; }

    char& operator[] (size_t index) { return array_[index]; }

    size_t length() const { return size_; }

    size_t size() const { return size_; }

    size_t capacity() const { return capacity_ - 1; }

    void push_back(char symbol) {
      size_t new_size_ = size_ + 1;
      if (new_size_ >= capacity_) {
        capacity_ = new_size_ * 2 + 1;
        reallocate();
      }
      size_ = new_size_;
      array_[size_ - 1] = symbol;
      array_[size_] = '\0';
    }
    
    void pop_back() {
      size_ = size_ - 1;
      array_[size_] = '\0';
    };

    const char& front() const { return array_[0]; }

    char& front() { return array_[0]; }

    const char& back() const { return array_[size_ - 1]; }

    char& back() { return array_[size_ - 1]; }

    String& operator+=(char symbol) {
      push_back(symbol);
      return *this;
    }

    String& operator+=(const String& other) {
      size_t new_size = size_ + other.size_;
      if (new_size >= capacity_) {
        capacity_ = new_size + 1;
        reallocate();
      }
      memcpy(array_ + size_, other.array_, other.size_);
      size_ = new_size;
      array_[size_] = '\0';
      return *this;
    }

    size_t find(const String& other) const {
      size_t index = 0;
      while (index <= size_ - other.size_) {
        bool result = memcmp(array_ + index, other.array_, other.size_);
        if (result == 0) {
          return index;
        }
        index++;
      }
      return length();
    }

    size_t rfind(const String& other) const {
      size_t index = size_ - other.size_;
      while (index > 0) {
        bool result = memcmp(array_ + index, other.array_, other.size_);
        if (result == 0) {
          return index;
        }
        index--;
      }
      bool result = memcmp(array_ + index, other.array_, other.size_);
      if (result == 0) {
        return index;
      }
      return length();
    }

    String substr(size_t start, size_t count) const {
      String new_string(count, 'a');
      memcpy(new_string.array_, array_ + start, count);
      return new_string;
    }

    bool empty() const {
      return (!size_);
    }

    void clear() { size_ = 0; }

    void shrink_to_fit() {
      capacity_ = size_ + 1;
      reallocate();
      array_[size_] = '\0';
    }

    char* data() const { return array_; };
};

bool operator==(const String& first_string, const String& second_string) {
  size_t size1 = first_string.size();
  size_t size2 = second_string.size();
  if (size1 == size2) {
    return !(memcmp(first_string.data(), second_string.data(), size1));
  }
  return false;
}

bool operator!=(const String& first_string, const String& second_string) {
  return !(first_string == second_string);
}

bool operator<(const String& first_string, const String& second_string) {
  size_t size1 = first_string.size();
  size_t size2 = second_string.size();
  if (size1 < size2) {
    return true;
  }
  if (size1 == size2)  {
    if ((memcmp(first_string.data(), second_string.data(), size1)) < 0) {
      return true;
    }
  }
  return false;
}

bool operator>(const String& first_string, const String& second_string) {
  return (second_string < first_string);
}

bool operator<=(const String& first_string, const String& second_string) {
  return !(first_string > second_string);
}

bool operator>=(const String& first_string, const String& second_string) {
  return !(first_string < second_string);
}

String operator+(const String& first_string, const String& second_string) {
  String result = first_string;
  result += second_string;
  return result;
}

String operator+(const String& first_string, char symbol) {
  String result;
  result += first_string;
  result += symbol;
  return result;
}

String operator+(char symbol, const String& second_string) {
  String result;
  result += symbol;
  result += second_string;
  return result;
}

std::ostream& operator<<(std::ostream& out, const String& other) {
  size_t index = 0;
  while (index < other.size()) {
    out << other[index];
    index++;
  }
  return out;
}

std::istream& operator>>(std::istream &in, String& other) {
  other.clear();
  char symbols;
  while (in.get(symbols) && (!std::isspace(symbols))) {
    other.push_back(symbols);
  }
  return in;
}
