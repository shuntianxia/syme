#include "cdlb_sort.h"

static int partition(quick_sort_t *qs, int low, int high)  
{  
    int key = qs->get_key(qs->get_elem(qs->arry,low)); //基准元素 
    while(low < high){ //从表的两端交替地向中间扫描 
    	while (low < high && 0 >= qs->compare(key,qs->get_key(qs->get_elem(qs->arry,high))))
        //while(low < high  && qs->get_key(qs->get_elem(qs->arry,high)) >= key )
            --high;  //从high 所指位置向前搜索，至多到low+1 位置。将比基准元素小的交换到低端
        qs->swap(qs->get_elem(qs->arry,low),qs->get_elem(qs->arry,high));
	while (low < high && 0 <= qs->compare(key,qs->get_key(qs->get_elem(qs->arry,low))))
        //while(low < high  && qs->get_key(qs->get_elem(qs->arry,low)) <= key)
            ++low;
        qs->swap(qs->get_elem(qs->arry,low),qs->get_elem(qs->arry,high));
    }
    
    return low;
}

void quick_sort(quick_sort_t *qs, int low, int high)
{
    int mid = 0;
    
    if(low < high) {
        mid = partition(qs,low,high); //将表一分为二  
        quick_sort(qs,low,mid-1); //递归对低子表递归排序  
        quick_sort(qs,mid+1,high); //递归对高子表递归排序  
    }
}


