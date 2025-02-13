#include <iostream>
#include <thread>
#include <atomic>
#include <x86intrin.h>

#define INIT_PUSH 1000000
#define MAX_THREAD_NUM 100
#define MAX_VOLUME 10000000

class DBStack {
public:
	DBStack() : head(nullptr) {}
	
	void push(int d) {
		Node* newNode = new Node;
		newNode->d = d;
		Node* oldHead;

		do {
			oldHead = head.load();	// read head
			newNode->p = oldHead;	// new node point to old head
		} while (!head.compare_exchange_weak(oldHead, newNode));
	}

	int pop() {
		Node* oldHead;
		do {
			oldHead = head.load();
			if (oldHead == nullptr) {
				return 0;	// stack is empty
			}
		} while (!head.compare_exchange_weak(oldHead, oldHead->p));
		
		int temp = oldHead->d;
		delete oldHead;
		return temp;
	}

	bool isEmpty() {
		return head.load() == nullptr;
	}

	void display();

private:
	struct Node {
		int d;
		Node* p;
	};
	
	std::atomic<Node*> head;	// atomic head pointer
};

void testStack (DBStack* toTest, const int volume, int threadNum) {
	for (int i = 0; i < volume; i++) {
		int randNum = rand() % volume;
		int pushOrPop = i%2;
		if (pushOrPop) {
			toTest->push(randNum);
		} else {
			toTest->pop();
		}
	}
}

int main (int argc, char** argv) {		
	srand(time(NULL));
	DBStack toTest;
	int maxThreads = 0;
	
	if (argc > 1) {
		maxThreads = atoi(argv[1]);
	} else {
		printf ("no arguments :( \n");
		return 0;
	}
	
	std::thread thr[maxThreads];
	
	for (int i = 0; i < INIT_PUSH; i++) {
		int randVal = rand() % INIT_PUSH;
		toTest.push(randVal);
	}
	
	uint64_t tick = __rdtsc() / 100000;
	
	for (int i = 0; i < maxThreads; i++) {
		thr[i] = std::thread (testStack, &toTest, MAX_VOLUME / maxThreads, i);
	}
	
	for (int i = 0; i < maxThreads; i++) {
		thr[i].join();
	}
	
	uint64_t tick2 = __rdtsc() / 100000;
	printf ("%d, %lu, \n", maxThreads, tick2 - tick);
	
	return 0;
}