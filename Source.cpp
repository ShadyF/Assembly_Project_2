#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <map>
using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(32*1024)


enum cacheResType { MISS = 0, HIT = 1 };

unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

unsigned int rand_()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr = 0;   //static means that variable is saved through function calls
	return (addr++) % (DRAM_SIZE);
}

unsigned int memGen2()
{
	return rand_() % (DRAM_SIZE);
}

unsigned int memGen3()
{
	static unsigned int addr = 0;
	return (addr++) % (1024 * 8);
}

unsigned int memGen4()
{
	static unsigned int addr = 0;
	return (addr++) % (1024 * 64);
}

// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr, int linesize, map<unsigned int, unsigned int> &A)
{
	static map<unsigned int, unsigned int>::iterator it;
	static unsigned int blocks = CACHE_SIZE / linesize;
	addr = (addr / linesize);
	unsigned int index = addr % blocks;
	unsigned int tag = addr / blocks;
	it = A.find(index);
	if (it != A.end())
	{
		if (tag == it->second)
			return HIT;
		else
			A.erase(it);
	}
	A.insert(pair<unsigned int, unsigned int>(index, tag));
	return MISS;
}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr, int line_size, unsigned int A[])
{
	static int empty_slots = CACHE_SIZE / line_size;
	int slots = CACHE_SIZE / line_size;

	int offset_bits = log(line_size) / log(2);
	unsigned int tag = addr >> int(offset_bits);

	for (int i = 0; i<slots; i++)
	if (A[i] == tag)
		return HIT;

	//Puts address in ascending order at first
	if (empty_slots > 0)
	{
		A[slots - empty_slots] = tag;
		empty_slots--;
	}

	//Randomly if no valid slots
	else
		A[rand() % slots] = tag;

	return MISS;
}
char *msg[2] = { "Miss", "Hit" };

int main()
{
	srand(unsigned(time(NULL)));
	int inst = 0;
	int line_size;
	int HITS = 0;
	int MISSES = 0;
  int prompt;
	cacheResType r;
	unsigned int addr;
	unsigned int *A;
	map<unsigned int, unsigned int> TABLE;

  cout<<"Select Type of Cache:\n";
  cout<<"1. Direct Mapped\n";
  cout<<"2. Fully Associative\n>";
  cin>>prompt;

  if (prompt != 1 && prompt != 2)
  {
    cout <<"Incorrect input, type either 1 or 2\n> ";
    cin >>prompt;
  }
	cout << "Enter cache line size (8, 16, 32, 64).\n> ";
	cin >> line_size;

	A = new unsigned int[CACHE_SIZE / line_size];

  if(prompt == 1)
  {
	  cout<<"\nDirect Mapped Cache Simulator\n";

    for (; inst<1000000; inst++)
	  {
		  addr = memGen1();
		  r = cacheSimDM(addr, line_size, TABLE);

		  if (r == HIT)
			  HITS++;
		  else
			  MISSES++;

		 //cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
	  }
  }
  else
  {
    cout<<"\nFully Associative Cache Simulator\n";

    for (; inst<1000000; inst++)
	  {
		  addr = memGen4();
		  r = cacheSimFA(addr, line_size, A);

		  if (r == HIT)
			  HITS++;
		  else
			  MISSES++;

		  //cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
	  }
  }
  
	cout << "\nHits: " << dec << HITS << endl;
	cout << "Misses: " << dec << MISSES << endl <<endl;

  cout << "Hit Ratio: " <<(HITS/1000000.0) * 100 <<"%" <<endl;
  cout << "Miss Ratio: " <<100 - ((HITS/1000000.0) * 100) <<"%" <<endl <<endl;

	delete[] A;
	return 0;
}