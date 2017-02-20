#include<stdio.h>
#include<pthread.h>
#include<assert.h>
#include<stdlib.h>

pthread_cond_t myCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t myLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct ListNode
{
	int _data;
	struct ListNode* _next;
}Node,*pNode,**ppNode;

pNode AllocNode(int data)
{
	pNode NewNode = (pNode)malloc(sizeof(Node));
	if(NewNode == NULL)
	{
		perror("malloc");
		return NULL;
	}
	NewNode->_next = NULL;
	NewNode->_data = data;
	return NewNode;
}

void DeallocNode(pNode node)
{
	if(node != NULL)
		free(node);
}

int IsEmpty(pNode head)
{
	assert(head);
	if(head -> _next == NULL)
		return 1;
	else
		return 0;
}

void InitList(ppNode l)
{
	assert(l);
	//申请头结点
	pNode head = AllocNode(0);
	*l = head;
	if(head == NULL)
	{
		printf("申请头结点失败\n");
		perror("AllocNode");
	}
	return ;
}

void PushNode(pNode head,int data)//头插
{
	assert(head);
	pNode NewNode = AllocNode(data);
	if(NewNode == NULL)
	{
		perror("AllocNode");
		return;
	}
	NewNode->_next = head->_next;
	head->_next = NewNode;
}

void PopNode(pNode head)//头删
{
	assert(head);
	if(IsEmpty(head))
		return;
	Node* del = head->_next;
	head->_next = del->_next;
	DeallocNode(del);
}

void DestroyList(pNode head)
{
	assert(head);
	Node* cur = head->_next;
	while(cur)
	{
		PopNode(head);
		cur = cur->_next;
	}
	DeallocNode(head);
}

void ShowList(pNode head)
{
	assert(head);
	Node* cur = head->_next;
	while(cur)
	{
		printf("%d ",cur->_data);
		cur = cur->_next;
	}
	printf("\n");
}
void* Product(void* h)
{
	assert(h);
	pNode head = h;
	//生产者放结点
	while(1)
	{
	 	sleep(1);
		int data = rand()%1000;
		pthread_mutex_lock(&myLock);
		PushNode(head,data);
		printf("Product:%d\n",data);
		pthread_mutex_unlock(&myLock);
		pthread_cond_signal(&myCond);
	}
	return NULL;
}

void* Consumer(void* h)
{
	assert(h);
	pNode head = h;
	while(1)
	{
		pthread_mutex_lock(&myLock);
		while(IsEmpty(head))
		{
			pthread_cond_wait(&myCond,&myLock);
		}
		printf("Consumer:%d\n",head->_next->_data);
		PopNode(head);
		pthread_mutex_unlock(&myLock);
	}
	return NULL;
}
void test()
{
	pNode head;
	InitList(&head);
	if(head == NULL)
		return;
	pthread_t tid1;
	pthread_t tid2;
	pthread_create(&tid1,NULL,Product,(void*)head);
	pthread_create(&tid2,NULL,Consumer,(void*)head);

	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	return ;
	/*int i = 0;
	for(i = 0; i < 10; ++i)
	{
		PushNode(head,i);
		ShowList(head);
	}
	for(; i >= 3; --i)
	{
		PopNode(head);
		ShowList(head);
	}*/
}

int main()
{
	test();
	return 0;
}
