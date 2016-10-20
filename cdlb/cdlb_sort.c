#include "cdlb_sort.h"

static int partition(quick_sort_t *qs, int low, int high)  
{  
    int key = qs->get_key(qs->get_elem(qs->arry,low)); //��׼Ԫ�� 
    while(low < high){ //�ӱ�����˽�������м�ɨ�� 
    	while (low < high && 0 >= qs->compare(key,qs->get_key(qs->get_elem(qs->arry,high))))
        //while(low < high  && qs->get_key(qs->get_elem(qs->arry,high)) >= key )
            --high;  //��high ��ָλ����ǰ���������ൽlow+1 λ�á����Ȼ�׼Ԫ��С�Ľ������Ͷ�
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
        mid = partition(qs,low,high); //����һ��Ϊ��  
        quick_sort(qs,low,mid-1); //�ݹ�Ե��ӱ�ݹ�����  
        quick_sort(qs,mid+1,high); //�ݹ�Ը��ӱ�ݹ�����  
    }
}


