#include <pthread.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <iostream>
#include <functional>

struct Argument{
  pthread_mutex_t *mutex;
  double *result;
  double a, b, h;
  std::function<double(double)> const *func;
};

void parse_arg(int argc, char *argv[], double &h, int &cpus){
  if(argc  < 3)
    throw std::runtime_error("Incorrect input");
  try{
    h = std::stod(argv[1]);
    cpus = std::stoi(argv[2]);
  }catch(...){
    throw std::runtime_error("Parsing failed");
  }
  if (h < 1e-11){
    throw std::runtime_error("h is too low");
  }
  if (cpus < 1){
    throw std::runtime_error("Cant ran in less than 1 thread");
  }
}

void * integrate_thread(void * arg){
  auto p = reinterpret_cast<Argument *>(arg);
  double func = (*(p -> func))(p -> a), new_funk, t = p -> a;
  double res  = 0;
  while(t < p -> b - p-> h/2){
    auto h = std::min(p -> h, p -> b - t);
    t += h;
    new_funk = (*(p -> func))(t);
    res +=  (new_funk + func) * h / 2;
    func = new_funk;
  }
  pthread_mutex_lock(p->mutex);
  *(p -> result) += res;
  pthread_mutex_unlock(p->mutex);
  return nullptr;
}

double integrate(std::function<double(double)> const &func, double a, double b, double h, int threads_num){
  std::vector<Argument> args(static_cast<unsigned long>(threads_num));
  std::vector<pthread_t> threads(static_cast<unsigned long>(threads_num));
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  if (b < a){
    std::swap(a, b);
  }
  double result = 0;
  double step = (b - a) / threads_num;
  for(auto i = 0; i < threads_num; ++i){
    args[i] = {&mutex, &result, a + step * i, a + step * (i + 1), h, &func};
    pthread_create(threads.data() + i, nullptr, integrate_thread, args.data() + i);
  }
  for(auto i = 0; i < threads_num; ++i) {
    pthread_join(threads[i], nullptr);
  }
  return result;
}

double y(double x){
  return cos(1/(x + 3));
}

int main(int argc, char *argv[]){
  double h;
  int th;
  try{
    parse_arg(argc, argv, h, th);
  }catch(std::runtime_error &err){
    std::cerr << err.what();
    return -1;
  }
  auto t = std::chrono::high_resolution_clock::now();
  std::cout.precision(15);
  auto res = integrate(y, -2.999, 0, h, th);
  std::cout << res << "(" << (res - 1.59535790382934)  <<")\n\t";
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count() <<" ms";
}