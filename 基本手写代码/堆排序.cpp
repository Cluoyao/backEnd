//�������Ϊ���������ַ�ʽ��һ����������루O(n)����һ����heapify(�ӵ�һ����Ҷ�ӽ�㿪ʼ������bottom to up)��O(1)��
//�����ݽṹ���ϵ�����������������0λ�ò���Ԫ�أ���1��ʼ
//�����0λ�ÿ�ʼ
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
		//��i = 1ʱ������ֻʣһ����㣬��ʱ�����������ֻ������ѭ���������
		heapadjust(a, 0, i - 1);		//ÿ�ν�����ǰ��Ĳ����ٵ���Ϊ���ѣ�����������ʱ�򴫽�ȥ�ľ��ǵ�ǰ������ȡ��������±��ˣ���Ϊi����ֵʱ�Ѿ����
	}
	cout << a[0] << endl;
}

void build_max_heap(vector<int> &a, int len) {	//����
	for (int i = len / 2 - 1; i >= 0; i--) {
		heapadjust(a, i, len - 1);				//���ѵ�ʱ��Ӧ�ô���ȥlen-1������ǰ������ȡ��������±꣬����֤�ѵ������������Һ��Ӳ�������Խ��
	}
}

void heapadjust(vector<int> &a, int i, int len){
	int temp, j;
	temp = a[i];

	for (j = 2*i+1; j <= len; j *= 2){
		//�ȱȽ����Һ���,�ҵ����Һ����нϴ���Ǹ�
		if (j < len && a[j] < a[j+1])
			++j;

		//������Һ��������ֵС�ڵ�ǰֵ������Ҫ�ƶ�
		if (a[j] <= temp)
			break;
		//�����ֵ���ڵ�ǰֵ����Ҫ�����ֵ��ǰ�ƶ�������iλ��
		//��ʱ��Ҫ��j��ֱ���ҵ�j����λ���ٸ�ֵ
		a[i] = a[j];

		//����ǰֵ�����Һ��ӱȽϺ����ƶ��ˣ���������±Ƚϣ���j�����ƶ����ղŽϴ�ĺ��Ӵ�
		i = j;
	}
	//ֱ���ҵ�temp�ĺ���λ��
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
