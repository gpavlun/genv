#include <stdbool.h>
#include <stdio.h>

#define result(type) type##_result_t

typedef struct{
  int result; 
  bool ok;
}int_result_t;

result(int) divide(int x, int y){
  if(!y){
    result(int) ret = {
      .ok = false,
      .result = 0
    };
    
    return ret;
  }

  result(int) ret = {
    .ok = true,
    .result = x/y
  };
  return ret;
}







int main(void){





  return 0;
}