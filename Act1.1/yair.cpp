#include <iostream>
#include <vector>

void merge(std::vector<int> &vec, int start, int mid, int end,
           std::vector<int> &temp) {

  // Use indexes instead of sizes
  int i = start;   // left start
  int j = mid + 1; // right start
  int k = start;   // temp index

  // Compare and copy to temp at the same time
  while (i <= mid && j <= end) {
    // if left > than right then put left
    if (vec[i] <= vec[j]) {
      temp[k] = vec[i];
      i++;
      // if not, copy right to temp
    } else {
      temp[k] = vec[j];
      j++;
    }
    k++;
  }

  // copy everything left on the left
  while (i <= mid) {
    temp[k] = vec[i];
    i++;
    k++;
  }

  // copy everything left on the right
  while (j <= end) {
    temp[k] = vec[j];
    j++;
    k++;
  }

  std::cout << "{";

  // copy everything in temp to the vector
  for (int p = start; p <= end; p++) {
    vec[p] = temp[p];
    std::cout << " " << vec[p] << " ";
  }
  
  std::cout << "}" << std::endl;

};

void mergeSortEngine(std::vector<int> &vec, int start, int end,
                       std::vector<int> &temp) {
  if ( start >= end) {
    return;
  }

  int mid = (start + end) / 2;

  mergeSortEngine(vec, start, mid, temp);
  mergeSortEngine(vec, mid + 1, end, temp);

  merge(vec, start, mid, end, temp);
}

void mergeSort(std::vector<int> &vec) {
  std::vector<int> temp(vec.size());
  mergeSortEngine(vec, 0, vec.size(), temp);
};

int main() {

  std::vector<int> vector = {1, 5, 67, 19, 5};

  mergeSort(vector);
  return 0;
}