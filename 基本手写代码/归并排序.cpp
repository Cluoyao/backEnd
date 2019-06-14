//�鲢����˼�룺�ȷ����ٹ鲢������������n����¼���������Ϊ����Ϊ1��n�������У�Ȼ�������鲢������ظ��鲢���õ�����Ϊn����������

//�鲢����ƽ�����O(nlogn),���O(nlogn)���O(nlogn),���ռ临�Ӷ�O(n)���ȶ�

//�������д���ͬλ�����ұ����Ƚϣ����ݴ�С�����������������е�ǰ���ǣ������������ж��Ǵ�С�������е�
//
//�ȵݹ齫���ҹ鲢Ϊ�������У��ٽ����ҽ��й鲢
//����������鲢ʱ���������д���ͬλ�����ұ����Ƚϣ����ݴ�С������������������
//���ĺ�����merge��Msort�����ǵݹ���ε���merge�Ĺ���
#include <iostream>
#include <vector>
using namespace std;

//�����ǽ�sr����鲢�����ݣ��������н�tr��
//sr[s...m, m+1...t]
void merge(int sr[], int tr[], int s, int m, int t){

	int i = s, j = m + 1, k = s;

	//ֻ�������
	while (i <= m) //sr[low...mid] copy �� tr[low...mid]
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
	//���������ݳ��Ȳ�����ȫ���ʱ��ֻ��Ҫ�����ʣ�����ݿ�������
	//��Ϊ�Ҳ�ֱ����ԭ�����ϲ��������Բ��ù��Ҳ�
	while (i <= m)
		sr[k++] = tr[i++];

}

//sr[s, t]ԭ���飬tr��ʱ����
void msort(int sr[], int tr[], int s, int t){

	if (s >= t)
		return;

	//����ƽ���е�
	//int m = (s+t) / 2;
	int m = s + (t - s) / 2;

	//��sr[s,m]��sr[m+1, r]�ֱ�鲢��tr����
	//�ٽ�tr�������������sr��
	msort(sr, tr, s, m);
	msort(sr, tr, m+1, t);

	//�Ż�����Ϊ�������඼������õ����飬ֻ�е�����������Ҳ���Сʱ�źϲ�����Ϊ�ϲ���ζ�űȽϣ����˷�ʱ��
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
