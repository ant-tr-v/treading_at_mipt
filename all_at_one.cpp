#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

struct Argument{
  ulong *x;
  ulong n;
};

void *parallel_inc(void * arg){
  auto p = reinterpret_cast<Argument *>(arg);
  while(*(p->x) != p->n);
  //usleep(1);
  std::cout << "thread #" << p->n << std::endl;
  ++*(p->x);
  return nullptr;
}

ulong parse_arg(int argc, char *argv[]){
  ulong res;
  if(argc  < 2)
    throw std::runtime_error("Incorrect input");
  try{
    res = std::stoul(argv[1]);
  }catch(...){
    throw std::runtime_error("Parsing failed");
  }
  return res;
}

int main(int argc, char *argv[]) {
  ulong N;
  try{
    N = parse_arg(argc, argv);
  }catch(std::runtime_error &err){
    std::cerr << err.what();
    return -1;
  }
  std::vector<Argument> args(N);
  std::vector<pthread_t> threads(N);
  ulong X = 0;
  for(ulong i = N - 1; i > 0; --i){
    args[i] = {&X, i};
    int ret;
    if((ret = pthread_create(threads.data() + i, nullptr, parallel_inc,
                      reinterpret_cast<void *>(args.data() + i))) != 0){
      std::cerr << "pthread_create() terminated with code " << ret;
      return -1;
    }
  }
  std::cout << "Master thread" << std::endl;
  ++X;
  for(ulong i = 1; i < N; ++i){
    pthread_join(threads[i], nullptr);
  }
  return 0;
}