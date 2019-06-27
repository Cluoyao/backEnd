//二叉树的前序，中序和后序遍历是根据根结点的位置来判断
//前序：根左右
//中序：左根右
//后序：左右根
//叶子结点也需要判断左右节点，只不过左右节点都是空


//非递归前序遍历
//(1) 输出根节点
//(2) 一直找到二叉树最左边的结点
//(3) 
void Pretravel(BiNode* root)
{
	if (!root)
	{
		return;
	}

	stack<BiNode*> st;
	BiNode* p = root;

	while (p || !st.empty())
	{
		while (p){
			cout << p->data;
			st.push(p);
			p = p->lchild;
		}
		

		if (!st.empty())
		{
			p = st.top();
			st.pop();
			p = p->rchild;
		}
	}
}

//非递归中序遍历
void Intravel(BiNode* root){
	if (root == NULL)
		return;
	stack<BiNode*>st;
	BiNode *p = root;

	while (!st.empty() || p){
		while (p){
			st.push(p);
			p = p->lchild;
		}

		if (!st.empty()){
			p = st.top();
			st.pop();
			cout << p->data;
			p = p->rchild;
		}
	}
}

//非递归后序遍历
void behtravel(BiNode* root){
	if (NULL == root)
		return;
	stack<BiNode *> st;
	BiNode * p = root;
	BiNode * pre = NULL;
	while (!st.empty || p){
		while (p){
			st.push(p);
			p = p->lchild;
		}
		if (!st.empty()){
			p = st.top();
			st.pop();

			if (!p->rchild || pre == p->rchild){
				cout << p->data;
				pre = p;
			}
			else{
				st.push(p);
				p = p->rchild;
			}
		}
	}
}