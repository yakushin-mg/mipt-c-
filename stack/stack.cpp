#include <cstring>
#include <iostream>

struct Node {
  char* string;
  Node* previous;
};

void Push(Node*& top) {
  size_t size = 16;
  char* line = new char[size];
  size_t index_string = 0;
  char symbol;
  std::cin.get(symbol);
  while (std::cin.get(symbol) && symbol != '\n') {
    line[index_string++] = symbol;
    if (index_string >= size - 2) {
      char* new_line = new char[2 * size];
      memcpy(new_line, line, size);
      delete[] line;
      line = new_line;
      size *= 2;
    }
  }
  line[index_string] = '\0';
  Node* new_top = new Node;
  new_top->string = line;
  new_top->previous = top;
  top = new_top;
}

void Pop(Node*& current) {
  Node* previous = current->previous;
  delete[] current->string;
  delete current;
  current = previous;
}

void Clear(Node*& top) {
  while (top != nullptr) {
    Pop(top);
  }
}

int main() {
  Node* top = nullptr;
  size_t size = 0;
  while (true) {
    char request[6];
    std::cin >> request;
    if (!memcmp(request, "push", 4)) {
      Push(top);
      size++;
      std::cout << "ok" << std::endl;
    } else if (!memcmp(request, "pop", 3)) {
      if (size == 0) {
        std::cout << "error" << std::endl;
      } else {
        std::cout << top->string << std::endl;
        Pop(top);
        size--;
      }
    } else if (!memcmp(request, "back", 4)) {
      if (size == 0) {
        std::cout << "error" << std::endl;
      } else {
        std::cout << top->string << std::endl;
      }
    } else if (!memcmp(request, "size", 4)) {
      std::cout << size << std::endl;
    } else if (!memcmp(request, "clear", 5)) {
      Clear(top);
      size = 0;
      std::cout << "ok" << std::endl;
    } else {
      Clear(top);
      std::cout << "bye" << std::endl;
      break;
    }
  }
}
