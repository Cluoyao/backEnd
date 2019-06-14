//归并排序思想：先分治再归并。待排序列有n个记录，将其分治为长度为1的n个子序列，然后两两归并，如此重复归并，得到长度为n的有序序列

//归并排序，平均情况O(nlogn),最好O(nlogn)，最坏O(nlogn),，空间复杂度O(n)，稳定

//两个序列从相同位置往右遍历比较，根据大小重新排列在新数组中的前提是，左右两个序列都是从小到大排列的
//
//先递归将左右归并为有序序列，再将左右进行归并
//将左右有序归并时，两个序列从相同位置往右遍历比较，根据大小重新排列在新数组中
//核心函数是merge，Msort函数是递归分治调用merge的过程
#include <iostream>
#include <vector>
using namespace std;

//函数是将sr两侧归并的数据，重新排列进tr中
//sr[s...m, m+1...t]
void merge(int sr[], int tr[], int s, int m, int t){

	int i = s, j = m + 1, k = s;

	//只拷贝左边
	while (i <= m) //sr[low...mid] copy 到 tr[low...mid]
	{
		tr[i] = sr[i];
		++i;
	}

	for (i, j, k; i <= m && j <= t; k++){
		if (tr[i] < sr[j])
			sr[k] = tr[i++];
		else
			sr[k] = sr[j++];
	}
	//当两侧数据长度并非完全相等时，只需要将左侧剩余数据拷贝即可
	//因为右侧直接在原数组上操作，所以不用管右侧
	while (i <= m)
		sr[k++] = tr[i++];

}

//sr[s, t]原数组，tr临时数组
void msort(int sr[], int tr[], int s, int t){

	if (s >= t)
		return;

	//创建平分中点
	//int m = (s+t) / 2;
	int m = s + (t - s) / 2;

	//将sr[s,m]和sr[m+1, r]分别归并到tr数组
	//再将tr两侧的数据排序到sr中
	msort(sr, tr, s, m);
	msort(sr, tr, m+1, t);

	//优化，因为左右两侧都是排序好的数组，只有当左侧最大大于右侧最小时才合并，因为合并意味着比较，会浪费时间
	if (sr[m] > sr[m+1])
		merge(sr, tr, s, m, t);

}

int main(){

	int sr[10] = {10,9,8,7,6,5,4,3,2,1};
	int *tr = (int *)malloc(sizeof(int) * 10);
	msort(sr, tr, 0, 9);

	for (int i = 0; i < 10; i++){
		cout << sr[i] << endl;
	}
	free(tr);
	system("pause");
	return 0;
}
