#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(32*1024)


enum cacheResType {MISS=0, HIT=1};

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
	static unsigned int addr=0;
	return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
	return rand_()%(DRAM_SIZE);
}

unsigned int memGen3()
{
	static unsigned int addr=0;
	return (addr++)%(1024*8);
}

unsigned int memGen4()
{
	static unsigned int addr=0;
	return (addr++)%(1024*64);
}

// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr)
{	
	// This function accepts the memory address for the read and 
	// returns whether it caused a cache miss or a cache hit

	// The current implementation assumes there is no cache; so, every transaction is a miss
	return MISS;
}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr, int line_size, unsigned int A[])
{	
  static int empty_slots = CACHE_SIZE / line_size;
  int slots = CACHE_SIZE / line_size;

  int offset_bits = log (line_size) / log (2);
  unsigned int tag = addr >> int(offset_bits);

  for(int i=0; i<slots;i++)
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
char *msg[2] = {"Miss","Hit"};

int main()
{
  srand(unsigned(time(NULL)));
	int inst=0;
  int line_size;
  int HITS = 0;
  int MISSES = 0;
  cacheResType r;
  unsigned int addr;
  unsigned int *A;

  cout<<"Enter cache line size (8, 16, 32, 64).\n> ";
  cin>>line_size;

  A = new unsigned int[CACHE_SIZE/line_size];

	cout << "Fully Associative Cache Simulator\n";
	for(;inst<1000000;inst++)
	{
		addr = memGen2();
		r = cacheSimFA(addr, line_size, A);

    if (r == HIT)
      HITS++;
    else
      MISSES++;

		//cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
	}

  cout <<"Hits: " <<dec <<HITS <<endl;
  cout <<"Misses: " <<dec <<MISSES <<endl;

  delete [] A;
}