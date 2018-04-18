#include <pthread.h>
#include <vector>
#include <queue>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <functional>

struct Task{
  double a, b, sigm;
};

struct Argument{
  pthread_mutex_t *mutex;
  std::queue<Task> *queue; // tasks queue
  double *result;
  std::function<double(double)> const *func;
  int *n;
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
  if (cpus < 1){
    throw std::runtime_error("Cant ran in less than 1 thread");
  }
}

double get_step(double a, double b,  std::function<double(double)> const &func, double sigma){
  std::minstd_rand0 gen(pthread_self());
  std::uniform_real_distribution<double> dist(a, b);
  static const int N = 50; /// MAGICAL CONST - number of points to calculate second derivative in
  double max_der = 0;
  double  h = 1e-12; /// MAGICAL CONST - step of second derivative
  for(int i = 0; i < N; ++i){
    double p = dist(gen);
    double t = (func(p - h) - 2*func(p) + func(p + h))/ h / h;
    max_der = std::max(max_der, std::abs(t));
  }
  double d = (b - a);
  return sqrt(sigma * 3 / (max_der + 1e-13) / d);
}

void * integrate_thread(void * arg){
  auto p = reinterpret_cast<Argument *>(arg);
  while(true) {
    pthread_mutex_lock(p->mutex);
    if((p->queue)->empty()) {
      pthread_mutex_unlock(p->mutex);
      break;
    }
    Task task = (p->queue)->front();
    (p->queue)->pop();
    pthread_mutex_unlock(p->mutex);
    double h = get_step(task.a, task.b, *(p -> func), task.sigm);
    //std::cout << task.a << " " << task.b << " " << h << std::endl;
    double func = (*(p->func))(task.a), new_funk, t = task.a;
    double res = 0;
    while (t < task.b - h / 2) {
      auto h0 = std::min(h, task.b - t);
      t += h0;
      new_funk = (*(p->func))(t);
      res += (new_funk + func) * h0 / 2;
      func = new_funk;
    }
    pthread_mutex_lock(p->mutex);
    *(p->result) += res;
    ++*(p->n);
    pthread_mutex_unlock(p->mutex);
  }
  return nullptr;
}

double integrate(std::function<double(double)> const &func, double a, double b, double prec, int threads_num){
  std::vector<Argument> args(static_cast<unsigned long>(threads_num));
  std::vector<pthread_t> threads(static_cast<unsigned long>(threads_num));
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  int n = 0;
  if (b < a){
    std::swap(a, b);
  }
  double result = 0;
  std::queue <Task> task_queue;
  static const int M = 40; /// MAGICAL CONST - number of tasks
  auto d = (b - a) / (M);
  for(auto i = 0; i <  M; ++i){
    task_queue.push({a + d*i, a + d*(i + 1), prec/ M});
  }

  for(auto i = 0; i < threads_num; ++i){
    args[i] = {&mutex, &task_queue, &result, &func, &n};
    pthread_create(threads.data() + i, nullptr, integrate_thread, args.data() + i);
  }
  for(auto i = 0; i < threads_num; ++i) {
    pthread_join(threads[i], nullptr);
  }
  std::cout << n << "\n";
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
  auto res = integrate(y, -2.99, 0, h, th);
  std::cout << res << "(" << (res - 1.59530474926)  <<")\n\t";
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count() <<" ms";
}