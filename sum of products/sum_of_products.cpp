include <iostream>

void FillingIndexes(long long* indexes, size_t number) {
  for (size_t i = 0; i < number; i++) {
    indexes[i] = 0;
  }
}

long long Multiplications(int** arrays, size_t index_array, size_t number, long long* indexes, char* argv[]) {
  long long summ = 0;
  long long multiply;
  size_t len = atoi(argv[index_array + 1]);
  for (size_t i = 0; i < len; i++) {
    if (indexes[i] != 1) {
      indexes[i] = 1;
      if (index_array != (number - 1)) {
        multiply = arrays[index_array][i];
        multiply *= (Multiplications(arrays, index_array + 1, number, indexes, argv));
        summ += multiply;
      } else {
        summ += arrays[index_array][i];
      }
      indexes[i] = 0;
    }
  }
  return summ;
}
int main(int argc, char* argv[]) {
  size_t number = argc - 1;
  int** arrays = new int* [number];
  size_t max_len = 0;
  size_t len;
  for (size_t i = 0; i < number; i++) {
    len = atoi(argv[i + 1]);
    if (len > max_len) {
      max_len = len;
    }
    arrays[i] = new int [len];
    for (size_t j = 0; j < len; j++) {
      std::cin >> arrays[i][j];
    }
  }
  long long* indexes = new long long [max_len];
  FillingIndexes(indexes, number);
  std::cout << std::endl;
  long long summ = Multiplications(arrays, 0, number, indexes, argv);
  std::cout << summ << std::endl;
  for (size_t i = 0; i < number; ++i) {
    delete[] arrays[i];
  }
  delete[] arrays;
  delete[] indexes;
}
