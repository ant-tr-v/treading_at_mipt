#include <thread>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <limits>

void my_sort(std::vector<int>::iterator begin, std::vector<int>::iterator end, int threads){
  auto size = static_cast<int>(end - begin);
  if(threads > 1){
    std::thread left(my_sort, begin, begin + (size + 1)/2,  (threads + 1)/ 2);
    std::thread right(my_sort, begin + (size + 1)/2, end,  (threads)/ 2);
    left.join();
    right.join();
    std::vector<int>tmp(static_cast<unsigned long>(size));
    std::merge(begin, begin + (size + 1)/2, begin + (size + 1)/2, end, tmp.begin());
    std::copy(tmp.begin(), tmp.end(), begin);
  }else if(threads == 1) {
    std::sort(begin, end);
  }
}


int main(int argc, char *argv[]){
  if (argc <= 2) {
    std::cerr << "Not enough input data";
    return -1;
  }
  int N;
  int p;
  try {
    N = std::stoi(argv[1]);
    p = std::stoi(argv[2]);
  }catch(...){
    std::cerr << "Input error";
    return -1;
  }
  if (N < 1 || p < 1) {
    std::cerr << "Too low";
    return -1;
  }
  std::vector<int> v(static_cast<unsigned long>(N), std::numeric_limits<int>::max());
  std::mt19937 gen(static_cast<unsigned long>(time(nullptr)));

  std::uniform_int_distribution<int> distribution(//-10, 10);
      std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
  //generating vectors
  for (auto i = 0; i < N; ++i) {
    v[i] = distribution(gen);
  }
  auto t = std::chrono::high_resolution_clock::now();
  my_sort(v.begin(), v.end(), p);
  //output may bee too long
  /*for(auto el: v){
    std::cout << el << " ";
  }*/
  std::cout <<"\n"<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count() <<" ms";
  return 0;
}