#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 100;  

int main(int argc, char **argv) {
    int stack_var = 200; 

    printf("---- Таблица страниц при старте ----\n\n");
    pages(0, 0, 0);

    int size = 3 * 4096;  
    char *heap_array = sbrk(size);
    if ((uint64)heap_array == -1) {
        printf("sbrk failed\n");
        exit(1);
    }

    memset(heap_array, 0, size); 
    printf("\n---- После выделения кучи (%d байт) ----\n\n", size);
    pages(0, 0, 0);

    printf("\n---- Страницы, содержащие часть кучи (ее 4096 байт с отступом на 10)) ----\n\n");
    pages((uint64 *)(heap_array + 10), 4096, 0);

    printf("\n---- После снятия A и D куску кучи выше ----\n\n");
    strip_flags((uint64 *)(heap_array + 10), 4096, 3);
    pages(0, 0, 0);

    printf("\n---- После снятия A и D всем ----\n\n");
    strip_flags(0, 0, 3);
    pages(0, 0, 0);

    int temp = global_var + stack_var + heap_array[0];
    printf("%d\n", temp); // to prevent unused var varning
    printf("\n---- После чтения данных (ожидается установка бита A) ----\n\n");
    pages(0, 0, 2); 

    global_var += 1;
    stack_var += 1;
    heap_array[0] += 1;
    printf("\n---- После изменения данных (ожидается установка бита D) ----\n\n");
    pages(0, 0, 1);  

    sbrk(-size);
    printf("\n---- После освобождения кучи ----\n\n");
    pages(0, 0, 0);

    exit(0);
}