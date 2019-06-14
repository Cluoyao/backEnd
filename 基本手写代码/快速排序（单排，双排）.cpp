//��������˼�룺ѡ��һ���ڱ���ͨ��һ�����򽫴������зָ�������֣�����һ����С���ڱ�ֵ������һ���ִ����ڱ�ֵ��Ȼ���ٶ������ֱַ��������������ֱ���������

//��������ƽ�����O(nlogn),���O(nlogn)���O(n^2)��ѡ��궨Ԫ���йأ�,���ռ临�Ӷ�O(nlogn)~O(n)�����ȶ�

/**************************************************************************
**	Ҫȡ��[a,b)�����������ʹ��(rand() % (b-a))+ a;
**	Ҫȡ��[a,b]�����������ʹ��(rand() % (b-a+1))+ a;
**	Ҫȡ��(a,b]�����������ʹ��(rand() % (b-a))+ a + 1;
**	ͨ�ù�ʽ:a + rand() % n�����е�a����ʼֵ��n�������ķ�Χ��
**	Ҫȡ��a��b֮��������������һ�ֱ�ʾ��a + (int)b * rand() / (RAND_MAX + 1)��
**	Ҫȡ��0��1֮��ĸ�����������ʹ��rand() / double(RAND_MAX)��
*************************************************************************/



#include <iostream>
using namespace std;

void swapOffer(int &a, int &b){
	int temp = a;
	a = b;
	b = temp;
}

//��·�������
int partition(int *data, int start, int end){
	//����start��end֮��������
	int index = (rand() % (end - start + 1)) + start;
	
	//��ѡ�е����ֽ�����startλ��
	swap(data[index], data[start]);

	int pivot = data[start];

	//ѡ���pivotΪstartλ��
	//data[start+1, small] < v   smallΪС��v�����һ��Ԫ��
	//data[small+1, i - 1] >= v   small+1Ϊ����v�ĵ�һ��Ԫ��
	int small = start;
	for (int i = start + 1; i <= end; ++i){
		if (data[i] < pivot){
			//����ǰ��Ԫ��С��pivot����Ҫ����Ԫ�طŵ�data[start+1, small]�н�����smallλ��
			//��small+1��i���н���������small�ĳ��ȼӳ�
			swap(data[small + 1], data[i]);
			++small;
		}
	}
	//���start����Ӧ�õ�λ�ã���small��small+1֮�䣬��Ϊ���ȫ��С��pivot�ģ���˽�small��pivot����
	swap(data[small], data[start]);
	return small;
}

//˫·����
int partition2(int *data, int start, int end){
	//����start��end֮��������
	int index = (rand() % (end - start + 1)) + start;

	//��ѡ�е����ֽ�����startλ��
	swap(data[index], data[start]);

	int pivot = data[start];

	//ѡ���pivotΪstartλ��
	//data[start+1, i) <= pivot   i-1 ΪС��v�����һ��Ԫ�أ�iΪ��ǰ��߷��ʵ�Ԫ��
	//data(j, end] >= pivot  j+1 Ϊ����v�ĵ�һ��Ԫ�أ�jΪ��ǰ�ұ߷��ʵ�Ԫ��
	int i = start + 1, j = end;
	while (true){
		while (i <= end && data[i] <= pivot)
			i++;
		while (j >= start + 1 && data[j] >= pivot)
			j--;
		if (i > j)
			break;
		swap(data[i], data[j]);

		//���������У�������ȵ�����ʱ����������(�������û��дdata[i] <= pivot����д�ˣ������ʡ��)
		i++;
		j--;
	}
	//���jֹͣ��<= v�����һ��λ�ã� iֹͣ��>=pivot�ĵ�һ��λ�ã�pivot��j���н���
	swap(data[start], data[j]);
	return j;
}


void quickSortOffer(int *data,int start, int end){
	if (start == end)
		return;
	if (start < end){
		int index = partition2(data,  start, end);
		quickSortOffer(data,  start, index - 1);
		quickSortOffer(data, index + 1, end);
	}
}

int main(){
	int arr[8] = { 2, 1, 3, 78, 78,53, 13, 20 };
	quickSortOffer(arr, 0, 7);  //0��ʾ������0λ�ÿ�ʼ����4λ������ 
	int i;
	for (i = 0; i<8; ++i){
		printf("%d ", arr[i]);
	}
	system("pause");
	return 0;
}