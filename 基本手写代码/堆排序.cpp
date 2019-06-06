//由数组变为最大堆有两种方式，一种是逐个插入（O(n)），一种是heapify(从第一个非叶子结点开始调整，bottom to up)（O(1)）
//大话数据结构书上的排序基本都是数组的0位置不放元素，从1开始
//这里从0位置开始
#include <iostream>
#include <vector>
using namespace std;

void build_max_heap(vector<int> &a, int len);
void heapadjust(vector<int> &a, int i, int len);

void heap_sort(vector<int> &a) {
	int len = a.size();
	build_max_heap(a, len);
	int tmp = 0;
	for (int i = len - 1; i >= 1; i--) {
		cout << a[0] << endl;
		tmp = a[i];
		a[i] = a[0];
		a[0] = tmp;
		//当i = 1时，堆中只剩一个结点，此时不会再输出，只能跳出循环后再输出
		heapadjust(a, 0, i - 1);		//每次将数组前面的部分再调整为最大堆，后续调整的时候传进去的就是当前数组能取到的最大下标了，因为i赋初值时已经解决
	}
	cout << a[0] << endl;
}

void build_max_heap(vector<int> &a, int len) {	//建堆
	for (int i = len / 2 - 1; i >= 0; i--) {
		heapadjust(a, i, len - 1);				//建堆的时候应该传进去len-1，即当前数组能取到的最大下标，即保证堆调整过程中左右孩子不会数组越界
	}
}

void heapadjust(vector<int> &a, int i, int len){
	int temp, j;
	temp = a[i];

	for (j = 2*i+1; j <= len; j *= 2){
		//先比较左右孩子,找到左右孩子中较大的那个
		if (j < len && a[j] < a[j+1])
			++j;

		//如果左右孩子中最大值小于当前值，则不需要移动
		if (a[j] <= temp)
			break;
		//若最大值大于当前值，需要将最大值往前移动，赋给i位置
		//此时不要动j，直到找到j所在位置再赋值
		a[i] = a[j];

		//将当前值和左右孩子比较后，若移动了，则继续往下比较，将j向下移动到刚才较大的孩子处
		i = j;
	}
	//直到找到temp的合适位置
	a[i] = temp;
}

int main(){
	vector<int>a;
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);
	a.push_back(4);
	a.push_back(5);
	a.push_back(6);
	a.push_back(7);
	heap_sort(a);
	system("pause");
	return 0;
}
